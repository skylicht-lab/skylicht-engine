cmake -S . -B ./PrjCygwin
cd Assets
python BuildAssetBundles.py
cd ..
make -C PrjCygwin