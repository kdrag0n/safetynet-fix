#define LOG_TAG "keystore"

#include <stdint.h>
#include <dlfcn.h>
#include <string>
#include <fstream>
#include <android/log.h>
#include <binder/Parcel.h>
#include <binder/IPCThreadState.h>

static android::status_t interceptTransaction(uint32_t code, uint32_t attestKey_code) {
    // TRANSACTION_attestKey, varies by Android version
	if (code == attestKey_code) {
        pid_t pid = android::IPCThreadState::self()->getCallingPid();
        std::string cmdline;
        std::ifstream cmdlineFile("/proc/" + std::to_string(pid) + "/cmdline");
        std::getline(cmdlineFile, cmdline);
        cmdlineFile.close();

        std::string gmsPrefix("com.google.android.gms");
        if (!cmdline.compare(0, gmsPrefix.size(), gmsPrefix)) {
            ALOGI("Blocking key attestation attempt from %s", cmdline.c_str());
            return android::INVALID_OPERATION;
        }
	}

    return android::NO_ERROR;
}

static void ensureSymbol(void** ptr, std::string realName) {
    if (!*ptr) {
        char* error;
        void* symPtr = dlsym(RTLD_NEXT, realName.c_str());
        if ((error = dlerror()) != NULL) {
            ALOGE("%s", error);
            exit(1);
        }

        *ptr = symPtr;
    }
}

namespace android {

namespace security {

namespace keystore {

// Android 10 - 11 (SDK 29-30), TX 28
// libkeystore_aidl.so (android::security::keystore::BnKeystoreService)
class BnKeystoreService {
public:
	android::status_t onTransact(uint32_t, const android::Parcel&, android::Parcel*, uint32_t);
};

typedef android::status_t (BnKeystoreService::*SecKsOnTransactFunc)(uint32_t, const android::Parcel&, android::Parcel*, uint32_t);
static SecKsOnTransactFunc real_onTransact = nullptr;

android::status_t BnKeystoreService::onTransact(uint32_t code, const android::Parcel& data, android::Parcel* reply, uint32_t flags) {
    android::status_t ret = interceptTransaction(code, 28);
    if (ret != android::NO_ERROR) {
        return ret;
    }

    ensureSymbol(reinterpret_cast<void**>(&real_onTransact), "_ZN7android8security8keystore17BnKeystoreService10onTransactEjRKNS_6ParcelEPS3_j");
    return (this->*real_onTransact)(code, data, reply, flags);
}

};

// Android 9 (SDK 28), TX 35
// libkeystore_aidl.so (android::security::BnKeystoreService)
class BnKeystoreService {
public:
	android::status_t onTransact(uint32_t, const android::Parcel&, android::Parcel*, uint32_t);
};

typedef android::status_t (BnKeystoreService::*SecOnTransactFunc)(uint32_t, const android::Parcel&, android::Parcel*, uint32_t);
static SecOnTransactFunc real_onTransact = nullptr;

android::status_t BnKeystoreService::onTransact(uint32_t code, const android::Parcel& data, android::Parcel* reply, uint32_t flags) {
    android::status_t ret = interceptTransaction(code, 35);
    if (ret != android::NO_ERROR) {
        return ret;
    }

    ensureSymbol(reinterpret_cast<void**>(&real_onTransact), "_ZN7android8security17BnKeystoreService10onTransactEjRKNS_6ParcelEPS2_j");
    return (this->*real_onTransact)(code, data, reply, flags);
}

};

// Android 7.0 - 8.1.0 (SDK 24-27), TX 36
// libkeystore_binder.so (android::BnKeystoreService)
class BnKeystoreService {
public:
	android::status_t onTransact(uint32_t, const android::Parcel&, android::Parcel*, uint32_t);
};

typedef android::status_t (BnKeystoreService::*AOnTransactFunc)(uint32_t, const android::Parcel&, android::Parcel*, uint32_t);
static AOnTransactFunc real_onTransact = nullptr;

android::status_t BnKeystoreService::onTransact(uint32_t code, const android::Parcel& data, android::Parcel* reply, uint32_t flags) {
    android::status_t ret = interceptTransaction(code, 36);
    if (ret != android::NO_ERROR) {
        return ret;
    }

    ensureSymbol(reinterpret_cast<void**>(&real_onTransact), "_ZN7android17BnKeystoreService10onTransactEjRKNS_6ParcelEPS1_j");
    return (this->*real_onTransact)(code, data, reply, flags);
}

};
