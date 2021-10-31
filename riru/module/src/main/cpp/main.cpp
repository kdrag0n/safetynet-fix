#include <jni.h>
#include <sys/types.h>
#include <riru.h>
#include <malloc.h>
#include <cstring>
#include <config.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <android/log.h>

#ifndef NDEBUG
#define DEBUG(...) __android_log_write(ANDROID_LOG_DEBUG, "SafetyNetRiru/JNI", __VA_ARGS__)
#else
#define DEBUG(...)
#endif

static void *moduleDex;
static size_t moduleDexSize;

static bool gmsSpecializePending = false;

static void updateNiceName(JNIEnv *env, jstring niceName) {
    const char *copy = env->GetStringUTFChars(niceName, NULL);
    // The unstable process is where SafetyNet attestation actually runs, so we only need to
    // spoof the model in that process. Leaving other processes alone fixes various issues
    // caused by model detection and flag provisioning, such as broken weather with the new
    // smartspace on Android 12.
    gmsSpecializePending = !strcmp(copy, "com.google.android.gms.unstable");
    env->ReleaseStringUTFChars(niceName, copy);
}

static void specializeCommon(JNIEnv *env) {
    DEBUG("specializeCommon");
    if (!moduleDex || !gmsSpecializePending) {
        DEBUG("dex null or specialize not pending");
        riru_set_unload_allowed(true);
        return;
    }

    DEBUG("get system classloader");
    // First, get the system classloader
    jclass clClass = env->FindClass("java/lang/ClassLoader");
    jmethodID getSystemClassLoader = env->GetStaticMethodID(clClass, "getSystemClassLoader", "()Ljava/lang/ClassLoader;");
    jobject systemClassLoader = env->CallStaticObjectMethod(clClass, getSystemClassLoader);

    DEBUG("create buf");
    // Assuming we have a valid mapped module, load it. This is similar to the approach used for
    // Dynamite modules in GmsCompat, except we can use InMemoryDexClassLoader directly instead of
    // tampering with DelegateLastClassLoader's DexPathList.
    jobject buf = env->NewDirectByteBuffer(moduleDex, moduleDexSize);
    DEBUG("construct dex cl");
    jclass dexClClass = env->FindClass("dalvik/system/InMemoryDexClassLoader");
    jmethodID dexClInit = env->GetMethodID(dexClClass, "<init>", "(Ljava/nio/ByteBuffer;Ljava/lang/ClassLoader;)V");
    jobject dexCl = env->NewObject(dexClClass, dexClInit, buf, systemClassLoader);

    // Load the class
    DEBUG("load class method lookup");
    jmethodID loadClass = env->GetMethodID(clClass, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
    DEBUG("call load class");
    jstring entryClassName = env->NewStringUTF("dev.kdrag0n.safetynetriru.EntryPoint");
    jobject entryClassObj = env->CallObjectMethod(dexCl, loadClass, entryClassName);

    // Call init. Static initializers don't run when merely calling loadClass from JNI.
    DEBUG("call init");
    auto entryClass = (jclass) entryClassObj;
    jmethodID entryInit = env->GetStaticMethodID(entryClass, "init", "()V");
    env->CallStaticVoidMethod(entryClass, entryInit);
    DEBUG("specializeCommon end");
}

static void *readFile(char *path, size_t *fileSize) {
    int fd = open(path, O_RDONLY, 0);
    if (fd < 0) {
        DEBUG("open fail");
        return nullptr;
    }

    // Get size
    DEBUG("get size");
    *fileSize = lseek(fd, 0, SEEK_END);
    if (*fileSize < 0) {
        DEBUG("seek fail");
        return nullptr;
    }
    lseek(fd, 0, SEEK_SET);

    // Map
    /*
    DEBUG("mmap");
    moduleDex = mmap(nullptr, *fileSize, PROT_READ, MAP_PRIVATE, fd, 0);
    if (moduleDex == MAP_FAILED) {
        DEBUG("mmap fail");
    }*/

    // Read the entire file into a buffer
    // TODO: see if mmap path is visible in /proc/pid/maps after closing and forking
    char *data = (char *) malloc(*fileSize);
    int bytes = 0;
    while (bytes < *fileSize) {
        bytes += read(fd, data + bytes, *fileSize - bytes);
    }

    // Close the fd. This doesn't destroy the mapping.
    DEBUG("close");
    close(fd);

    return data;
}

static void forkAndSpecializePre(
        JNIEnv *env, jclass clazz, jint *uid, jint *gid, jintArray *gids, jint *runtimeFlags,
        jobjectArray *rlimits, jint *mountExternal, jstring *seInfo, jstring *niceName,
        jintArray *fdsToClose, jintArray *fdsToIgnore, jboolean *is_child_zygote,
        jstring *instructionSet, jstring *appDataDir, jboolean *isTopApp, jobjectArray *pkgDataInfoList,
        jobjectArray *whitelistedDataInfoList, jboolean *bindMountAppDataDirs, jboolean *bindMountAppStorageDirs) {
    updateNiceName(env, *niceName);
}

static void specializeAppProcessPre(
        JNIEnv *env, jclass clazz, jint *uid, jint *gid, jintArray *gids, jint *runtimeFlags,
        jobjectArray *rlimits, jint *mountExternal, jstring *seInfo, jstring *niceName,
        jboolean *startChildZygote, jstring *instructionSet, jstring *appDataDir,
        jboolean *isTopApp, jobjectArray *pkgDataInfoList, jobjectArray *whitelistedDataInfoList,
        jboolean *bindMountAppDataDirs, jboolean *bindMountAppStorageDirs) {
    updateNiceName(env, *niceName);
}

static void forkAndSpecializePost(JNIEnv *env, jclass clazz, jint res) {
    if (res == 0) {
        // Child process
        specializeCommon(env);
    }
}

static void specializeAppProcessPost(JNIEnv *env, jclass clazz) {
    specializeCommon(env);
}

static void onModuleLoaded() {
    // Load
    DEBUG("onModuleLoaded, loading file");
    char pathBuf[128];
    snprintf(pathBuf, 128, "%s/%s", riru_magisk_module_path, "classes.dex");
    DEBUG((char*)riru_magisk_module_path);
    DEBUG(pathBuf);

    moduleDex = readFile(pathBuf, &moduleDexSize);
    if (!moduleDex) {
        return;
    }

    DEBUG("module loaded");
}

extern "C" {

int riru_api_version;
const char *riru_magisk_module_path = nullptr;
int *riru_allow_unload = nullptr;

static auto module = RiruVersionedModuleInfo{
    .moduleApiVersion = riru::moduleApiVersion,
    .moduleInfo = RiruModuleInfo{
        .supportHide = true,
        .version = riru::moduleVersionCode,
        .versionName = riru::moduleVersionName,
        .onModuleLoaded = onModuleLoaded,
        .forkAndSpecializePre = forkAndSpecializePre,
        .forkAndSpecializePost = forkAndSpecializePost,
        .forkSystemServerPre = NULL,
        .forkSystemServerPost = NULL,
        .specializeAppProcessPre = specializeAppProcessPre,
        .specializeAppProcessPost = specializeAppProcessPost,
    },
};

RiruVersionedModuleInfo *init(Riru *riru) {
    auto core_max_api_version = riru->riruApiVersion;
    riru_api_version = core_max_api_version <= riru::moduleApiVersion ? core_max_api_version : riru::moduleApiVersion;
    module.moduleApiVersion = riru_api_version;

    riru_magisk_module_path = strdup(riru->magiskModulePath);
    if (riru_api_version >= 25) {
        riru_allow_unload = riru->allowUnload;
    }
    return &module;
}

}
