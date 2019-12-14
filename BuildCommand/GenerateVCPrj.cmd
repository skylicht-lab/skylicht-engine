cd..
cd Assets
python BuildAssetBundles.py
cd ..
cmake -S . -B ./PrjVisualStudio -G "Visual Studio 15 2017" -A WIN32