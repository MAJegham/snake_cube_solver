mkdir -p build
cd ./build
cmake -C ../preload.cmake ..
make
./sudoku
