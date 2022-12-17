# Universal SafetyNet Fix

Magisk module to work around Google's SafetyNet attestation.

This module works around hardware attestation and recent updates to SafetyNet CTS profile checks. You must already be able to pass basic CTS profile attestation, which requires a valid combination of device and model names, build fingerprints, and security patch levels.

If you still have trouble passing SafetyNet with this module and have Magisk v23 or older, use [MagiskHide Props Config](https://github.com/Magisk-Modules-Repo/MagiskHidePropsConf) to spoof a certified device profile. This is a common issue on old devices, custom ROMs, and stock ROMs without GMS certification (e.g. Chinese ROMs).

Android versions up to 13 Beta 3 are supported, including OEM skins such as Samsung One UI and MIUI.

## Download

**[Download latest version](https://github.com/kdrag0n/safetynet-fix/releases)**

Install the downloaded module in Magisk Manager, then **enable Zygisk in Magisk settings.**

There is also a [Riru version](https://github.com/kdrag0n/safetynet-fix/releases/tag/v2.1.3) for Magisk v23 and older.

## How does it work?

See [Details](docs/details.md) for details about how this module works.

## ROM integration

Ideally, this workaround should be incorporated in custom ROMs instead of injecting code with a Magisk module. See the [ProtonAOSP website](https://protonaosp.org/developers/details/safetynet) for more information.

## Support

If you found this module helpful, please consider supporting development with a **[recurring donation](https://patreon.com/kdrag0n)** on Patreon for benefits such as exclusive behind-the-scenes development news, early access to updates, and priority support. Alternatively, you can also [buy me a coffee](https://paypal.me/kdrag0ndonate). All support is appreciated.
