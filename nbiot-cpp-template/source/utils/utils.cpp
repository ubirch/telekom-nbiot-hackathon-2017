/*!
 * @file
 * @brief Utility functions for use during the hackathon.
 *
 * @author Matthias L. Jugel
 * @date   2017-09-11
 *
 * @copyright &copy; 2017 ubirch GmbH (https://ubirch.com)
 *
 * @section LICENSE
 * ```
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * ```
 */

#include <MicroBit.h>
#include <nacl/armnacl.h>
#include "utils.h"


// [0x00|0x01] + 0xCE + [4 byte id] + [0xD9|0xDA] + [1|2 byte length] + [64 byte signature] + [message]
#define PACKET_HEADER_SIZE (1 + 1 + 4 + 3)
#define SIGNATURE_SIZE crypto_sign_BYTES
#define MAX_MESSAGE_SIZE (512 - SIGNATURE_SIZE - 8)

ManagedString sign(ManagedString &message, const unsigned char *signingKey) {
    const unsigned int totalMessageLength = (!signingKey ? 0 : crypto_sign_BYTES) + (unsigned int) message.length();
    const unsigned int packetSize = PACKET_HEADER_SIZE + totalMessageLength;

    // allocate the string data buffer (has a max overhead of 2 byte due to msgpack encoding and terminating \0)
    auto packet = (StringData *) malloc(4 + packetSize + 1);
    // initialize the packet
    packet->init();
    packet->len = (uint16_t) packetSize;

    // position in the msgpack data structure
    uint16_t index = 0;

    // write message header indicating signed or unsigned message
    packet->data[index++] = (uint8_t) ((!signingKey ? 0x00 : 0x01) & 0b0111111);

    // write the device id
    packet->data[index++] = (uint8_t) 0xce;
    // we need to store the device id big endian
    uint32_t _deviceId = __builtin_bswap32(microbit_serial_number());
    memcpy(packet->data + index, (uint8_t *) &_deviceId, 4);
    index += 4;

    // write message and handle different length
    if (totalMessageLength < 256) {
        // long messages have only two byte headers
        packet->data[index++] = (uint8_t) 0xd9;
        packet->data[index++] = (uint8_t) totalMessageLength;
    } else if (totalMessageLength < MAX_MESSAGE_SIZE) {
        // long messages have tree byte headers
        packet->data[index++] = (uint8_t) 0xda;
        uint16_t _signedMessageLength = __builtin_bswap16((uint16_t) totalMessageLength);
        memcpy(packet->data + index, &_signedMessageLength, 2);
        index += 2;
    } else {
        // we have a size limitation
#ifndef NDEBUG
        printf("encoding failed: message too long: %d > %d (max)\r\n", totalMessageLength, MAX_MESSAGE_SIZE);
#endif
        delete packet;
        return ManagedString().leakData();
    }

    if (signingKey) {
        // do the actual signature generation and add it to the packet
        crypto_uint16 smlen;
        if (crypto_sign((unsigned char *) (packet->data + index), &smlen, (const unsigned char *) message.toCharArray(),
                        (crypto_uint16) (message.length()), signingKey)) {
#ifndef NDEBUG
            printf("signing failed\r\n");
#endif
            delete packet;
            return ManagedString();
        }
        printf("%d == %d\r\n", packet->len, index + smlen);
        index += smlen;
    } else {
        memcpy(packet->data + index, message.toCharArray(), (size_t) message.length());
        index += message.length();
    }
    packet->len = index;

    return ManagedString(packet);
}

ManagedString stringToHex(ManagedString &input) {
    const char l[17] = "0123456789ABCDEF";
    ManagedString r;
    for (int i = 0; i < input.length(); i++) {
        char c = input.charAt(i);
        r = r + ManagedString(l[c >> 4]) + ManagedString(l[c & 0x0f]);
    }
    return r;
}

void hexdump(const char *prefix, const uint8_t *b, size_t size) {
    for (unsigned int i = 0; i < size; i += 16) {
        if (prefix && strlen(prefix) > 0) printf("%s %06x: ", prefix, i);
        for (int j = 0; j < 16; j++) {
            if ((i + j) < size) printf("%02x", b[i + j]); else printf("  ");
            if ((j + 1) % 2 == 0) putchar(' ');
        }
        putchar(' ');
        for (int j = 0; j < 16 && (i + j) < size; j++) {
            putchar(b[i + j] >= 0x20 && b[i + j] <= 0x7E ? b[i + j] : '.');
        }
        printf("\r\n");
    }
}

void hexprint(const uint8_t *b, size_t size) {
    for (unsigned int i = 0; i < size; i++) printf("%02x", b[i]);
    printf("\r\n");
}
