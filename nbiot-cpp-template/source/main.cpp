/**
 * This is a simple template on how to use the BC95 NB-IoT modem in conjunction
 * with the ubirch backend.
 *
 * If you want to know more about the protocol, see https://github.com/ubirch/ubirch-protocol
 *
 * @copyright (c) ubirch GmbH.
 * @author Matthias L. Jugel.
 *
 * Licensed under the Apache License 2.0
 */

#include <MicroBit.h>
#include "utils/utils.h"
#include <armnacl.h>
#include <CryptoUbirchProtocol.h>

// ATTENTION: Select the correct COPS value depending on the network you are on (Germany or Austria)
//#define COPS  "26201"   // Telekom-DE
#define COPS  "23203"     // Telekom-AT

// if you want to see AT commands and responses, uncomment this
//#define SHOW_AT_COMMANDS

#ifdef SHOW_AT_COMMANDS
#define DEBUGAT(prefix, info) log(prefix, info)
#else
#define DEBUGAT(prefix, info)
#endif

#define LOG(info) log("!!!", info)

/**
 * Of course this is totally unsafe. The keys are not secured in any way.
 * Since the Calliope mini gets erased every time we re-flash a new firmware this is a
 * simple shortcut. Do not do this for production devices.
 *
 * REPLACE THESE KEYS WITH ONE YOU CREATED. THIS IS JUST FOR TESTING.
 *
 * You can use the provided ecc-generate.py script to generate your own key.
 *
 */

/* ==== ECC KEYS ================= */
unsigned char ed25519_public_key[crypto_sign_PUBLICKEYBYTES] = {
0x7b, 0x60, 0x88, 0x2b, 0xee, 0x2e, 0x3f, 0x01, 0x79, 0x07, 0xce, 0x84, 0xe5, 0xe1, 0xc8, 0x7f,
0x70, 0x57, 0x60, 0xfa, 0xc5, 0x87, 0x7f, 0xe0, 0xde, 0x7c, 0x58, 0x06, 0xc4, 0x69, 0x1f, 0x2f,
};
unsigned char ed25519_secret_key[crypto_sign_SECRETKEYBYTES] = {
0x3b, 0xc4, 0xb2, 0x49, 0x9d, 0x97, 0x50, 0x1a, 0xba, 0x63, 0xb0, 0xf6, 0x30, 0x8f, 0x8d, 0x91,
0x3b, 0xcb, 0xdc, 0xed, 0xa8, 0x53, 0x26, 0x9d, 0x68, 0x75, 0xa0, 0x6a, 0x64, 0x32, 0xfa, 0xc7,
0x7b, 0x60, 0x88, 0x2b, 0xee, 0x2e, 0x3f, 0x01, 0x79, 0x07, 0xce, 0x84, 0xe5, 0xe1, 0xc8, 0x7f,
0x70, 0x57, 0x60, 0xfa, 0xc5, 0x87, 0x7f, 0xe0, 0xde, 0x7c, 0x58, 0x06, 0xc4, 0x69, 0x1f, 0x2f,
};

MicroBit uBit;
CryptoUbirchProtocol ubirch;

// log a message to the USB console
// this will switch the serial back and forth between the modem and the USB, not nice
void log(const char *prefix, const char *message) {
    uBit.sleep(100);
    while (uBit.serial.txInUse()) uBit.sleep(10);
    uBit.serial.redirect(USBTX, USBRX);
    uBit.serial.baud(115200);
    uBit.serial.send(ManagedString(prefix) + " " + ManagedString(message) + "\r\n");
    while (uBit.serial.txInUse()) uBit.sleep(10);
    uBit.serial.redirect(MICROBIT_PIN_P8, MICROBIT_PIN_P2);
    uBit.serial.baud(9600);
    uBit.sleep(100);
}

// send an AT command and expect OK, returns the first line of the response
ManagedString expectOK(ManagedString command) {
    DEBUGAT("+++", command.toCharArray());

    ManagedString atcmd = "\rAT" + command + "\r\n";

    uBit.sleep(100);
    uBit.serial.send(atcmd);

    ManagedString firstResponse;
    ManagedString s;
    do {
        s = uBit.serial.readUntil("\r\n", MicroBitSerialMode::SYNC_SPINWAIT);
        if (s.length() > 0) {
            DEBUGAT("---", s.toCharArray());
            if (firstResponse.length() == 0) firstResponse = s;
        }
        if (s == "ERROR") return s;
    } while (!(s == "OK"));
    return ManagedString(firstResponse);
}


// initialize the BC95 NB-IoT Modem, checks the firmware and updates mandatory settings
bool initializeModem() {
    // setup some basics
    expectOK("+NCONFIG=AUTOCONNECT,TRUE");
    expectOK("+NCONFIG=CR_0354_0338_SCRAMBLING,TRUE");
    expectOK("+NCONFIG=CR_0859_SI_AVOID,TRUE");
    return true;
}

// attach to the NB-IoT network, waits 1s per try
bool attach(int tries) {
    expectOK("+CFUN=1");
    if (expectOK("+COPS=1,2,\"" COPS "\"") == "OK") {
        for (int i = 0; i < tries; i++) {
            if (expectOK("+CGATT?") == "+CGATT:1") return true;
            uBit.sleep(1000);
        }
    }
    return false;
}

// send a message to the backend
bool send(ManagedString &message, const char *server, int port) {
    if (message.length() > 0 && server && port != 0) {
        // open the socket and remember the socket number (44567 is our receive port, ignored)
        ManagedString socket = expectOK("+NSOCR=DGRAM,17,44567,1");
        if (socket.length() > 0 && !(socket == "ERROR")) {
            // send UDP packet
            bool sent = !(expectOK("+NSOST=" + socket + "," + server + "," + port
                                   + "," + message.length() + "," + stringToHex(message)) == "ERROR");
            // close socket
            return expectOK("+NSOCL=" + socket) == "OK" && sent;

        }
    }
    return false;
}

/**
 * Save the last generated signature to Calliope mini flash.
 */
void saveSignature() {
    uBit.serial.printf("saving last signature...\r\n");
    PacketBuffer signature = ubirch.getLastSignature();
    uBit.storage.put("s1", signature.getBytes(), 32);
    uBit.storage.put("s2", signature.getBytes() + 32, 32);
}

/**
 * Load the latest signature from flash (after reset).
 */
void loadSignature() {
    KeyValuePair *s1 = uBit.storage.get("s1");
    KeyValuePair *s2 = uBit.storage.get("s2");
    if (s1 && s2) {
        uBit.serial.printf("found last signature...\r\n");
        uint8_t s[64];
        memcpy(s, s1->value, 32);
        memcpy(s + 32, s2->value, 32);
        PacketBuffer signature(s, sizeof(s));
        ubirch.setLastSignature(signature);
    }
    // make sure to free memory
    delete s1;
    delete s2;
}


int main() {
    // necessary to initialize the Calliope mini
    uBit.init();
    uBit.serial.baud(115200);
    
    // set up the serial connection to the NB-IoT modem (BC95)
    uBit.serial.redirect(A_TX, A_RX);
    uBit.serial.baud(9600);

    // printf won't work after the mode has been initialized, use DEBUG(prefix, message)
    LOG("ubirch NB-IoT hackathon template v1.0");

    // initialize, connect and then send a message (json encoded, signed with the ECC key)
    if (initializeModem()) {
        LOG("INITIALIZED");
        if (attach(6)) {
            time_t timestamp;
            LOG("ATTACHED");

            // after startup load the stored last signature
            loadSignature();

            // this will produce the time the Calliope mini is running
            // maybe update the time during startup from the network?
            time(&timestamp);

            // create a new data packet using the ubirch-protocol
            // structure: [ubirch-header..., {"data": {1234: {"t":1234, "l":1234}}}, signature]
            ubirch.startMessage()
                    .addMap(1)                                       // payload is a map with a single entry
                    .addMap("data", 1)                               // contains a single key "data", which is a map
                    .addMap((int) timestamp, 2)                      // data map has key timestamp and 2 entries
                    .addInt("t", uBit.thermometer.getTemperature())  // first entry is the temperature
                    .addInt("l", uBit.display.readLightLevel());     // second is the light level
            ManagedString packet = ubirch.finishMessage();

            if (send(packet, "34.248.246.47", 7070)) {
                LOG("PACKET SENT OK");
                saveSignature();
            } else {
                LOG("FAILED TO SEND");
            }
        }
    }


    LOG("FINISH");
}



