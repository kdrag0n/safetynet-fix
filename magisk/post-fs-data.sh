#!/system/bin/sh

## This script is part of the Universal SafetyNet Fix module project.
## kdrag0n @ xda-developers

# Remove Play Services from the Magisk Denylist when set to enforcing.
if magisk --denylist status; then
	magisk --denylist rm com.google.android.gms
fi
