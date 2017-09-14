// Grove Ultrasonic Ranger
// may need to use a different digital pin(!)
// not having float reduces accuracy here ...

let pin = DigitalPin.C16

basic.forever(() => {
    pins.digitalWritePin(pin, 0);
    pins.digitalWritePin(pin, 1);
    control.waitMicros(5);
    pins.digitalWritePin(pin, 0);
    let duration = pins.pulseIn(pin, 1);
    let rangeInCentimeters = duration / 29 / 2;

    console.log("range: " + rangeInCentimeters);
})
