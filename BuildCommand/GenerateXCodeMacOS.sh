cd ..
cd Assets
python3 BuildAssetBundles.py
cd ..
cmake -S . -B ./PrjMac -G Xcode -DCMAKE_BUILD_TYPE=Debug