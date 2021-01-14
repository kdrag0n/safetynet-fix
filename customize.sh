#!/sbin/sh

sdk="$(getprop ro.build.version.sdk)"
version="$(getprop ro.vendor.build.version.release)"

if mv "$MODPATH/system_sdk$sdk" $MODPATH/system; then
    ui_print "Installing for Android $version"
else
    ui_print "Android $version (SDK $sdk) is not supported!"
    exit 1
fi

# Set executable permissions
set_perm_recursive $MODPATH/system/bin 0 0 0755 0755
