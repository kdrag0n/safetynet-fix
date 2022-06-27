#include <cstdlib>
#include <string>
#include <vector>
#include <unistd.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <android/log.h>

#include "zygisk.hpp"
#include "module.h"

namespace safetynetfix {

class SafetyNetFixModule : public zygisk::ModuleBase {
public:
    void onLoad(zygisk::Api *api, JNIEnv *env) override {
        this->api = api;
        this->env = env;
    }

    void preAppSpecialize(zygisk::AppSpecializeArgs *args) override {
        const char *rawProcess = env->GetStringUTFChars(args->nice_name, nullptr);
        if (rawProcess == nullptr) {
            return;
        }

        std::string process(rawProcess);
        env->ReleaseStringUTFChars(args->nice_name, rawProcess);

        preSpecialize(process);
    }

    void postAppSpecialize(const zygisk::AppSpecializeArgs *args) override {
        // Inject if module was loaded, otherwise this would've been unloaded by now (for non-GMS)
        if (!moduleDex.empty()) {
            LOGD("Injecting payload...");
            injectPayload();
            LOGI("Payload injected");
        }
    }

    void preServerSpecialize(zygisk::ServerSpecializeArgs *args) override {
        // Never tamper with system_server
        api->setOption(zygisk::DLCLOSE_MODULE_LIBRARY);
    }

private:
    zygisk::Api *api;
    JNIEnv *env;

    std::vector<char> moduleDex;

    static int receiveFile(int remote_fd, std::vector<char>& buf) {
        off_t size;
        int ret = read(remote_fd, &size, sizeof(size));
        if (ret < 0) {
            LOGE("Failed to read size");
            return -1;
        }

        buf.resize(size);

        int bytesReceived = 0;
        while (bytesReceived < size) {
            ret = read(remote_fd, buf.data() + bytesReceived, size - bytesReceived);
            if (ret < 0) {
                LOGE("Failed to read data");
                return -1;
            }

            bytesReceived += ret;
        }

        return bytesReceived;
    }

    void loadPayload() {
        auto fd = api->connectCompanion();

        auto size = receiveFile(fd, moduleDex);
        LOGD("Loaded module payload: %d bytes", size);

        close(fd);
    }

    void preSpecialize(std::string process) {
        // Only take action for GMS, otherwise unload
        if (process.rfind("com.google.android.gms", 0) != 0) {
            // If this isn't GMS, bail out and unload.
            api->setOption(zygisk::DLCLOSE_MODULE_LIBRARY);
            return;
        }

        // Force DenyList unmounting for all GMS processes
        api->setOption(zygisk::FORCE_DENYLIST_UNMOUNT);

        // The unstable process is where SafetyNet attestation actually runs, so we only need to
        // spoof the model in that process. Leaving other processes alone fixes various issues
        // caused by model detection and flag provisioning, such as broken weather with the new
        // smartspace on Android 12.
        if (process == "com.google.android.gms.unstable") {
            // Load the payload, but don't inject it yet until after specialization
            // Otherwise, specialization fails if any code from the payload still happens to be
            // running
            LOGD("Loading payload...");
            loadPayload();
            LOGD("Payload loaded");
        }
    }

    void injectPayload() {
        // First, get the system classloader
        LOGD("get system classloader");
        auto clClass = env->FindClass("java/lang/ClassLoader");
        auto getSystemClassLoader = env->GetStaticMethodID(clClass, "getSystemClassLoader",
                                                           "()Ljava/lang/ClassLoader;");
        auto systemClassLoader = env->CallStaticObjectMethod(clClass, getSystemClassLoader);

        // Assuming we have a valid mapped module, load it. This is similar to the approach used for
        // Dynamite modules in GmsCompat, except we can use InMemoryDexClassLoader directly instead of
        // tampering with DelegateLastClassLoader's DexPathList.
        LOGD("create buffer");
        auto buf = env->NewDirectByteBuffer(moduleDex.data(), moduleDex.size());
        LOGD("create class loader");
        auto dexClClass = env->FindClass("dalvik/system/InMemoryDexClassLoader");
        auto dexClInit = env->GetMethodID(dexClClass, "<init>",
                                          "(Ljava/nio/ByteBuffer;Ljava/lang/ClassLoader;)V");
        auto dexCl = env->NewObject(dexClClass, dexClInit, buf, systemClassLoader);

        // Load the class
        LOGD("load class");
        auto loadClass = env->GetMethodID(clClass, "loadClass",
                                               "(Ljava/lang/String;)Ljava/lang/Class;");
        auto entryClassName = env->NewStringUTF("dev.kdrag0n.safetynetfix.EntryPoint");
        auto entryClassObj = env->CallObjectMethod(dexCl, loadClass, entryClassName);

        // Call init. Static initializers don't run when merely calling loadClass from JNI.
        LOGD("call init");
        auto entryClass = (jclass) entryClassObj;
        auto entryInit = env->GetStaticMethodID(entryClass, "init", "()V");
        env->CallStaticVoidMethod(entryClass, entryInit);
    }
};

static off_t sendFile(int remote_fd, const std::string& path) {
    auto in_fd = open(path.c_str(), O_RDONLY);
    if (in_fd < 0) {
        LOGE("Failed to open file %s: %d (%s)", path.c_str(), errno, strerror(errno));
        return -1;
    }

    auto size = lseek(in_fd, 0, SEEK_END);
    if (size < 0) {
        LOGERRNO("Failed to get file size");
        close(in_fd);
        return -1;
    }
    lseek(in_fd, 0, SEEK_SET);

    // Send size first for buffer allocation
    int ret = write(remote_fd, &size, sizeof(size));
    if (ret < 0) {
        LOGERRNO("Failed to send size");
        close(in_fd);
        return -1;
    }

    ret = sendfile(remote_fd, in_fd, nullptr, size);
    if (ret < 0) {
        LOGERRNO("Failed to send data");
        close(in_fd);
        return -1;
    }

    close(in_fd);
    return size;
}

static void companionHandler(int remote_fd) {
    // Serve module dex
    auto size = sendFile(remote_fd, MODULE_DEX_PATH);
    LOGD("Sent module payload: %ld bytes", size);
}

}

REGISTER_ZYGISK_COMPANION(safetynetfix::companionHandler)
REGISTER_ZYGISK_MODULE(safetynetfix::SafetyNetFixModule)
