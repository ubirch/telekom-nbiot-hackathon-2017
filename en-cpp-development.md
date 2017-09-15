[![ubirch GmbH](files/ubirch.png)](https://ubirch.com)

# Calliope mini Development using C++

1. [Installation](#installation)
2. [Development](#development)
    1. [Debugging](#debugging)
        1. [Serial Console](#serial-console)
        2. [GDB](#gdb)
3. [Pins](#pins)
  
> Details on backend registration and ECC key generation can be found on the [PXT page](en-pxt-development.md). 
      
## Installation

This is for advanced developers who have worked with C/C++ before or dare to handle the development environment.
It is required to install the development tool [yotta](http://docs.yottabuild.org/#installing) and the required
compiler suite for ARM Cortex-M processors.

To install the development environment, follow these steps:

1. [clone this repository](https://github.com/telekom-nbiot-hackathon-2017): `git clone https://github.com/ubirch/telekom-nbiot-hackathon-2017`
2. [install yotta](http://docs.yottabuild.org/#installing)
3. change into the directory [`nbiot-cpp-template`](nbiot-cpp-template) and run `yt update` on the command line 
4. use `yt build` to compile
5. copy the resulting `build/calliope-mini-classic-gcc/source/calliope-project-template-combined.hex` file to the `MINI` USB drive

## Development

The sources for the example project are located in `source/main.cpp`. The same directory also contains additional
library code to aid the usage of the NB-IoT module. 

> Remember to execute `yt build` (#4), as well as the copy step (#5) after you changed code.

### Debugging

#### Serial Console

Output to the serial console on the Calliope mini can be watched using the USB serial port. To do that, use a terminal
program (Windows: [Termite](https://www.compuphase.com/software_termite.htm), Linux/MacOS: [screen](https://www.gnu.org/software/screen/)).

Example (MacOS):
```
screen /dev/cu.usbmodem14232 115200
```
or the python serial terminal
```
pip install pyserial
mininterm.py /dev/cu.usbmodem14232 115200
```

The settings for the serial port are `115200 8N1`.

#### gdb

Alternatively, you can also directly debug the nRF51. To do that, compile your program with debug info: 
edit `config.json` and change `debug` to `1`.

Now you can install and run the [OpenOCD](http://openocd.org/): `openocd -f openocd.cfg` debug server and attach to the Calliope mini
using `arm-none-eabi-gdb` as follows:

```
$ arm-none-eabi-gdb build/calliope-mini-classic-gcc/source/calliope-project-template
(gdb) target remote localhost:3333
(gdb) break main
(gdb) mon reset
(gdb) c
```

## Pins

```
C0/P0 = MICROBIT_ID_IO_P12  // edge connector 0
C1/P1 = MICROBIT_ID_IO_P0   // edge connector 1
C2/P2 = MICROBIT_ID_IO_P1   // edge connector 2, analog
C3/P3 = MICROBIT_ID_IO_P16  // edge connector 3, analog
C4    = MICROBIT_ID_IO_P3   // LED matrix COL1
C5    = MICROBIT_ID_IO_P4   // LED matrix COL2
C6    = MICROBIT_ID_IO_P10  // LED matrix COL3
C7    = MICROBIT_ID_IO_P13  // LED matrix COL4
C8    = MICROBIT_ID_IO_P14  // LED matrix COL5
C9    = MICROBIT_ID_IO_P15  // LED matrix COL6
C10   = MICROBIT_ID_IO_P9   // LED matrix COL7
C11   = MICROBIT_ID_IO_P7   // LED matrix COL8
C12   = MICROBIT_ID_IO_P6   // LED matrix COL9
C16   = MICROBIT_ID_IO_P2   // RX, analog, Grove
C17   = MICROBIT_ID_IO_P8   // TX, analog, Grove
C18   = MICROBIT_ID_IO_P20  // SDA, Grove
C19   = MICROBIT_ID_IO_P19  // SCL, Grove
MIC   = MICROBIT_ID_IO_P21  // Microphone 
```

> C18/C19 are fixed to I<sup>2</sup>C and can't be used for digital right now.