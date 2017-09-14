// check if someone crossed the line and show a happy face
//
// @author Matthias L. Jugel

pins.onPulsed(DigitalPin.C16, PulseValue.Low, () => {
    basic.showIcon(IconNames.Happy)
    basic.pause(100)
    basic.clearScreen()
})