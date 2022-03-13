#!/system/bin/sh

# Android 8.0 or newer
if [[ "$(getprop ro.build.version.sdk)" -lt 26 ]]; then
    ui_print ""
    ui_print "Functionality is limited on Android 7 and older."
    ui_print "Hardware-backed attestation will not be disabled."
    ui_print ""

    # Remove Zygisk module, but keep props and scripts
    rm -fr "$MODPATH/zygisk"
fi

chmod 755 "$MODPATH/service.sh" "$MODPATH/post-fs-data.sh"
