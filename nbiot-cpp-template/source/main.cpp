/**
 * This is a simple template on how to use the BC95 NB-IoT modem in conjunction
 * with the ubirch backend.
 *
 * Not the best code, but I am sure the experts will fix that.
 *
 * @copyright (c) ubirch GmbH.
 * @author Matthias L. Jugel.
 *
 * Licensed under the Apache License 2.0
 */

#include <MicroBit.h>
#include "utils/utils.h"
#include "nacl/armnacl.h"
#include "MicroBitUARTService.h"
#include "MicroBitI2C.h"

// if you want to see AT commands and responses, uncomment this
#define SHOW_AT_COMMANDS

#ifdef SHOW_AT_COMMANDS
#define DEBUGAT(prefix, info) log(prefix, info)
#else
#define DEBUGAT(prefix, info)
#endif

#define LOG(info) log("!!!", info)


#define GPS_TIMEOUT 5000LL
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
static unsigned char vrfyKey[crypto_sign_PUBLICKEYBYTES] = {
0x7b, 0x60, 0x88, 0x2b, 0xee, 0x2e, 0x3f, 0x01, 0x79, 0x07, 0xce, 0x84, 0xe5, 0xe1, 0xc8, 0x7f,
0x70, 0x57, 0x60, 0xfa, 0xc5, 0x87, 0x7f, 0xe0, 0xde, 0x7c, 0x58, 0x06, 0xc4, 0x69, 0x1f, 0x2f,
};
static unsigned char signKey[crypto_sign_SECRETKEYBYTES] = {
0x3b, 0xc4, 0xb2, 0x49, 0x9d, 0x97, 0x50, 0x1a, 0xba, 0x63, 0xb0, 0xf6, 0x30, 0x8f, 0x8d, 0x91,
0x3b, 0xcb, 0xdc, 0xed, 0xa8, 0x53, 0x26, 0x9d, 0x68, 0x75, 0xa0, 0x6a, 0x64, 0x32, 0xfa, 0xc7,
0x7b, 0x60, 0x88, 0x2b, 0xee, 0x2e, 0x3f, 0x01, 0x79, 0x07, 0xce, 0x84, 0xe5, 0xe1, 0xc8, 0x7f,
0x70, 0x57, 0x60, 0xfa, 0xc5, 0x87, 0x7f, 0xe0, 0xde, 0x7c, 0x58, 0x06, 0xc4, 0x69, 0x1f, 0x2f,
};


const char ipAddr[20]  = "52.28.140.176";

MicroBit uBit;

MicroBitUARTService *bleUart;

int connected = 0;

void onConnected(MicroBitEvent e)
{
    uBit.display.scroll("C");
    connected = 1;
}

void onDisconnected(MicroBitEvent e)
{
    uBit.display.scroll("D");
    connected = 0;
    uBit.bleManager.advertise();
}


// log a message to the USB console
// this will switch the serial back and forth between the modem and the USB, not nice
void log(const char *prefix, const char *message) {
    uBit.sleep(20);
    while (uBit.serial.txInUse()) uBit.sleep(10);
    uBit.serial.redirect(USBTX, USBRX);
    uBit.serial.baud(115200);
    uBit.serial.send(ManagedString(prefix) + " " + ManagedString(message) + "\r\n", SYNC_SPINWAIT);
    while (uBit.serial.txInUse()) uBit.sleep(10);
    uBit.serial.redirect(MICROBIT_PIN_P8, MICROBIT_PIN_P2);
    uBit.serial.baud(9600);
    uBit.sleep(20);
}


/*
 * Extract a received UDP message from a string in this format:
 * 0,198.199.120.16,2115,3,4F6921,0 
 * Only length and payload matter for now 
 */
ManagedString extractUDPMessage(ManagedString atResponse) {
    ManagedString payload;
    int commaCounter = 0;
    int i = 0;

    int sizeCommaPos = 0;
    int payloadCommaPos = 0;
    int remainingBytesCommaPos = 0;

    while (i < atResponse.length()) {
        if (atResponse.charAt(i) == ',') {
            
            switch (commaCounter) {
                case 2: {
                    sizeCommaPos = i;
                    break;
                }
                case 3: {
                    payloadCommaPos = i;
                    break;
                }
                case 4: {
                    remainingBytesCommaPos = i;
                    break;
                }
                
            }
            commaCounter++;
        }
        i++;
    }
    
    if (commaCounter == 5) {
        // we got the right amount of commas, let's hope for the best
        payload = atResponse.substring(payloadCommaPos + 1, remainingBytesCommaPos - payloadCommaPos - 1);
    }
    return payload;
}


ManagedString recvUDP() {
    const char* command = "+NSORF=0,20";
    ManagedString msCommand = ManagedString(command);
    DEBUGAT("+++", msCommand.toCharArray());

    ManagedString atcmd = "\rAT" + msCommand + "\r\n";

    uBit.sleep(50);
    uBit.serial.send(atcmd, SYNC_SPINWAIT);

    ManagedString firstResponse;
    ManagedString secondResponse;
    ManagedString s;
    do {
        s = uBit.serial.readUntil("\r\n", MicroBitSerialMode::SYNC_SPINWAIT);
        if (s.length() > 0) {
            DEBUGAT("---", s.toCharArray());
            if (firstResponse.length() == 0) firstResponse = s;
            else {
                if ((firstResponse.length() != 0) && (secondResponse.length() == 0)) {
                    secondResponse = s;
                }
            }
        }
        if (s == "ERROR") return s;
    } while (!(s == "OK"));
    
    // first message should have been, hopefully was +NSONMI:
    // extract the message from secondResponse...

    return extractUDPMessage(secondResponse);
}

// send an AT command and expect OK, returns the first line of the response
ManagedString expectOK(ManagedString command) {
    DEBUGAT("+++", command.toCharArray());

    ManagedString atcmd = "\rAT" + command + "\r\n";

    uBit.sleep(50);
    uBit.serial.send(atcmd, SYNC_SPINWAIT);

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
//    if (!(expectOK("+CGMR") == "V100R100C10B656")) {
//        uBit.display.scroll("BC95 wrong firmware");
//        return false;
//    }
    // setup some basics
    expectOK("+NCONFIG=AUTOCONNECT,TRUE");
    expectOK("+NCONFIG=CR_0354_0338_SCRAMBLING,TRUE");
    expectOK("+NCONFIG=CR_0859_SI_AVOID,TRUE");
    expectOK("+NSOCL=0"); // delete socket # 0 if restarting, error is ok (no socket 0)
    return true;
}

// attach to the NB-IoT network, waits 1s per try
bool attach(int tries) {
    expectOK("+CFUN=1");
    if (expectOK("+COPS=1,2,\"26201\"") == "OK") {
        for (int i = 0; i < tries; i++) {
            if (expectOK("+CGATT?") == "+CGATT:1") return true;
            uBit.sleep(1000);
        }
    }
    return false;
}

// send a message to the backend
bool send(const char* message, uint16_t len, const char *server, int port) {
    if (len > 0 && server && port != 0) {
            log("BRX", stringToHex(message, len).toCharArray());
            // send UDP packet over socket 0
            expectOK((ManagedString)"+NSOST=0," + server + "," + port
                                   + "," + len + "," + stringToHex(message, len));
            return true;
    }
    return false;
}

// send a message to the backend
bool send(ManagedString &message, const char *server, int port) {
    return send(message.toCharArray(), message.length(), server, port);
}


int main() {
    // necessary to initialize the Calliope mini
    uBit.init();
    uBit.serial.baud(115200);
    
    // set up the serial connection to the NB-IoT modem (BC95)
    uBit.serial.redirect(MICROBIT_PIN_P8, MICROBIT_PIN_P2);
    uBit.serial.baud(9600);
    
    
    // listen for Bluetooth connection state changes
    uBit.messageBus.listen(MICROBIT_ID_BLE, MICROBIT_BLE_EVT_CONNECTED, onConnected);
    uBit.messageBus.listen(MICROBIT_ID_BLE, MICROBIT_BLE_EVT_DISCONNECTED, onDisconnected);
    bleUart = new MicroBitUARTService(*uBit.ble, 32, 32); 
        
    uBit.bleManager.advertise();

    // printf won't work after the mode has been initialized, use DEBUG(prefix, message)
    LOG("ubirch NB-IoT hackathon template v1.0");

    // initialize, connect and then send a message (json encoded, signed with the ECC key)
    if (initializeModem()) {
        LOG("INITIALIZED");
        if (attach(6)) {
            LOG("ATTACHED");  
        }
    }

    
    // wait until connected for the first time
    while (!connected) {uBit.sleep(20);};
    
    // open the socket
    // port doesn't seem to matter due to NAT issues, server must use port it received UDP message on
    ManagedString socket = expectOK("+NSOCR=DGRAM,17,44567,1");
    
    // signal our presence.
    const char oioioi[3] = {0x4F, 0x69, 0x21};
    send(oioioi, 3, ipAddr, 2115);
    
    do {
        char uartRx[21];
        uint8_t uartRx_len;
        // Read BLE pipe
        uartRx_len = bleUart->read((uint8_t*)uartRx, 20, ASYNC);
        
        ManagedString nBIoTRx;
        // Read NB-IoT pipe
        nBIoTRx = recvUDP();
        
        // Forward BLE data to NB-IoT
        if (uartRx_len > 0) {
            // No further analysis needed, just forward
            send(uartRx, uartRx_len, ipAddr, 2115);
        }

        // Forward NB-IoT data to BLE
        if (nBIoTRx.length() > 0) {
            log("NRX", nBIoTRx.toCharArray());
            // hex string for data to forward, convert to byte array
            uint8_t buffer[20];
            uint8_t len = hexarrayToByte(buffer, nBIoTRx);
            if (len > 0) {
                bleUart->send(buffer, len, ASYNC);
            }
        }
        
    } while (true);
    
    LOG("FINISH");
}



