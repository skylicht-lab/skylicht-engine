rem set cmake_args=-DCMAKE_SYSTEM_NAME=WindowsStore -DCMAKE_SYSTEM_VERSION=10.0
cmake %cmake_args% -S . -B ./PrjVisualStudio -G "Visual Studio 15 2017" -A WIN32