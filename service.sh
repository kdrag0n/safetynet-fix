#!/usr/bin/env bash

# Keystore v2 starts before Magisk on Android 12, so it needs to be restarted.
# Do this in service.sh to make sure that files have been mounted already.
if getprop ro.build.id | grep -q SPB3; then
    stop keystore2
    start keystore2
fi
