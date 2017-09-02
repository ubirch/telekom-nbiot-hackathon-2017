/**
 * This is a simple template for use with Calliope mini.
 *
 * @copyright (c) Calliope gGmbH.
 * @author Matthias L. Jugel.
 *
 * Licensed under the Apache License 2.0
 */

#include <MicroBit.h>

MicroBit uBit;


int main(void) {
    uBit.init();
    uBit.serial.baud(115200);
    uBit.serial.send("Calliope mini template v1.0\r\n");

    uBit.serial.send("Hello world!\r\n");
}