#!/system/bin/sh

# Set conditional sensitive properties.

maybe_set_prop() {
    local prop="$1"
    local contains="$2"
    local value="$3"

    if [[ "$(getprop "$prop")" == *"$contains"* ]]; then
        resetprop "$prop" "$value"
    fi
}

reset_prop() {
    local prop="$1"
    local value="$2"

    if [ "$(getprop "$prop")" ]; then
        if [ "$(getprop "$prop")" != "$value" ]; then
            resetprop "$prop" "$value"
        fi
    fi
}

# __ Check and correct sensitive properties as needed. __

# Magisk recovery mode
maybe_set_prop ro.bootmode recovery unknown
maybe_set_prop ro.boot.mode recovery unknown
maybe_set_prop vendor.boot.mode recovery unknown

# MIUI cross-region flash
maybe_set_prop ro.boot.hwc CN GLOBAL
maybe_set_prop ro.boot.hwcountry China GLOBAL

# Remove 'ro.build.selinux'
resetprop --delete ro.build.selinux

# SELinux permissive
if [[ "$(cat /sys/fs/selinux/enforce)" == "0" ]]; then
    chmod 640 /sys/fs/selinux/enforce
    chmod 440 /sys/fs/selinux/policy
fi

# Properties that need to be reset after boot_completed
{
    until [[ "$(getprop sys.boot_completed)" == "1" ]]; do
        sleep 1
    done

    # avoid breaking Realme fingerprint scanners
    reset_prop ro.boot.flash.locked 1

    # avoid breaking OnePlus display modes/fingerprint scanners
    reset_prop vendor.boot.verifiedbootstate green
}&
