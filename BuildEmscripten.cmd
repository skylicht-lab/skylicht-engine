call C:\emsdk\emsdk_env.bat
cmake -DCMAKE_TOOLCHAIN_FILE=C:\emsdk\fastcomp\emscripten\cmake\Modules\Platform\Emscripten.cmake  -S . -B ./PrjEmscripten -G "MinGW Makefiles"
mingw32-make -C PrjEmscripten