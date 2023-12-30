# Bachelor
Monorepository for bachelor work

# Project structrure
```
src
 lib
 mcu
 ui
text
```
- `lib` contains shared code between MCU and Desktop app.
- `mcu` contains code for Car MCU.
- `ui` contains code for Desktop app.
- `text` contains bachelor work itself.

# Setup
## UI
### Dependencies
1. Cmake == 3.28
2. Qt >= 6.4

### Build
```sh
cmake -S . -B build
cmake --build build --parallel
```

## MCU
### Dependencies
1. MCUxpresso IDE

### Build
Import all eclipse projects from src folder and build it.
