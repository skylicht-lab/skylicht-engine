cd ..
cd Assets
python BuildAssetBundles.py
cd ..
cmake -S . -B ./PrjVisualStudio -G "Visual Studio 17 2022" -A x64