[![ubirch GmbH](files/ubirch.png)](https://ubirch.com)

# Calliope mini Development using C++

1. [Installation](#installation)
2. [Development](#development)
    1. [Debugging](#debugging)
        1. [Serial Console](#seriel-console)
        2. [GDB](#gdb)
        
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
