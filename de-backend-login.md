### Gerät am Backend anmelden

Zuerst die ID des Calliope mini herausfinden, dazu <a href="https://raw.githubusercontent.com/ubirch/telekom-nbiot-hackathon-2017/master/deviceinfo.hex">deviceinfo.hex</a> auf den Calliope mini spielen. 

Auf dem Display wird dann die ID als eine HEX Zeichenkette ausgegeben (z.B. id:AB12CD34).

Auf <a href="http://ubirch.demo.ubirch.com">uBirchDemo</a> gehen und über 'Mit Google einloggen' anmelden.
Nun bitte die erhaltenen Zugangsdaten eingeben.

Jetzt kann der device angelegt werden, hierfür muss nur die ID des Calliope mini und ein Name für das Gerät eingegeben werden.

Nach dem Wechsel auf den Reiter 'devinfo' kann man den Abruf der vom Gerät gesendeten Daten einrichten.
Hierzu gibt es 2 Möglichkeiten:
#### 1. Streaming API
Um die Daten kontinuierlich zu empfangen kann die Streaming API verwendet werden.

Benötigt wird Mosquitto um auf den MQTT-Server zugreifen zu können.
Installiert wird dieser client durch die Eingabe:
>brew install mosquitto
(Vorraussetzung heirfür ist brew, sollte dies nicht installiert sein, kann dies über die Eingabe von:
>brew (ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
erfolgen)

Um die Daten von dem angelegten Calliope mini streamen zu können, muss nun das Topic subscribed werden. Dazu gibt man: 
>mosquitto_sub -h mq.demo.ubirch.com -p 1883 -t “ubirch-demo/ubirch/devices/$DEVICE_ID/processed” -u telekom -P SmartPublicLife2017
ein, wobei $DEVICE_ID durch die DeviceID des Geräts ersetzt wird. Diese findet sich auf dem Reiter 'additional settings'.

![DeviceID](files/show-deviceid.png)

Die vom Calliope mini gesendeten Daten werden ausgegeben.

![Streaming_Result](files/streaming-result.png)


#### 2. Query API
Curl muss vorhanden sein
