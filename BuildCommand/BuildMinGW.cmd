set PATH=%PATH%;C:\MinGW\bin
cd..
cmake -S . -B ./PrjMinGW -G "MinGW Makefiles" -DCMAKE_SH="CMAKE_SH-NOTFOUND" -DBUILD_SHARED_LIBS=ON
cd Assets
python BuildAssetBundles.py
cd ..
mingw32-make -C PrjMinGW
cd PrjMinGW
ctest --verbose
cd ..