#!/sbin/sh

# We check the native ABI instead of all supported ABIs because this is a system
# service, and underlying AIDL/HIDL ABIs may not match. We also link against other
# system libraries.
arch="$(getprop ro.product.cpu.abi)"
if [[ "$arch" != "arm64-v8a" ]]; then
    ui_print "Unsupported CPU architecture: $arch"
    exit 1
fi

sdk="$(getprop ro.build.version.sdk)"
version="$(getprop ro.vendor.build.version.release)"
if getprop ro.build.id | grep -q SPB2; then
    sdk=31-b2
    version="12 Beta 2"
fi

if mv "$MODPATH/system_sdk$sdk" $MODPATH/system; then
    ui_print "Installing for Android $version"
else
    ui_print "Android $version (SDK $sdk) is not supported!"
    exit 1
fi

# Remove unused SDKs
rm -fr $MODPATH/system_sdk*

# Set executable permissions
set_perm_recursive $MODPATH/system/bin 0 0 0755 0755
