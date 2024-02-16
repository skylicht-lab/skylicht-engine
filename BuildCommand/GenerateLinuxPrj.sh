cd ..
cd Assets
python3 BuildAssetBundles.py
cd ..
cmake -S . -B ./PrjLinux -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug