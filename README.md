# ghLib

ghLib is a C++ helper library created by The Gearheads (Team 1189) for use with FRC.

## Prerequisites

- CMake 2.6 or newer
- GoogleTest (gtest) development libraries (optional, used for unit tests)
- Native Toolchain (To build locally)
- ARM Toolchain (To crosscompile for use on the robot)
- doxygen (optional, to build documentation)

## Dependencies
The following dependencies are automatically downloaded and built

- [narflog](https://github.com/jcreigh/narflog)
- [narflib](https://github.com/narfblock/narflib)
- [ntcore](https://github.com/PeterJohnson/ntcore)
- [wpilib](https://usfirst.collab.net/sf/projects/wpilib/)

## Building

### Native
```bash
mkdir build
cd build
cmake -DROBOT=OFF ..
make
```

### ARM (Robot)
```bash
mkdir build
cd build
cmake ..
make
```

### Documentation
```bash
doxygen
```

