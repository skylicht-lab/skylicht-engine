# How to build BoringSSL

- Please read [BUILDING.md](https://boringssl.googlesource.com/boringssl/+/HEAD/BUILDING.md)

```console
C:\sdk>git clone "https://boringssl.googlesource.com/boringssl"
```

## Build tool

NASM: https://www.nasm.us

Ninja: https://ninja-build.org

## Windows

### Step 2: Build

- x64 version:
```console
C:\sdk\boringssl>cmake -S . -B ./PrjVisualStudio -G "Visual Studio 17 2022" -A x64
```

- x86 version:
```console
cd C:\sdk\boringssl
C:\sdk\boringssl>cmake -S . -B ./PrjVisualStudio -G "Visual Studio 17 2022" -A Win32
```

## Android

From Windows + R: "cmd"

`build_ssl_android_platform.cmd`

```console
cd boringssl
cmake -DCMAKE_BUILD_TYPE=Release -DANDROID_ABI=%ABI% -DANDROID_PLATFORM=android-21 -DCMAKE_TOOLCHAIN_FILE=%ANDROID_NDK%/build/cmake/android.toolchain.cmake -GNinja -B build_%ABI%
cmake --build ./build_%ABI%
cd ..
```

`build_ssl_android.cmd`

```console
set ABI=armeabi-v7a
call build_ssl_android_platform.cmd
set ABI=arm64-v8a
call build_ssl_android_platform.cmd
set ABI=x86
call build_ssl_android_platform.cmd
set ABI=x86_64
call build_ssl_android_platform.cmd
```