#!/system/bin/sh

# DenyList must be enabled to pass basicIntegrity
magisk --denylist enable

# Remove Play Services from DenyList, otherwise the Zygisk module won't load
magisk --denylist rm com.google.android.gms
