#!/system/bin/sh

# Check compatibility and set static sensitive properties.

add_prop() {
    local prop="$1"
    local value="$2"

    if [ "$(getprop "$prop")" ]; then
        echo $prop"="$value >>"$MODPATH/system.prop"
    fi
}

# Check Android version.
if [ "$(getprop ro.build.version.sdk)" -lt "26" ]; then
    ui_print " "
    ui_print "This module only supports Android 8 and newer."
    ui_print " This device is Android "$(getprop ro.build.version.release)
    rm -fr "$TMPDIR" "$MODPATH"
    ui_print " "
    exit 1
fi

# Check Magisk version.
if [ "$MAGISK_VER_CODE" -lt "24000" ]; then
    ui_print " "
    ui_print " Requires Magisk v24+ to be installed and active."
    ui_print "  Installed version is "$MAGISK_VER_CODE
    rm -fr "$TMPDIR" "$MODPATH"
    ui_print " "
    exit 1
fi

# Make sure the scripts are executable.
chmod 755 "$MODPATH/service.sh" "$MODPATH/post-fs-data.sh"

# Create a new 'system.prop' file.
echo "# Device sensitive properties" >"$MODPATH/system.prop"
echo "" >>"$MODPATH/system.prop"

# __ Add sensitive properties. __

# Common
add_prop ro.adb.secure 1
add_prop ro.build.tags release-keys
add_prop ro.build.type user
add_prop ro.debuggable 0
add_prop ro.secure 1

# SafetyNet
add_prop ro.boot.verifiedbootstate green
add_prop ro.boot.veritymode enforcing
add_prop ro.boot.vbmeta.device_state locked
add_prop vendor.boot.vbmeta.device_state locked

# __ Manufacturer specific sensitive properties. __

# OnePlus
add_prop ro.is_ever_orange 0

# Samsung
add_prop ro.boot.warranty_bit 0
add_prop ro.vendor.boot.warranty_bit 0
add_prop ro.vendor.warranty_bit 0
add_prop ro.warranty_bit 0
