#!/system/bin/sh
# Conditional MagiskHide properties

MODDIR=${0%/*}

maybe_set_prop() {
    local prop="$1"
    local contains="$2"
    local value="$3"

    if [[ "$(getprop "$prop")" == *"$contains"* ]]; then
        resetprop "$prop" "$value"
    fi
}

# Magisk recovery mode
maybe_set_prop ro.bootmode recovery unknown
maybe_set_prop ro.boot.mode recovery unknown
maybe_set_prop vendor.boot.mode recovery unknown

# MIUI cross-region flash
maybe_set_prop ro.boot.hwc CN GLOBAL
maybe_set_prop ro.boot.hwcountry China GLOBAL

# Kernel cmdline
cat "/proc/cmdline" | sed 's/orange/green/i' | sed 's/yellow/green/i' | sed 's/unlocked/locked/i' | sed 's/permissive/enforcing/i' > "${MODDIR}/cmdline"

mount -o bind "${MODDIR}/cmdline /proc/cmdline"

# SELinux permissive
resetprop --delete ro.build.selinux

if [[ "$(cat /sys/fs/selinux/enforce)" == "0" ]]; then
    echo "1" > "${MODDIR}/enforce"

    chmod 0640 "${MODDIR}/enforce"
    chmod 0640 /sys/fs/selinux/enforce
    chmod 0440 /sys/fs/selinux/policy

    mount -o bind "${MODDIR}/enforce /sys/fs/selinux/enforce"
fi

# Late props which must be set after boot_completed
{
    until [[ "$(getprop sys.boot_completed)" == "1" ]]; do
        sleep 1
    done

    # avoid breaking Realme fingerprint scanners
    resetprop ro.boot.flash.locked 1

    # avoid breaking Oppo fingerprint scanners
    resetprop ro.boot.vbmeta.device_state locked

    # avoid breaking OnePlus display modes/fingerprint scanners
    resetprop vendor.boot.verifiedbootstate green
}&
