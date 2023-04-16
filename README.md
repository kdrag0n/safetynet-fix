# Universal SafetyNet Fix [MOD]

Magisk module to work around Google's SafetyNet and Play Integrity attestation.

This module works around hardware attestation and updates to SafetyNet and Play Integrity CTS profile checks. You must already be able to pass basic CTS profile attestation, which requires a valid combination of device and model names, build fingerprints, and security patch levels.

If you still have trouble passing SafetyNet or Play Integrity with this module, spoof the profile of a certified device by copying `ro.product` properties. [MagiskHide Props Config](https://github.com/Magisk-Modules-Repo/MagiskHidePropsConf) is an easy way to do so on Magisk v23 and older. This is a common issue on old devices, custom ROMs, and stock ROMs without GMS certification (e.g. Chinese ROMs).

Android versions up to 13 are supported, including OEM skins such as Samsung One UI and MIUI.

## Download

**[Download latest [MOD] version](https://github.com/Displax/safetynet-fix/releases)**

Install the downloaded module in Magisk Manager, then **enable Zygisk in Magisk settings.**

## How does it work?

See [Details](docs/details.md) for details about how this module works.

## ROM integration

Ideally, this workaround should be incorporated in custom ROMs instead of injecting code with a Magisk module. See the [ProtonAOSP website](https://protonaosp.org/developers/details/safetynet) for more information.
