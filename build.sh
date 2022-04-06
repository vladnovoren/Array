mkdir -p build
cd build
cmake ..
cmake --build ./ --target all -- -j 6
