// show the microphone values as a bar graph
// The mic is analogue and has its base around ~512
//
// @author Matthias L. Jugel

let Platzhalter = 0
basic.forever(() => {
    Platzhalter = pins.analogReadPin(AnalogPin.MIC)
    serial.writeValue("mic", Platzhalter)
    led.plotBarGraph(
        Math.abs(Platzhalter - 512),
        512
    )
})