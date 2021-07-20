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
if getprop ro.build.id | grep -q SPB3; then
    sdk=31
    version="12 Beta 3"
fi

# Initial version check; version can be changed later.
if [[ ! -d "$MODPATH/system_sdk$sdk" ]]; then
    ui_print "Android $version (SDK $sdk) is not supported!"
    rm -fr "$MODPATH"
    exit 1
fi

# Set executable permissions
for sdk in $MODPATH/system_sdk*
do
    set_perm_recursive $sdk/bin 0 0 0755 0755
done
chmod 755 $MODPATH/*.sh
