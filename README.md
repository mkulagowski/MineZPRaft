# MineZPRaft

MineZPRft is an university project, which recreates Creative Mode from Minecraft.

Written in C++, uses OpenGL 3.3 to render.

Supported platforms:
* Windows
* Linux (tested on Ubuntu 14.04.3 LTS and Fedora 23)

## Building the project - Linux

Download necessary tools:

```
# Ubuntu:
sudo apt-get install build-essentials cmake g++ libgl1-mesa-dev

# Fedora:
sudo dnf install cmake gcc gcc-c++ mesa-libGL-devel
```

Navigate to project directory and run:

```
Scripts/rebuild-all.sh
```

The script will build all possible configurations. One can also build a specific configuration manually:

```
# replace <conf> with "Debug" or "Release"
cmake . -DCMAKE_BUILD_TYPE=<conf>
make
```

## Building the project - Windows

Visual Studio 2015 is required, however an Express Edition should be enough. Make sure that Windows SDK is installed as well.

Run MineZPRft.sln inside Visual Studio and make sure that Google Test is visible for the compiler. Then, the project should be buildable right away.

## Known Issues - Linux

### Segmentation fault right after launch

Google Test requires pthread library to be preloaded. To quickly check if this is the case, run MineZPRft with command:

```
LD_PRELOAD=/lib/x86_64-linux-gnu/libpthread.so.0 ./MineZPRft
```

If the application launches successfully, we recommend defining `LD_PRELOAD` env variable globally, so each reboot it will be remembered.

## Known Issues - Windows

None (so far...)
