cmake -S . -B ./PrjMinGW -G "MinGW Makefiles"
cd Assets
python BuildAssetBundles.py
cd ..
mingw32-make -C PrjMinGW
cd PrjMinGW
ctest --verbose
cd ..