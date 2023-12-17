#/bin/bash
rm -rf build
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build