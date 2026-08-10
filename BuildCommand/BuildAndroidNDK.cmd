set MINGW=C:\mingw64\bin
set NDK=C:\Users\Pham_\AppData\Local\Android\Sdk\ndk\30.0.15729638
set SDK_VERSION=26
cd..
set PATH=%PATH%;%MINGW%

set ABI="armeabi-v7a"
cmake -S . -B ./PrjAndroid-%ABI% -G "MinGW Makefiles" -DCMAKE_TOOLCHAIN_FILE=%NDK%/build/cmake/android.toolchain.cmake -DANDROID_ABI=%ABI% -DANDROID_ARM_NEON=ON -DANDROID_PLATFORM=%SDK_VERSION%
mingw32-make -C PrjAndroid-%ABI%

set ABI="arm64-v8a"
cmake -S . -B ./PrjAndroid-%ABI% -G "MinGW Makefiles" -DCMAKE_TOOLCHAIN_FILE=%NDK%/build/cmake/android.toolchain.cmake -DANDROID_ABI=%ABI% -DANDROID_ARM_NEON=ON -DANDROID_PLATFORM=%SDK_VERSION%
mingw32-make -C PrjAndroid-%ABI%

set ABI="x86_64"
cmake -S . -B ./PrjAndroid-%ABI% -G "MinGW Makefiles" -DCMAKE_TOOLCHAIN_FILE=%NDK%/build/cmake/android.toolchain.cmake -DANDROID_ABI=%ABI% -DANDROID_ARM_NEON=ON -DANDROID_PLATFORM=%SDK_VERSION%
mingw32-make -C PrjAndroid-%ABI%