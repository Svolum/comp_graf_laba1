rmdir /s /q build
mkdir build
cd build
cmake ..
cmake --build . --config Release
cd Release
comp_grafika_laba1_var9.exe
cd ../..