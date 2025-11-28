set MINGW=C:\mingw64\bin
set NDK=C:\Users\Pham_\AppData\Local\Android\Sdk\ndk\29.0.14033849
set SDK_VERSION=29
cd..
set PATH=%PATH%;%MINGW%

set ABI="armeabi-v7a"
cmake -S . -B ./PrjAndroid-%ABI% -G "MinGW Makefiles" -DCMAKE_TOOLCHAIN_FILE=%NDK%/build/cmake/android.toolchain.cmake -DANDROID_ABI=%ABI% -DANDROID_ARM_NEON=ON -DANDROID_NATIVE_API_LEVEL=%SDK_VERSION%
mingw32-make -C PrjAndroid-%ABI%

set ABI="arm64-v8a"
cmake -S . -B ./PrjAndroid-%ABI% -G "MinGW Makefiles" -DCMAKE_TOOLCHAIN_FILE=%NDK%/build/cmake/android.toolchain.cmake -DANDROID_ABI=%ABI% -DANDROID_ARM_NEON=ON -DANDROID_NATIVE_API_LEVEL=%SDK_VERSION%
mingw32-make -C PrjAndroid-%ABI%

set ABI="x86_64"
cmake -S . -B ./PrjAndroid-%ABI% -G "MinGW Makefiles" -DCMAKE_TOOLCHAIN_FILE=%NDK%/build/cmake/android.toolchain.cmake -DANDROID_ABI=%ABI% -DANDROID_ARM_NEON=ON -DANDROID_NATIVE_API_LEVEL=%SDK_VERSION%
mingw32-make -C PrjAndroid-%ABI%