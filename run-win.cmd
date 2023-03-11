rmdir /s /q build
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Build -DCMAKE_MAKE_PROGRAM=C:/msys64/mingw64/bin/ninja.exe -DCMAKE_C_COMPILER=C:/msys64/mingw64/bin/gcc.exe -DCMAKE_CXX_COMPILER=C:/msys64/mingw64/bin/g++.exe -G Ninja -S ./.. -B ./
cmake --build ./
rendevox.exe
