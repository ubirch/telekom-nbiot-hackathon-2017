// show the microphone values as a bar graph
// The mic is analogue, you may need to change the used pin(!)
//
// @author Matthias L. Jugel

let Platzhalter = 0
basic.forever(() => {
    for (let i = 0; i < 32; i++) {
        Platzhalter += pins.analogReadPin(AnalogPin.C16)
    }
    Platzhalter = Platzhalter >> 5;
    led.plotBarGraph(
        Platzhalter,
        1024
    )
    Platzhalter = 0;
})
