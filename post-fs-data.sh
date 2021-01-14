#!/system/bin/sh

set -eufo pipefail

system_patch="$(getprop ro.build.version.security_patch)"
vendor_patch="$(getprop ro.vendor.build.security_patch)"

# Fix mismatching security patch levels if vendor patch is not empty
if [[ ! -z "$vendor_patch" ]] && [[ "$system_patch" != "$vendor_patch" ]]; then
    resetprop ro.build.version.security_patch "$vendor_patch"
    resetprop ro.build.version.real_security_patch "$system_patch"
fi
