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

static constexpr bool DEBUG = true;

static void *moduleDex;
static size_t moduleDexSize;

static constexpr size_t APP_DATA_DIR_SIZE = 128;
static char lastAppDataDir[APP_DATA_DIR_SIZE];

static void updateAppDataDir(JNIEnv *env, jstring appDataDir) {
    DEBUG("updateAppDataDir");
    if (appDataDir == nullptr) {
        DEBUG("dir is null");
        memset(lastAppDataDir, 0, APP_DATA_DIR_SIZE);
    } else {
        DEBUG("copy dir");
        const char *copy = env->GetStringUTFChars(appDataDir, NULL);
        strncpy(lastAppDataDir, copy, APP_DATA_DIR_SIZE);
        env->ReleaseStringUTFChars(appDataDir, copy);
        DEBUG(lastAppDataDir);
    }
}

static void specializeCommon(JNIEnv *env) {
    DEBUG("specializeCommon");
    DEBUG(lastAppDataDir);
    if (moduleDex == nullptr) {
        DEBUG("dex null");
    }
    if (moduleDex == nullptr || strstr(lastAppDataDir, "com.google.android.gms") == nullptr) {
        DEBUG("pkg doesn't match");
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

    // Run it
    DEBUG("load class method lookup");
    jmethodID loadClass = env->GetMethodID(clClass, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
    DEBUG("call load class");
    // Create a Java string. VM crashes with a C string.
    jstring entryClassName = env->NewStringUTF("dev.kdrag0n.safetynetriru.EntryPoint");
    jobject entryClassObj = env->CallObjectMethod(dexCl, loadClass, entryClassName);

    // Call init
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
    // Close the fd. This doesn't destroy the mapping.
    //DEBUG("close");
    char *data = (char *) malloc(*fileSize);
    int bytes = 0;
    while (bytes < *fileSize) {
        bytes += read(fd, data + bytes, *fileSize - bytes);
    }
    close(fd);

    return data;
}

static void forkAndSpecializePre(
        JNIEnv *env, jclass clazz, jint *uid, jint *gid, jintArray *gids, jint *runtimeFlags,
        jobjectArray *rlimits, jint *mountExternal, jstring *seInfo, jstring *niceName,
        jintArray *fdsToClose, jintArray *fdsToIgnore, jboolean *is_child_zygote,
        jstring *instructionSet, jstring *appDataDir, jboolean *isTopApp, jobjectArray *pkgDataInfoList,
        jobjectArray *whitelistedDataInfoList, jboolean *bindMountAppDataDirs, jboolean *bindMountAppStorageDirs) {
    updateAppDataDir(env, *appDataDir);
}

static void specializeAppProcessPre(
        JNIEnv *env, jclass clazz, jint *uid, jint *gid, jintArray *gids, jint *runtimeFlags,
        jobjectArray *rlimits, jint *mountExternal, jstring *seInfo, jstring *niceName,
        jboolean *startChildZygote, jstring *instructionSet, jstring *appDataDir,
        jboolean *isTopApp, jobjectArray *pkgDataInfoList, jobjectArray *whitelistedDataInfoList,
        jboolean *bindMountAppDataDirs, jboolean *bindMountAppStorageDirs) {
    updateAppDataDir(env, *appDataDir);
}

static void forkAndSpecializePost(JNIEnv *env, jclass clazz, jint res) {
    // Called "after" com_android_internal_os_Zygote_nativeForkAndSpecialize in frameworks/base/core/jni/com_android_internal_os_Zygote.cpp
    // "res" is the return value of com_android_internal_os_Zygote_nativeForkAndSpecialize

    if (res == 0) {
        // In app process
        specializeCommon(env);
    }
}

static void specializeAppProcessPost(
        JNIEnv *env, jclass clazz) {
    // Called "after" com_android_internal_os_Zygote_nativeSpecializeAppProcess in frameworks/base/core/jni/com_android_internal_os_Zygote.cpp
    specializeCommon(env);
}

static void onModuleLoaded() {
    // Called when this library is loaded and "hidden" by Riru (see Riru's hide.cpp)

    // If you want to use threads, start them here rather than the constructors
    // __attribute__((constructor)) or constructors of static variables,
    // or the "hide" will cause SIGSEGV


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
