cd..
call C:\Devtools\emsdk\emsdk_env.bat
call emcmake cmake -S . -B ./PrjEmscripten
cd Assets
python BuildAssetBundles.py
cd ..
mkdir Bin\BuiltIn
copy Bin\BuiltIn.zip Bin\BuiltIn
call emmake make -C PrjEmscripten
copy Projects\MainApp\Platforms\Emscripten\Index.html Bin
emrun --browser chrome Bin\Index.html