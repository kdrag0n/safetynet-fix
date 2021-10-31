#pragma once

namespace safetynetfix {

static constexpr auto TAG = "SafetyNetFix/JNI";

static constexpr auto MODULE_DEX_PATH = "/data/adb/modules/safetynet-fix/classes.dex";

#ifdef NDEBUG
#define LOGD(...)
#else
#define LOGD(...)     __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)
#endif

#define LOGI(...)     __android_log_print(ANDROID_LOG_INFO,  TAG, __VA_ARGS__)
#define LOGE(...)     __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)
#define LOGERRNO(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__ ": %d (%s)", errno, strerror(errno))

}
