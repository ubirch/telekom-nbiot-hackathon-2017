# ubirch NB-IoT hackathon C++ template

- first, install [yotta](http://docs.yottabuild.org/#installing)
- run `yt update` once, it will download all required dependencies

# ECC key

To use the [ubirch backend](http://ubirch.demo.ubirch.com/) you will need to create
your own ECC key. A simple way to generate a key and register your Callioper mini
with the backend, run the provides `ecc-generate.py` script.

However, before you do that run [deviceinfo.hex](https://raw.githubusercontent.com/ubirch/telekom-nbiot-hackathon-2017/master/deviceinfo.hex)
on your Calliope mini and write down the 8 hex digit ID.

```bash
# you may need to install the ed25519 library
pip install ed25519
python ecc-generate.py
```

The script outputs C-code that can be directly included into `source/main.cpp`. Simply replace
the test keys. The script also registers your device and the public key with the ubirch
backend.

Now you are ready to compile (`yt build`) and copy the file `build/calliope-mini-classic-gcc/source/calliope-project-template-combined.hex`
onto the `MINI` USB drive. Log into the backend and the temperature value should show up.


# IDE use

You can use any C/C++ IDE you want. Options include
[CLion](https://www.jetbrains.com/clion/),
[Eclipse CDT](https://eclipse.org/cdt/), and
[Visual Studio Code](https://code.visualstudio.com/).

> The included `CMakeLists.txt` file is for the use of the
> [CLion](https://www.jetbrains.com/clion/) IDE.
> It supports syntax highlighting, navigation, refactoring and building
> with the custom target `compile`. Do not use run,
> it won't work.
> <br/>
> Select the target `compile` and press the build button left of it:
> ![compile](clion_compile.png)

To enable GDB debugging, edit `config.json` and set `debug` to `1`.
Install [OpenOCD](http://openocd.org/) and run `openocd -f openocd.cfg` and use gdb remote debugging. Then use this
to commence debugging:

```
$ arm-none-eabi-gdb build/calliope-mini-classic-gcc/source/calliope-project-template
(gdb) target remote localhost:3333
(gdb) break main
(gdb) mon reset
(gdb) c
```


# License

This template is available under the [Apache License](LICENSE)

```
Copyright 2017 Calliope gGmbH

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
````



