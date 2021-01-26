#!/sbin/sh

# We check the native ABI instead of all supported ABIs because this is a system
# service, and underlying AIDL/HIDL ABIs may not match. We also link against other
# system libraries.
arch="$(getprop ro.product.cpu.abi)"
if [[ "$arch" != "arm64-v8a" ]] && [[ "$arch" != "armeabi-v7a" ]]; then
    ui_print "Unsupported CPU architecture: $arch"
    exit 1
fi

# Key attestation was introduced in Android 7.0
sdk="$(getprop ro.build.version.sdk)"
if [[ "$sdk" -lt 24 ]]; then
    version="$(getprop ro.vendor.build.version.release)"
    ui_print "Android $version (SDK $sdk) does not support key attestation, not installing."
    exit 1
fi

mkdir $MODPATH/system/bin
chmod 755 $MODPATH/post-fs-data.sh
set_perm_recursive $MODPATH/_bin 0 0 0755 0755
