cd ..
cd Assets
python BuildAssetBundles.py
cd ..
cmake -S . -B ./PrjIOS -G Xcode -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=./ios.cmake -DIOS_PLATFORM=OS