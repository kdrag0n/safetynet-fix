#!/system/bin/sh
# Conditional MagiskHide properties

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

resetprop --delete ro.build.selinux

# SELinux permissive
if [[ "$(cat /sys/fs/selinux/enforce)" == "0" ]]; then
    chmod 640 /sys/fs/selinux/enforce
    chmod 440 /sys/fs/selinux/policy
fi

# Late props which must be set after boot_completed
{
    until [[ "$(getprop sys.boot_completed)" == "1" ]]; do
        sleep 1
    done

    # Avoid breaking Realme fingerprint scanners
    resetprop ro.boot.flash.locked 1

    # Avoid breaking Oppo fingerprint scanners
    resetprop ro.boot.vbmeta.device_state locked

    # Avoid breaking OnePlus display modes/fingerprint scanners
    resetprop vendor.boot.verifiedbootstate green

    # Safetynet (avoid breaking OnePlus display modes/fingerprint scanners on OOS 12)
    resetprop ro.boot.verifiedbootstate green
    resetprop ro.boot.veritymode enforcing
    resetprop vendor.boot.vbmeta.device_state locked

    # Avoid breaking encryption, set shipping level to 32 for devices >=33 to allow for software attestation
    if [[ "$(getprop ro.product.first_api_level)" -ge 33 ]]; then
        resetprop ro.product.first_api_level 32
    fi
}&
