#!/usr/bin/env bash

set -euo pipefail

build_mode="${1:-release}"

cd "$(dirname "$0")"

pushd zygisk/module
rm -fr libs
debug_mode=1
if [[ "$build_mode" == "release" ]]; then
    debug_mode=0
fi
$ANDROID_HOME/ndk/21.4.7075529/ndk-build -j48 NDK_DEBUG=$debug_mode
popd

pushd java
# Must always be release due to R8 requirement
./gradlew assembleRelease
popd

mkdir -p magisk/zygisk
for arch in arm64-v8a armeabi-v7a x86 x86_64
do
    cp "zygisk/module/libs/$arch/libsafetynetfix.so" "magisk/zygisk/$arch.so"
done

pushd magisk
version="$(grep '^version=' module.prop  | cut -d= -f2)"
rm -f "../safetynet-fix-$version.zip" classes.dex
unzip "../java/app/build/outputs/apk/release/app-release.apk" "classes.dex"
zip -r9 "../safetynet-fix-$version.zip" .
popd
