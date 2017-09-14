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

Benötigt wird <a href="https://mosquitto.org/download/">Mosquitto</a> um auf den MQTT-Server zugreifen zu können.

Um die Daten von dem angelegten Calliope mini streamen zu können, muss nun das Topic subscribed werden. Dazu gibt man: 
>mosquitto_sub -h mq.demo.ubirch.com -p 1883 -t “ubirch-demo/ubirch/devices/$DEVICE_ID/processed” -u telekom -P SmartPublicLife2017

ein, wobei $DEVICE_ID durch die DeviceID des Geräts ersetzt wird. Diese findet sich auf dem Reiter 'additional settings'.

![DeviceID](files/show-deviceid.png)

Die vom Calliope mini gesendeten Daten werden ausgegeben.

![Streaming_Result](files/streaming-result.png)


#### 2. Query API
Vorraussetzung ist <a href="https://curl.haxx.se/download.html">Curl</a>.

Der Abruf der Daten erfolgt durch den Aufruf:
>DEVICEID=$DEVICE_ID
>TOKEN=ya29.GlvGBNBxm5fa84UTyEi23JYSZ3E-OCOY8wVRAkmFaDwMfzCYtlc1TXuxBhnHLCNtIW26Z2yQGzO3EkPRsAIeWUeUEnzAfopy2f_FluXYl5Yp7OZyJjOnzEsxFmRk
>HOST=http://api.ubirch.demo.ubirch.com:8080
>
>#last 10 datapoints
>
>curl -XGET -H 'Authorization: Bearer $TOKEN' $HOST/api/avatarService/v1/device/$DEVICEID/data/history/0/10

wobei $DEVICE_ID durch die DeviceID des Geräts ersetzt wird. Diese findet sich auf dem Reiter 'additional settings'.

![DeviceID](files/show-deviceid.png)