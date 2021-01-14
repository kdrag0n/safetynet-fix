#!/sbin/sh

sdk="$(getprop ro.build.version.sdk)"

if [ "$sdk" -eq 30 ]; then
    ui_print "Installing for Android 11"
    mv $MODPATH/system_sdk30 $MODPATH/system
elif [ "$sdk" -eq 29 ]; then
    ui_print "Installing for Android 10"
    mv $MODPATH/system_sdk29 $MODPATH/system
else
    ui_print "Android $(getprop ro.vendor.build.version.release) (SDK $sdk) is not supported!"
    exit 1
fi

# Set executable permissions
set_perm_recursive $MODPATH/system/bin 0 0 0755 0755
