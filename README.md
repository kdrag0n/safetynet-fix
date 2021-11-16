# Universal SafetyNet Fix

This is a universal fix for SafetyNet on devices with hardware-backed attestation and unlocked bootloaders (or custom verified boot keys). It defeats both hardware attestation and the SafetyNet CTS profile updates released in 2021. The only requirement is that you can pass basic attestation, which requires a valid combination of device and model names, build fingerprints, and security patch levels. **MagiskHide is required as a result.**

Passing basic attestation is out-of-scope for this module; this module is meant to defy hardware attestation, as well as reported "basic" attestation that actually uses hardware under-the-hood. Use [MagiskHide Props Config](https://github.com/Magisk-Modules-Repo/MagiskHidePropsConf) to spoof your CTS profile if you have trouble passing basic attestation. This is a common issue on old devices and custom ROMs.

Android versions 7–12 are supported, including OEM skins such as Samsung One UI and MIUI. **This module requires Zygisk (for Magisk Canary) or Riru (for stable Magisk).**

## Installation

The module must be installed using Magisk Manager, not TWRP or any other custom recovery.

Always make sure you have the **latest version of the module** installed before reporting any issues.

## Zygisk

If you are using Magisk Canary, you need the Zygisk version of this module. Download and install the latest **Zygisk** version of this module from [GitHub Releases](https://github.com/kdrag0n/safetynet-fix/releases).

## Riru

If you are using stable Magisk, you need the Riru version of this module. Download and install the latest **Riru** version of this module from [GitHub Releases](https://github.com/kdrag0n/safetynet-fix/releases). You will also need to install "Riru" through Magisk Manager.

## How does it work?

Google Play Services opportunistically uses hardware-backed attestation to enforce SafetyNet security (since January 12, 2021), and enforces its usage based on the device model name (since September 2, 2021).

This module uses Riru to inject code into the Google Play Services process and then register a fake keystore provider that overrides the real one. When Play Services attempts to use key attestation, it throws an exception and pretends that the device lacks support for key attestation. This causes SafetyNet to fall back to basic attestation, which is much weaker and can be bypassed with existing methods.

However, blocking key attestation alone does not suffice because basic attestation fails on devices that are known by Google to support hardware-backed attestation. This module bypasses the check by appending a space character to the device model name. This has minimal impact on UX when only applied to Google Play Services, but it's sufficient for bypassing enforcement of hardware-backed attestation.

Unlike many other approaches, this doesn't break other features because key attestation is only blocked for Google Play Services, and even within Play Services, it is only blocked for SafetyNet code. As a result, other attestation-based features (such as using the device as a security key) will still work.

## ROM integration

Ideally, this workaround should be incorporated in custom ROMs instead of injecting code with a Magisk module. See the [ProtonAOSP website](https://protonaosp.kdrag0n.dev/developers/details/safetynet) for more information.

## Support

If you found this module helpful, please consider supporting development with a **[recurring donation](https://patreon.com/kdrag0n)** on Patreon for benefits such as exclusive behind-the-scenes development news, early access to updates, and priority support. Alternatively, you can also [buy me a coffee](https://paypal.me/kdrag0ndonate). All support is appreciated.
