# ghLib

[![Build Status](https://travis-ci.org/jcreigh/ghLib.svg?branch=master)](https://travis-ci.org/jcreigh/ghLib)

ghLib is a C++ helper library created by The Gearheads (Team 1189) for use with FRC.

See the [github page](http://jcreigh.github.io/ghLib/) for installation instructions.

## Prerequisites

- java (for gradle)
- Native Toolchain (To build locally)
- ARM Toolchain (To crosscompile for use on the robot)
- doxygen (optional, to build documentation)

## Dependencies
The following dependencies are automatically downloaded and built when needed

- [narflog](https://github.com/jcreigh/narflog)
- [narflib](https://github.com/narfblock/narflib)
- [ntcore](https://github.com/wpilibsuite/ntcore)
- [wpilib](https://github.com/wpilibsuite/allwpilib)
- [gtest](https://github.com/google/googletest)

### Build Library for Robot
```bash
./gradlew ghLibStaticLibrary
```

### Build Library for Desktop
```bash
./gradlew ghLibDesktopTestStaticLibrary
```

### Run Tests
```bash
./gradlew checkGhLibDesktopStaticLibrary
```

### Documentation
```bash
./gradlew doxygen
```

### Packaging
```bash
./gradlew ghLibZip
```
