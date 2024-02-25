cd ..
cd Assets
python3 BuildAssetBundles.py
cd ..
cmake -S . -B ./PrjIOS -G Xcode -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=./ios.cmake -DPLATFORM=OS64