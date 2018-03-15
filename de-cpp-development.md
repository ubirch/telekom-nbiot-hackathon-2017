[![ubirch GmbH](files/ubirch.png)](https://ubirch.com)

# Calliope mini Entwicklung in C++

1. [Installation](#installation)
2. [Entwicklung](#entwicklung)
    1. [Debugging](#debugging)
        1. [Serielle Konsole](#serielle-konsole)
        2. [GDB](#gdb)
3. [Pins](#pins)

> Details zu Backend Registrierung und ECC Schlüsselerzeugung finden sich auf der [PXT Seite](de-pxt-development.md). 

## Installation

Diese Variante ist für erfahrene Entwickler geeignet und erfordert
die Installation eines Compilers, sowie des Entwicklungswerkzeugs 
[yotta](http://docs.yottabuild.org/#installing). 

Zur Installation dieser Anleitung folgen:

1. [dieses Repository](https://github.com/telekom-nbiot-hackathon-2017) clonen: `git clone https://github.com/ubirch/telekom-nbiot-hackathon-2017`
2. [yotta installieren](http://docs.yottabuild.org/#installing) (Englisch! Für MacOSX: bitte über Terminal installieren und nicht yotta.app nutzen)
3. Paket srecord installieren 
4. in das Verzeichnis des Beispielprojekts [`nbiot-cpp-template`](nbiot-cpp-template) wechseln und `yt update` ausführen 
5. zum compilieren `yt build` ausführen
6. die Datei `build/calliope-mini-classic-gcc/source/calliope-project-template-combined.hex` auf das USB Lauferk `MINI` kopieren

> **ACHTUNG:** Bitte den korrekten Wert für `COPS` in `main.cpp` auswählen. Dieser unterscheidet sich, je nachdem, 
> welches Netzwerk genutzt wird. Für Deutschland: `26201` und für Österreich: `23203`.
 
## Entwicklung

Die Quellen des Beispielprojects befinden sich in `source/main.cpp`. Im gleichen Verzeichnis befinden sich noch 
Helferbibliotheken in Unterverzeichnissen. 

> Nach jeder Änderung `yt build` (Punkt 4), sowie den Kopiervorgang (Punkt 5) wiederholen.

### Debugging

#### Serielle Konsole

Ausgaben auf der seriellen Konsole werden über den USB Anschluß ausgegeben. Der serielle Anschluß
muss dafür mit einem Terminalprogramm (Windows: [Termite](https://www.compuphase.com/software_termite.htm), Linux/MacOS: [screen](https://www.gnu.org/software/screen/)) abgefragt werden.

Beispiel (MacOS):
```
screen /dev/cu.usbmodem14232 115200
```
oder alternativ das Python serial terminal

```
pip install pyserial
mininterm.py /dev/cu.usbmodem14232 115200
```

Die Einstellungen für die serielle Konsole sind `115200 8N1`.

#### gdb

Es existiert auch die Möglichkeit live auf dem nRF51 zu debuggen. Dazu muß das Programm mit 
debug code compiliert werden: `config.json` editieren und `debug` auf `1` setzen.

Danach kann mit Hilfe von [OpenOCD](http://openocd.org/): `openocd -f openocd.cfg` der debug server gestartet werden und 
mit `arm-none-eabi-gdb` die debug session starten:

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

> C18/C19 sind auf I<sup>2</sup>C fest eingestellt und können nicht für digitale Operationen genutzt werden.