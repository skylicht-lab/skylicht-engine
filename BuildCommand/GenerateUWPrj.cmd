cd..
cd Assets
python BuildAssetBundles.py
cd ..
mkdir PrjUWP
mkdir PrjUWP\Projects
mkdir PrjUWP\Projects\MainApp
mkdir PrjUWP\Projects\MainApp\Assets
copy Projects\MainApp\Platforms\UWP\Assets\*.* PrjUWP\Projects\MainApp\Assets
copy Bin\BuiltIn* PrjUWP\Projects\MainApp\Assets

cmake -S . -B ./PrjUWP -G "Visual Studio 15 2017" -A x64 -DCMAKE_SYSTEM_NAME=WindowsStore -DCMAKE_SYSTEM_VERSION=10.0