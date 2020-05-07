cd ..
cd Assets
python BuildAssetBundles.py
cd ..
mkdir PrjUWP
mkdir PrjUWP\Projects
mkdir PrjUWP\Projects\Template
mkdir PrjUWP\Projects\Template\Assets
copy Projects\Template\Platforms\UWP\Assets\*.* PrjUWP\Projects\Template\Assets
copy Bin\*.zip PrjUWP\Projects\Template\Assets

cmake -S . -B ./PrjUWP -G "Visual Studio 15 2017" -A x64 -DCMAKE_SYSTEM_NAME=WindowsStore -DCMAKE_SYSTEM_VERSION=10.0