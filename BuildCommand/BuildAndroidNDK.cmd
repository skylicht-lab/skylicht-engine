set MINGW=C:\mingw-w64\x86_64-8.1.0-posix-seh-rt_v6-rev0\mingw64\bin
set NDK=C:\SDK\android-ndk-r21e
set MINSDKVERSION=24
cd..
set PATH=%PATH%;%MINGW%

set ABI="armeabi-v7a"
cmake -S . -B ./PrjAndroid-%ABI% -G "MinGW Makefiles" -DCMAKE_TOOLCHAIN_FILE=%NDK%/build/cmake/android.toolchain.cmake -DANDROID_ABI=%ABI% -DANDROID_ARM_NEON=ON -DANDROID_NATIVE_API_LEVEL=%MINSDKVERSION%
mingw32-make -C PrjAndroid-%ABI%

set ABI="arm64-v8a"
cmake -S . -B ./PrjAndroid-%ABI% -G "MinGW Makefiles" -DCMAKE_TOOLCHAIN_FILE=%NDK%/build/cmake/android.toolchain.cmake -DANDROID_ABI=%ABI% -DANDROID_ARM_NEON=ON -DANDROID_NATIVE_API_LEVEL=%MINSDKVERSION%
mingw32-make -C PrjAndroid-%ABI%