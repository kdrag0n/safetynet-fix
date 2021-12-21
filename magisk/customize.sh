#!/system/bin/sh

# Android 8.0 or newer
if [[ "$(getprop ro.build.version.sdk)" -lt 26 ]]; then
    ui_print "This module only supports Android 8.0 and newer."

    # Abort install and clean up
    rm -fr "$TMPDIR" "$MODPATH"
    exit 1
fi

chmod 755 "$MODPATH/service.sh" "$MODPATH/post-fs-data.sh"
