const BMP280_ADDRESS = 0x77;
const BMP280_REGISTER_CHIPID = 0xd0;
const BMP280_REGISTER_CONTROL = 0xf4;
const BMP280_REGISTER_TEMPDATA = 0xfa;

const BMP280_REGISTER_DIG_T1 = 0x88;
const BMP280_REGISTER_DIG_T2 = 0x8A;
const BMP280_REGISTER_DIG_T3 = 0x8C;
const BMP280_REGISTER_DIG_P1 = 0x8E;
const BMP280_REGISTER_DIG_P2 = 0x90;
const BMP280_REGISTER_DIG_P3 = 0x92;
const BMP280_REGISTER_DIG_P4 = 0x94;
const BMP280_REGISTER_DIG_P5 = 0x96;
const BMP280_REGISTER_DIG_P6 = 0x98;
const BMP280_REGISTER_DIG_P7 = 0x9A;
const BMP280_REGISTER_DIG_P8 = 0x9C;
const BMP280_REGISTER_DIG_P9 = 0x9E;

function read8(reg: number): number {
    pins.i2cWriteNumber(BMP280_ADDRESS, reg, NumberFormat.UInt8BE);
    return pins.i2cReadNumber(BMP280_ADDRESS, NumberFormat.Int8BE);
}

function write8(reg: number, n: number): void {
    pins.i2cWriteNumber(BMP280_ADDRESS, (reg << 8) | (n & 0xff), NumberFormat.UInt16BE);
}

function read16(reg: number, format: NumberFormat = NumberFormat.UInt16BE): number {
    pins.i2cWriteNumber(BMP280_ADDRESS, reg, NumberFormat.UInt8LE);
    return pins.i2cReadNumber(BMP280_ADDRESS, NumberFormat.UInt16LE);
}

function read24(reg: number): number {
    pins.i2cWriteNumber(BMP280_ADDRESS, reg, NumberFormat.UInt8LE);
    let buf = pins.i2cReadBuffer(BMP280_ADDRESS, 3)
    let r = buf.slice(0, 1).getNumber(NumberFormat.Int8LE, 0) << 16;
    r += buf.slice(1, 1).getNumber(NumberFormat.Int8LE, 0) << 8
    r += buf.slice(2, 1).getNumber(NumberFormat.Int8LE, 0);
    return r;
}

class Calibration {
    T1: number;
    T2: number;
    T3: number;

    P1: number;
    P2: number;
    P3: number;
    P4: number;
    P5: number;
    P6: number;
    P7: number;
    P8: number;
    P9: number;
}
let calibration = new Calibration();

function readCoefficients() {
    calibration.T1 = read16(BMP280_REGISTER_DIG_T1, NumberFormat.UInt16LE);
    calibration.T2 = read16(BMP280_REGISTER_DIG_T2, NumberFormat.Int16LE);
    calibration.T3 = read16(BMP280_REGISTER_DIG_T3, NumberFormat.Int16LE);

    calibration.P1 = read16(BMP280_REGISTER_DIG_P1, NumberFormat.UInt16LE);
    calibration.P2 = read16(BMP280_REGISTER_DIG_P2, NumberFormat.Int16LE);
    calibration.P3 = read16(BMP280_REGISTER_DIG_P3, NumberFormat.Int16LE);
    calibration.P4 = read16(BMP280_REGISTER_DIG_P4, NumberFormat.Int16LE);
    calibration.P5 = read16(BMP280_REGISTER_DIG_P5, NumberFormat.Int16LE);
    calibration.P6 = read16(BMP280_REGISTER_DIG_P6, NumberFormat.Int16LE);
    calibration.P7 = read16(BMP280_REGISTER_DIG_P7, NumberFormat.Int16LE);
    calibration.P8 = read16(BMP280_REGISTER_DIG_P8, NumberFormat.Int16LE);
    calibration.P9 = read16(BMP280_REGISTER_DIG_P9, NumberFormat.Int16LE);
}

function init(): boolean {
    let x = read8(BMP280_REGISTER_CHIPID);
    if (x != 0x58) {
        return false;
    }

    readCoefficients();

    write8(BMP280_REGISTER_CONTROL, 0x3F);

    return true;
}

// it is basically impossible to do this correctly in PXT typescript
function readTemperature(): number {
    let adc_T = read24(BMP280_REGISTER_TEMPDATA);
    adc_T >>= 4;

    let var1 = ((((adc_T >> 3) - (calibration.T1 << 1))) *
        (calibration.T2)) >> 11;

    let var2 = (((((adc_T >> 4) - (calibration.T1)) *
        ((adc_T >> 4) - (calibration.T1))) >> 12) *
        (calibration.T3)) >> 14;

    let t_fine = var1 + var2;

    let T = (t_fine * 5 + 128) >> 8;
    return T / 100;
}


if (!init()) {
    console.log("Could not find BMP280");
} else {
    console.log("BMP280 found");
}

basic.forever(() => {
    console.log("Temp: " + readTemperature());
    basic.pause(200);
})

