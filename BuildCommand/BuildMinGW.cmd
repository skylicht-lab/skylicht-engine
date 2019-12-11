cd..
cmake -S . -B ./PrjMinGW -G "MinGW Makefiles" -DCMAKE_SH="CMAKE_SH-NOTFOUND"
cd Assets
python BuildAssetBundles.py
cd ..
mingw32-make -C PrjMinGW
cd PrjMinGW
ctest --verbose
cd ..