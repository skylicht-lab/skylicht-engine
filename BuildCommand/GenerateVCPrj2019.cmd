cd ..
cd Assets
python BuildAssetBundles.py
cd ..
cmake -S . -B ./PrjVisualStudio -G "Visual Studio 16 2019" -A x64