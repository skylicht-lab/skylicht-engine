cd ..
cd Assets
python BuildAssetBundles.py
cd ..
mkdir PrjUWP
mkdir PrjUWP\Projects

cmake -S . -B ./PrjUWP -G "Visual Studio 16 2019" -A x64 -DCMAKE_SYSTEM_NAME=WindowsStore -DCMAKE_SYSTEM_VERSION=10.0