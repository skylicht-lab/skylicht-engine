cd..
cd Assets
python BuildAssetBundles.py
cd..
mkdir Bin\BuiltIn
copy Bin\BuiltIn.zip Bin\BuiltIn
cmake -DCMAKE_TOOLCHAIN_FILE=C:\emsdk\upstream\emscripten\cmake\Modules\Platform\Emscripten.cmake  -S . -B ./PrjEmscripten -G "MinGW Makefiles" -DCMAKE_SH="CMAKE_SH-NOTFOUND
C:\MinGW\bin\mingw32-make.exe -C PrjEmscripten