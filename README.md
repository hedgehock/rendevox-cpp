# Rendevox

## Structure

- ##### app `User's functions`
- ##### rendevox-base `Core of rendevox`, 
  - ##### includes 
    - `Windowing`
    - `2D Rendering`
    - `Text Rendering`
## Dependencies

#### Install for Windows via [MSYS2](https://repo.msys2.org/distrib/x86_64/) latest version
```shell
$ pacman -S mingw-w64-x86_64-toolchain
$ pacman -S mingw-w64-x86_64-cmake
$ pacman -S mingw-w64-x86_64-glfw
$ pacman -S mingw-w64-x86_64-vulkan-devel
```

- C++17 compiler
- C++ standard libraries
- CMake
- Glfw
- Vulkan

## Features
- 2D & 3D rendering
- text rendering
- basic physics
- audio playback

## Template

```cpp
#include "../rendevox/rendevox.hpp"

int main() {
	Window window = Window("OpenGL", 1280, 720, "RVX", false);

	return 0;
}
```

## How to run

1. Create directory `app` and create file `main.cpp` inside
2. Paste code from template

### CLion
- Create `CMake Application` configuration in CLion with `Target` = `All Targets` and `executable` = `rendevox`

### Shell

#### Linux
``` shell
$ run-linux.sh
```
