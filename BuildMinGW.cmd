cmake -S . -B ./PrjMinGW -G "MinGW Makefiles"
mingw32-make -C PrjMinGW
cd PrjMinGW
ctest --verbose
cd ..