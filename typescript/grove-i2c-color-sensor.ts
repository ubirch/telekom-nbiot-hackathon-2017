// Grove I2C Color Sensor
// Example adapted from the Adafruit sensor library
// https://github.com/adafruit/Adafruit_TCS34725
//
// @author Matthias L. Jugel

const TCS34725_ADDRESS = 0x29;
const TCS34725_COMMAND_BIT = 0x80;
const TCS34725_ID = 0x12;
const TCS34725_ENABLE = 0x00;
const TCS34725_ATIME = 0x01;
const TCS34725_CONTROL = 0x0f;
const TCS34725_ENABLE_PON = 0x01;
const TCS34725_ENABLE_AEN = 0x02;
const TCS34725_CDATAL = 0x14;
const TCS34725_RDATAL = 0x16;
const TCS34725_GDATAL = 0x18;
const TCS34725_BDATAL = 0x1A;

enum TCS34725_INTEGRATIONTIME {
    TCS34725_INTEGRATIONTIME_2_4MS = 0xFF, /**<  2.4ms - 1 cycle    - Max Count: 1024  */
    TCS34725_INTEGRATIONTIME_24MS = 0xF6, /**<  24ms  - 10 cycles  - Max Count: 10240 */
    TCS34725_INTEGRATIONTIME_50MS = 0xEB, /**<  50ms  - 20 cycles  - Max Count: 20480 */
    TCS34725_INTEGRATIONTIME_101MS = 0xD5, /**<  101ms - 42 cycles  - Max Count: 43008 */
    TCS34725_INTEGRATIONTIME_154MS = 0xC0, /**<  154ms - 64 cycles  - Max Count: 65535 */
    TCS34725_INTEGRATIONTIME_700MS = 0x00 /**<  700ms - 256 cycles - Max Count: 65535 */
}

enum TCS34725_GAIN {
    TCS34725_GAIN_1X = 0x00, /**<  No gain  */
    TCS34725_GAIN_4X = 0x01, /**<  4x gain  */
    TCS34725_GAIN_16X = 0x02, /**<  16x gain */
    TCS34725_GAIN_60X = 0x03 /**<  60x gain */
}

function read8(reg: number): number {
    pins.i2cWriteNumber(TCS34725_ADDRESS, TCS34725_COMMAND_BIT | reg, NumberFormat.UInt8LE);
    return pins.i2cReadNumber(TCS34725_ADDRESS, NumberFormat.Int8LE);
}

function write8(reg: number, n: number): void {
    pins.i2cWriteNumber(TCS34725_ADDRESS, (TCS34725_COMMAND_BIT | reg) << 8 | (n & 0xff), NumberFormat.UInt16BE);
}

function read16(reg: number): number {
    pins.i2cWriteNumber(TCS34725_ADDRESS, TCS34725_COMMAND_BIT | reg, NumberFormat.UInt8LE);
    return pins.i2cReadNumber(TCS34725_ADDRESS, NumberFormat.UInt16LE);
}

let integrationTime = TCS34725_INTEGRATIONTIME.TCS34725_INTEGRATIONTIME_700MS;
let gain = TCS34725_GAIN.TCS34725_GAIN_1X;

function init() {
    let x = read8(TCS34725_ID);
    if ((x != 0x44) && (x != 0x10)) {
        console.log("FAIL");
    } else {
        console.log("OK")
    }

    write8(TCS34725_ATIME, integrationTime);
    write8(TCS34725_CONTROL, gain);

    write8(TCS34725_ENABLE, TCS34725_ENABLE_PON);
    basic.pause(3);
    write8(TCS34725_ENABLE, TCS34725_ENABLE_PON | TCS34725_ENABLE_AEN);
}

class RGBW {
    r: number;
    g: number;
    b: number;
    w: number;
    RGBW() {
        this.r = 0;
        this.g = 0;
        this.b = 0;
        this.w = 0;
    }
}

function getRawData(): RGBW {
    let color: RGBW = new RGBW();

    console.log("getRawData()");

    color.w = read16(TCS34725_CDATAL);
    color.r = read16(TCS34725_RDATAL);
    color.g = read16(TCS34725_GDATAL);
    color.b = read16(TCS34725_BDATAL);

    switch (integrationTime) {
        case TCS34725_INTEGRATIONTIME.TCS34725_INTEGRATIONTIME_2_4MS:
            basic.pause(3);
            break;
        case TCS34725_INTEGRATIONTIME.TCS34725_INTEGRATIONTIME_24MS:
            basic.pause(24);
            break;
        case TCS34725_INTEGRATIONTIME.TCS34725_INTEGRATIONTIME_50MS:
            basic.pause(50);
            break;
        case TCS34725_INTEGRATIONTIME.TCS34725_INTEGRATIONTIME_101MS:
            basic.pause(101);
            break;
        case TCS34725_INTEGRATIONTIME.TCS34725_INTEGRATIONTIME_154MS:
            basic.pause(154);
            break;
        case TCS34725_INTEGRATIONTIME.TCS34725_INTEGRATIONTIME_700MS:
            basic.pause(700);
            break;
    }
    return color;
}

init();

basic.forever(() => {
    let color = getRawData();
    serial.writeValue("w", color.w);
    serial.writeValue("r", color.r);
    serial.writeValue("g", color.g);
    serial.writeValue("b", color.b);
    basic.pause(1000);
});