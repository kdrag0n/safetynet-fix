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
        std::string process(rawProcess);
        env->ReleaseStringUTFChars(args->nice_name, rawProcess);

        preSpecialize(process);
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
        int fd = api->connectCompanion();

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
            // This is post-fork, so just inject the payload now
            loadPayload();
            injectPayload();
            LOGI("Payload injected");
        }
    }

    void injectPayload() {
        // First, get the system classloader
        LOGD("get system classloader");
        jclass clClass = env->FindClass("java/lang/ClassLoader");
        jmethodID getSystemClassLoader = env->GetStaticMethodID(clClass, "getSystemClassLoader",
                                                                "()Ljava/lang/ClassLoader;");
        jobject systemClassLoader = env->CallStaticObjectMethod(clClass, getSystemClassLoader);

        // Assuming we have a valid mapped module, load it. This is similar to the approach used for
        // Dynamite modules in GmsCompat, except we can use InMemoryDexClassLoader directly instead of
        // tampering with DelegateLastClassLoader's DexPathList.
        LOGD("create buffer");
        jobject buf = env->NewDirectByteBuffer(moduleDex.data(), moduleDex.size());
        LOGD("create class loader");
        jclass dexClClass = env->FindClass("dalvik/system/InMemoryDexClassLoader");
        jmethodID dexClInit = env->GetMethodID(dexClClass, "<init>",
                                               "(Ljava/nio/ByteBuffer;Ljava/lang/ClassLoader;)V");
        jobject dexCl = env->NewObject(dexClClass, dexClInit, buf, systemClassLoader);

        // Load the class
        LOGD("load class");
        jmethodID loadClass = env->GetMethodID(clClass, "loadClass",
                                               "(Ljava/lang/String;)Ljava/lang/Class;");
        jstring entryClassName = env->NewStringUTF("dev.kdrag0n.safetynetriru.EntryPoint");
        jobject entryClassObj = env->CallObjectMethod(dexCl, loadClass, entryClassName);

        // Call init. Static initializers don't run when merely calling loadClass from JNI.
        LOGD("call init");
        auto entryClass = (jclass) entryClassObj;
        jmethodID entryInit = env->GetStaticMethodID(entryClass, "init", "()V");
        env->CallStaticVoidMethod(entryClass, entryInit);
    }
};

static off_t sendFile(int remote_fd, const std::string& path) {
    int in_fd = open(path.c_str(), O_RDONLY);
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
