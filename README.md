# Bachelor
Monorepository for bachelor work

## Setup
### CarQt
#### Dependencies
1. Cmake == 3.28
2. Qt >= 6.4

#### Build
```sh
cmake -S . -B build
cmake --build build --parallel
```

### MCU
#### Dependencies
1. MCUxpresso IDE

#### Build
Import all eclipse projects from repo root and build it.

## Dockerfiles
In repository I have 2 dockerfiles:
1. Qt.Dockerfile
2. Tex.Dockerfile

First for CarQt project and second one for Text project.
To build docker image run command:
```sh
docker build -t tag -f filename .
```
