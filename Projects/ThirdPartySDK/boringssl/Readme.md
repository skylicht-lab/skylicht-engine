# How to build BoringSSL

- Please read [BUILDING.md](https://boringssl.googlesource.com/boringssl/+/HEAD/BUILDING.md)

## Windows

### Step 1: Install tool
Ninja: https://ninja-build.org

NASM: https://www.nasm.us

And set ENV PATH for ninja and nasm

### Step 2: Build

From Window/Search: "x64 Native Tools Command Prompt for VS 2022" or "x86 Native Tools Command Prompt for VS 2022"

```console
C:\sdk\boringssl>cmake -GNinja -B build_x64
C:\sdk\boringssl>ninja -C build_x64
```
On visual studio you should remove the -WX flag in CMakelist.txt if the compile fails.

## Android

From Run: "cmd"

```console
C:\sdk\boringssl>cmake -DANDROID_ABI=armeabi-v7a -DANDROID_PLATFORM=android-21 -DCMAKE_TOOLCHAIN_FILE=%ANDROID_NDK%/build/cmake/android.toolchain.cmake -GNinja -B build_android
C:\sdk\boringssl>cmake --build ./build_android
```