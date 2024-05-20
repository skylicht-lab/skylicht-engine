cd ..
set ANDROID_MAIN=Projects\Android\app\src\main
set LIBRARY=libSampleBoidSystemVAT.so

if not exist "%ANDROID_MAIN%/jniLibs" (
	mkdir "%ANDROID_MAIN%/jniLibs"
)

set ABI="armeabi-v7a"
if not exist "%ANDROID_MAIN%/jniLibs/%ABI%" (
	mkdir "%ANDROID_MAIN%/jniLibs/%ABI%"
)
copy Bin\Android\Libs\%ABI%\%LIBRARY% "%ANDROID_MAIN%/jniLibs/%ABI%"

set ABI="arm64-v8a"
if not exist "%ANDROID_MAIN%/jniLibs/%ABI%" (
	mkdir "%ANDROID_MAIN%/jniLibs/%ABI%"
)
copy Bin\Android\Libs\%ABI%\%LIBRARY% "%ANDROID_MAIN%/jniLibs/%ABI%"

set ABI="x86"
if not exist "%ANDROID_MAIN%/jniLibs/%ABI%" (
	mkdir "%ANDROID_MAIN%/jniLibs/%ABI%"
)
copy Bin\Android\Libs\%ABI%\%LIBRARY% "%ANDROID_MAIN%/jniLibs/%ABI%"