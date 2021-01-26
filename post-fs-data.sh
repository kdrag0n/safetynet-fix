#!/system/bin/sh

MODPATH="/data/adb/modules/safetynet-fix"

cp -af /system/bin/keystore $MODPATH/system/bin
$MODPATH/_bin/patchelf --add-needed libkeystore_shim.so $MODPATH/system/bin/keystore
