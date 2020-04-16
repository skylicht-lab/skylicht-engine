cd ..
cd Assets
python BuildAssetBundles.py
cd ..
cmake -S . -B ./PrjMac -G Xcode