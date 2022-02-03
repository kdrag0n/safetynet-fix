#!/system/bin/sh

# Remove Play Services from the Magisk Denylist when set to enforcing.
if magisk --denylist status; then
	magisk --denylist rm com.google.android.gms
fi
