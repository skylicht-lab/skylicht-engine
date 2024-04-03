cd ..
cmake -S . -B ./PrjVisualStudio -G "Visual Studio 17 2022" -A x64 -DINSTALL_LIBS=ON
cmake --build ./PrjVisualStudio --target install --config Debug