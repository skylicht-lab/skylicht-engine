set MINGW=C:\MinGW\bin
set NDK=C:\Android\android-ndk-r26d
set SDK_VERSION=26
cd..
set PATH=%PATH%;%MINGW%

set ABI="armeabi-v7a"
cmake -S . -B ./PrjAndroid-%ABI% -G "MinGW Makefiles" -DCMAKE_TOOLCHAIN_FILE=%NDK%/build/cmake/android.toolchain.cmake -DANDROID_ABI=%ABI% -DANDROID_ARM_NEON=ON -DANDROID_NATIVE_API_LEVEL=%SDK_VERSION%
mingw32-make -C PrjAndroid-%ABI%

set ABI="arm64-v8a"
cmake -S . -B ./PrjAndroid-%ABI% -G "MinGW Makefiles" -DCMAKE_TOOLCHAIN_FILE=%NDK%/build/cmake/android.toolchain.cmake -DANDROID_ABI=%ABI% -DANDROID_ARM_NEON=ON -DANDROID_NATIVE_API_LEVEL=%SDK_VERSION%
mingw32-make -C PrjAndroid-%ABI%