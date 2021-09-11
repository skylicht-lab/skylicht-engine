cd ..
cd Assets
python BuildAssetBundles.py
cd ..
cmake -S . -B ./PrjLinux -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug