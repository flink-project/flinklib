How to build flinklib from source
=================================

Requirements
------------

  - Linux based operating system (support for other platforms will be added in a further release)
  - GCC 4.6 or newer (clang/llvm should work too, but is not tested)
  - CMake 2.8 or newer
  - GNU make

Local build
-----------

  1. Clone git repository: `$ git clone https://github.com/flink-project/flinklib.git`
  2. Create a build directory: `$ mkdir build-local`
  3. Enter the build directory and setup your build environment with CMake: `$ cmake ..`
     - Optional: you can specify an installation prefix path: `-DCMAKE_INSTALL_PREFIX:PATH=/usr/local/`
  4. Build the source code: `$ make`
  5. Install the library and header files on your system: `$ make install`

Cross build
-----------

  1. Clone git repository: `$ git clone https://github.com/flink-project/flinklib.git`
  2. Create a build directory, e.g: `$ mkdir build-powerpc`
  3. Create a CMake toolchain file. For more informations, please have a look at the [CMake Wiki](http://www.cmake.org/Wiki/CMake_Cross_Compiling).
  4. Enter the build directory and setup your build environment with CMake: `$ cmake -DCMAKE_TOOLCHAIN_FILE:PATH=/path/to/your/toolchain/file.cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr/powerpc-linux-gnu/ ..`
  5. Build the source code: `$ make`
  6. Install the library and header files on your system: `$ make install`
