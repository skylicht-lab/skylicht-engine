call C:\emsdk\emsdk_env.bat
cmake -DCMAKE_TOOLCHAIN_FILE=C:\emsdk\upstream\emscripten\cmake\Modules\Platform\Emscripten.cmake  -S . -B ./PrjEmscripten -G "MinGW Makefiles"
mingw32-make -C PrjEmscripten