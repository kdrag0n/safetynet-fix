#!/system/bin/sh

MODPATH="/data/adb/modules/safetynet-fix"

# Get runtime version
sdk="$(getprop ro.build.version.sdk)"
version="$(getprop ro.vendor.build.version.release)"
if getprop ro.build.id | grep -q SPB3; then
    sdk=31
    version="12 Beta 3"
fi

# Prepare to update version
rm -fr "$MODPATH/system"

# Make sure version is supported
if [[ ! -d "$MODPATH/system_sdk$sdk" ]]; then
    exit
fi

# Symlink results in the wrong SELinux context
cp -r "$MODPATH/system_sdk$sdk" "$MODPATH/system"
