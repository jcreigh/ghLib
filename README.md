# ghLib

[![Build Status](https://travis-ci.org/jcreigh/ghLib.svg?branch=master)](https://travis-ci.org/jcreigh/ghLib)

ghLib is a C++ helper library created by The Gearheads (Team 1189) for use with FRC.

## Prerequisites

- CMake 2.8 or newer
- Native Toolchain (To build locally)
- ARM Toolchain (To crosscompile for use on the robot)
- doxygen (optional, to build documentation)

## Dependencies
The following dependencies are automatically downloaded and built when needed

- [narflog](https://github.com/jcreigh/narflog)
- [narflib](https://github.com/narfblock/narflib)
- [ntcore](https://github.com/PeterJohnson/ntcore)
- [wpilib](https://usfirst.collab.net/sf/projects/wpilib/)
- [gtest](http://googletest.googlecode.com/)

## Building
```bash
mkdir build
cd build
cmake ..
```

To build natively, use `cmake -DROBOT=OFF ..` instead

### Library
```bash
make
```

### Documentation
```bash
make docs
```

### Packaging
```bash
cpack ..
```
