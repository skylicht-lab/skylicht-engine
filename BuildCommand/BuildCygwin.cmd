cd..
cmake -S . -B ./PrjCygwin -G "Unix Makefiles"
cd Assets
python BuildAssetBundles.py
cd ..
make -C PrjCygwin