This project uses openaccess commands to create layout and provide a Cython
extension for Python.

To build this project, change .cshrc to contain correct path to OA include/link libraries.

To build C++ shared library:

1. make build folder, then run cmake:
  
mkdir build
cd build
cmake ..

2. run make to create shared library and test executable.

make

To build Cython extension:

1. type the following:

cd python
python setup.py build

2. a cybagoa.so shared library file will be built.  This shared library file can be imported into python and use to create layout.

