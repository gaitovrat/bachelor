# Bachelor
Monorepository for bachelor work

## Setup
### CarQt
#### Dependencies
1. Cmake == 3.28
2. Qt >= 6.4
3. OpenCV 4.x

#### Build
```sh
cmake -S . -B build
cmake --build build --parallel CarQt
```

### MCU
#### Dependencies
1. MCUxpresso IDE

#### Build
Import all eclipse projects from repo root and build it.
