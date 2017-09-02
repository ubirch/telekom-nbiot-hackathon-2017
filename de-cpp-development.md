[![ubirch GmbH](https://www.ubirch.com/wp-content/uploads/2016/02/ubirch.png)](https://ubirch.com)

# Calliope mini Entwicklung in C++

Diese Variante ist für erfahrene Entwickler geeignet und erfordert
die Installation eines Compilers, sowie des Entwicklungswerkzeugs 
[yotta](http://docs.yottabuild.org/#installing). 

Zur Installation dieser Anleitung folgen:

1. [dieses Repository](https://github.com/telekom-nbiot-hackathon-2017) clonen: `git clone https://github.com/ubirch/telekom-nbiot-hackathon-2017`
2. [yotta installieren](http://docs.yottabuild.org/#installing) (Englisch!)
3. in das Verzeichnis des Beispielprojekts [`nbiot-cpp-template`](nbiot-cpp-template) wechseln und `yt update` ausführen 
4. zum compilieren `yt build` ausführen
5. die Datei `build/calliope-mini-classic-gcc/source/calliope-project-template-combined.hex` auf das USB Lauferk `MINI` kopieren

## Entwickung

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
