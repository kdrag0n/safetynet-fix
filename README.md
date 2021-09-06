# Universal SafetyNet Fix

This is a universal fix for SafetyNet on devices with hardware-backed attestation and unlocked bootloaders (or custom verified boot keys). It defeats both hardware attestation and the SafetyNet CTS profile updates released on January 12, 2021. The only requirement is that you can pass basic attestation, which requires a valid combination of device and model names, build fingerprints, and security patch levels. **MagiskHide is required as a result.**

Passing basic attestation is out-of-scope for this module; this module is meant to defy hardware attestation, as well as reported "basic" attestation that actually uses hardware under-the-hood. Use [MagiskHide Props Config](https://github.com/Magisk-Modules-Repo/MagiskHidePropsConf) to spoof your CTS profile if you have trouble passing basic attestation. This is a common issue on old devices and custom ROMs.

Android versions 7–12 are supported, including OEM skins such as Samsung One UI and MIUI. This is a Riru module, so Riru must be installed in order for this to work.

## How does it work?

Google Play Services opportunistically uses hardware-backed attestation to enforce SafetyNet security (since January 12, 2021), and enforces its usage based on the device model name (since September 2, 2021).

This module uses Riru to inject code into the Google Play Services process and then register a fake keystore provider that overrides the real one. When Play Services attempts to use key attestation, it throws an exception and pretends that the device lacks support for key attestation. This causes SafetyNet to fall back to basic attestation, which is much weaker and can be bypassed with existing methods.

Normally, basic attestation fails on devices that are known by Google to support hardware-backed attestation, but this module bypasses the check by appending a space character to the device model name. This has minimal impact on UX when only applied to Google Play Services, but it's sufficient for bypassing enforcement of hardware-backed attestation.

Key attestation is only blocked specifically for SafetyNet in Google Play Services,
so no other features are broken.

## ROM integration

Ideally, this workaround should be incorporated in custom ROMs instead of injecting code with a Magisk module.

Commits for the system framework version of the workaround:

- [Android 11](https://github.com/ProtonAOSP/android_frameworks_base/commit/7f7a9b19c8293c09dfee12bec75ff17225c6710e)

## Support

If you found this module helpful, please consider supporting development with a **[recurring donation](https://patreon.com/kdrag0n)** on Patreon for benefits such as exclusive behind-the-scenes development news, early access to updates, and priority support. Alternatively, you can also [buy me a coffee](https://paypal.me/kdrag0ndonate). All support is appreciated.
