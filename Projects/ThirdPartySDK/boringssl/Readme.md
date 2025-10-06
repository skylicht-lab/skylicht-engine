# How to build BoringSSL

- Please read [BUILDING.md](https://boringssl.googlesource.com/boringssl/+/HEAD/BUILDING.md)

```console
C:\sdk>git clone "https://boringssl.googlesource.com/boringssl"
```

## Windows

### Step 1: Install tool
NASM: https://www.nasm.us
Ninja: https://ninja-build.org

And set ENV PATH for ninja and nasm

### Step 2: Build

- x86 version:
```console
C:\sdk\boringssl>cmake -S . -B ./PrjVisualStudio -G "Visual Studio 17 2022" -A x64
```

- x86 version:
```console
cd C:\sdk\boringssl
C:\sdk\boringssl>cmake -S . -B ./PrjVisualStudio -G "Visual Studio 17 2022" -A Win32
```

## Android

From Run: "cmd"

```console
C:\sdk\boringssl>cmake -DCMAKE_BUILD_TYPE=Release -DANDROID_ABI=armeabi-v7a -DANDROID_PLATFORM=android-21 -DCMAKE_TOOLCHAIN_FILE=%ANDROID_NDK%/build/cmake/android.toolchain.cmake -GNinja -B build_android
C:\sdk\boringssl>cmake --build ./build_android
```