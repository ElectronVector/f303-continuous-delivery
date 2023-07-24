# f303-continuous-delivery

This is a test example demonstrating how to use Github Actions to build and flash firmware (on real hardware devices).

For more details on how to flash devices with a Raspberry Pi, see https://www.electronvector.com.

The skeleton of the project was reused from: https://github.com/cortexm/baremetal.
This provided all the startup code.

## Code structure

- `CmakeLists.txt` is Cmake building script
- `src` folder contain application code
- `src/startup` folder contain all startup code
- `ld` folder contain linker scripts
- `cmake/arm-none-eabi.cmake` is platform file which define compilers and configuration for this project

## Dependencies

- gcc-arm for embedded [download here](https://developer.arm.com/open-source/gnu-toolchain/gnu-rm/downloads)
- [cmake](https://cmake.org/download/)

## Setup

The Arm GCC toolchain (arm-none-eabi-) must be available in the path.
Alternatively, define a new toolchain file (the one here is provided in cmake/arm-none-eabi.cmake) and pass it to CMake with `-D CMAKE_TOOLCHAIN_FILE=../cmake/arm-none-eabi.cmake`. 

## Building

_Note this assumes you are using Make as the underlying build tool, however I hear Ninja is much better.

```
mkdir cmake-build-debug
cd cmake-build-debug
cmake -D CMAKE_TOOLCHAIN_FILE=../cmake/arm-none-eabi.cmake -D CMAKE_BUILD_TYPE=Debug ..
make
```

change `CMAKE_BUILD_TYPE=Release` to build a release version.

You can use also `make bin`, `srec`, `hex`, `flash`, ...

## Changing the architecture

In the CmakeLists.txt file change CPU_OPTIONS:
- architecture options:
    - Cortex-M0: `set(CPU_OPTIONS -mthumb -mcpu=cortex-m0)`
    - Cortex-M0+ `set(CPU_OPTIONS -mthumb -mcpu=cortex-m0plus)`
    - Cortex-M3 `set(CPU_OPTIONS -mthumb -mcpu=cortex-m3)`
    - Cortex-M4 `set(CPU_OPTIONS -mthumb -mcpu=cortex-m4)`
    - Cortex-M7 `set(CPU_OPTIONS -mthumb -mcpu=cortex-m7)`
    - Cortex-M23: `set(CPU_OPTIONS -mthumb -mcpu=cortex-m23)`
    - Cortex-M33: `set(CPU_OPTIONS -mthumb -mcpu=cortex-m33)`

- floating point, add options to CPU_OPTIONS:
    - soft float `-mfloat-abi=softfp`
    - hard float `-mfloat-abi=hard`
- for floating point also add fpv architecture
    - Cortex-M4 single precision `-mfpu=fpv4-sp-d16`
    - Cortex-M7 and M33 single-precision `-mfpu=fpv5-sp-d16`
    - Cortex-M7 and M33 signle and double-precision `-mfpu=fpv5-d16`

*Detailed info about options is in readme: gcc-arm directory: share/doc/gcc-arm-none-eabi/readme.txt
or in [ARM-Options](https://gcc.gnu.org/onlinedocs/gcc/ARM-Options.html) document*

## Using with a Black Magic Probe

The Black Magic Probe (BMP) is a JTAG/SWD debugger that runs its own GDB server _on the target_.
This allows you to use GDB directly over a TTY device without having to configure OpenOCD and run a GDB server on your host machine.

### Hardware setup

The BMP SWD signals need to be connect from the BMP to the target board.

On the STM32F303 Discovery board there is a header labeled ST-LINK that has the SWD signals present.
HOWEVER, this connector is designed for using the ST-Link on the board to program another STM32 on another board.
To use this connector to flash the target STM32F303 on the board, you must remove the jumpers and connect these signals:
- SWCLK: pin 2
- SWDIO: pin 4

Additionally 3.3V and ground from the target must also be connected to the BMP.

### Running GDB

GDB can be run from the GCC Arm toolchain with the build output like this:

```shell
arm-none-eabi-gdb cmake-build-debug/f303
```

When running from inside the project file, the .gdbinit file will be loaded with starting GDB.
This configures some things to make it easier to use.

## Using the ST-Link onboard the F303 Discovery board

There is an ST-Link embedded on the F303. To use it, just connect a USB cable to the USB port labeled USB ST-LINK.

Start openocd with this command:
```shell
openocd -f interface/stlink-dap.cfg -f target/stm32f3x.cfg
```

Then connect to it with GDB with:
```shell
arm-none-eabi-gdb cmake-build-debug/f303
```

From inside GDB you'll need to attach to the target (by default openocd uses 3333 as the GDB port):
```shell
target extended-remote :3333
```

And you can load the file with:
```shell
load
```

There is a GDB command file supplied that will flash the firmware and then exit.
You can run it by supplying it at the command line:
```shell
arm-none-eabi-gdb -x gdb-st-link-flash-and-exit cmake-build-debug/f303
```

Note that this does NOT require openocd server to be started manually.
This GDB command file starts and exits openocd on its own.