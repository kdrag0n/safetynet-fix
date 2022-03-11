#!/system/bin/sh

# Check if is Android 8.0 or newer
if [[ "$(getprop ro.build.version.sdk)" -lt 26 ]]; then
    ui_print "Legacy Android version (<= 7.1)! Keep properties only."

    # Clean up
    rm -fr "$MODPATH/zygisk"
    rm -f "$MODPATH/classes.dex"
    rm -f "$MODPATH/post-fs-data.sh"
else
	chmod 755 "$MODPATH/post-fs-data.sh"
fi

chmod 755 "$MODPATH/service.sh"
