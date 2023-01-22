#!/system/bin/sh

## This script is part of the Universal SafetyNet Fix module project.
## kdrag0n @ xda-developers

# Some sensitive [secure] properties are dynamic and need to be adjusted (set) during boot and/or after boot compleate.


# # Module directory set by Magisk.
# MODDIR=${0%/*}

# __ Define functions. __

remove_prop() {
    if [ "$(getprop $1)" ]; then
        resetprop -n --delete $1
    fi
}

reset_prop() {
    if [ "$(getprop $1)" ]; then
        if [ "$(getprop $1)" = "$2" ]; then
            resetprop -n $1 $3
        fi
    fi
}

set_max() {
    if [ "$(getprop $1)" ]; then
        if [ "$(getprop $1)" -gt "$2" ]; then
            resetprop -n $1 $2
        fi
    fi
}

set_min() {
    if [ "$(getprop $1)" ]; then
        if [ "$(getprop $1)" -lt "$2" ]; then
            resetprop -n $1 $2
        fi
    fi
}

set_prop() {
    if [ "$(getprop $1)" ]; then
        if [ "$(getprop $1)" != "$2" ]; then
            resetprop -n $1 $2
        fi
    fi
}

# __ Check and adjust sensitive [secure] properties as needed. __

# Hide that we booted from recovery when magisk is in recovery mode.
reset_prop ro.boot.mode recovery unknown
reset_prop ro.bootmode recovery unknown
reset_prop vendor.boot.mode recovery unknown

# Xiaomi cross region flash
reset_prop ro.boot.hwc CN GLOBAL
reset_prop ro.boot.hwcountry China GLOBAL

# Remove properties.
remove_prop ro.build.selinux

# __ Wait for boot compleate to set some properties. __

{
    until [ "$(getprop sys.boot_completed)" -eq 1 ]; do
        date +%N > /dev/null
    done

    # Properties that need to be set after boot compleate.
    set_prop ro.boot.flash.locked 1
    set_prop ro.boot.vbmeta.device_state locked
    set_prop ro.boot.verifiedbootstate green
    set_prop ro.boot.veritymode enforcing
    set_max ro.product.first_api_level 32
    set_prop ro.secure 1
    set_prop sys.oem_unlock_allowed 0
    set_prop vendor.boot.vbmeta.device_state locked
    set_prop vendor.boot.verifiedbootstate green
}&
