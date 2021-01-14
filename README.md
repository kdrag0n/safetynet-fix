# Universal SafetyNet Fix

This is a universal fix for SafetyNet on devices with hardware attestation and unlocked bootloaders. It defeats both hardware attestation and the new SafetyNet ctsProfile updates released on January 12, 2021. The only requirement is that you can pass basic attestation, which requires a valid combination of device and model names, build fingerprints, and security patch levels. Passing basic attestation is out-of-scope for this module.

No device-specific features (such as the new Pixel-exclusive Google Assistant design or screen-off voice match) will be lost with this fix.

MagiskHide is required if the device is rooted.

Only Android 10 and 11 are supported. Heavy OEM skins are not officially supported, but it may work on them depending on your luck and the particular ROM in question.

## How does it work?

An in-depth explanation, as well as source code, can be found in the [ROM commit](https://github.com/ProtonAOSP/android_system_security/commit/15633a3d29bf727b83083f2c49d906c16527d389).
