# Developing Zygisk Modules

This repository hosts a template zygisk module for developers to start developing Zygisk modules. Before developing Zygisk modules, you should first check out the official documentation for [Magisk Modules](https://topjohnwu.github.io/Magisk/guides.html). Do not fork this repository for your new module; either manually clone this repository, or press the "Use this template" button in the GitHub UI.

This repository is archived because it is meant to be read-only; the project is not abandoned. For any issues, please report them to the main Magisk repository.

## API

- The canonical URL of the public Zygisk API is [module/jni/zygisk.hpp](https://github.com/topjohnwu/zygisk-module-sample/blob/master/module/jni/zygisk.hpp).
- The header file is self documented; directly refer to the header source code for all Zygisk API details.
- Magisk is committed to maintain backwards compatibility forever. That is, whenever there is an API update for Zygisk in a newer Magisk version, Magisk can always load Zygisk modules built for an older Zygisk API.

## Notes

- This repository can be opened with Android Studio.
- Developing Zygisk modules requires a modern C++ compiler. Please use NDK r21 or higher.
- All the C++ code is in the [module/jni](https://github.com/topjohnwu/zygisk-module-sample/tree/master/module/jni) folder.
- DO NOT modify the default configurations in `Application.mk` unless you know what you are doing.

## C++ STL

- The `APP_STL` variable in `Application.mk` is set to `none`. **DO NOT** use any C++ STL included in NDK.
- If you'd like to use C++ STL, you **have to** use the `libcxx` included as a git submodule in this repository. Zygisk modules' code are injected into Zygote, and the included `libc++` is setup to be lightweight and fully self contained that prevents conflicts with the hosting program.
- If you do not need STL, link to the system `libstdc++` so that you can at least call the `new` operator.
- Both configurations are demonstrated in the example `Android.mk`.

## Building

- In the `module` folder, call [`ndk-build`](https://developer.android.com/ndk/guides/ndk-build) to compile your modules.
- Your module libraries will be in `libs/<abi>/lib<module_name>.so`.
- Copy the libraries into your module's `zygisk` folder, with the ABI as it's file name:

```
module_id
├── module.prop
└── zygisk
    ├── arm64-v8a.so
    ├── armeabi-v7a.so
    ├── x86.so
    └── x86_64.so
```

## License

Although the main Magisk project is licensed under GPLv3, the Zygisk API and its headers are not. Every file in this repository is released to the public domain, so you don't have to worry about any licensing issues while developing Zygisk modules. You can create a closed source module, or publish your source code under any open source license you prefer; there is no restrictions at all.
