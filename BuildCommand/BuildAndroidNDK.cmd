set NDK=C:\android-ndk-r20b
set ABI="armeabi-v7a with NEON"
set MINSDKVERSION=24
cd..
cmake -S . -B ./PrjAndroidNDK -G "MinGW Makefiles" -DTARGET_ARCH=ANDROID -DCMAKE_TOOLCHAIN_FILE=%NDK%/build/cmake/android.toolchain.cmake -DANDROID_ABI=%ABI% -DANDROID_ARM_NEON=ON -DANDROID_NATIVE_API_LEVEL=%MINSDKVERSION%
mingw32-make -C PrjAndroidNDK