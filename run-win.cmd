rmdir /s /q build
mkdir build
cd build
cmake -S ./.. -B ./
cmake --build ./
rendevox.exe
