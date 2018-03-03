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
#include <ubirch/ubirch_protocol.h>
#include <ubirch/ubirch_ed25519.h>
#include <ubirch/digest/sha512.h>
#include "utils.h"

static unsigned char prev_sig[UBIRCH_PROTOCOL_SIGN_SIZE] = {};

inline int msgpack_string_write(void *data, const char *buf, size_t len) {
    msgpack_sbuffer *sbuf = (msgpack_sbuffer *) data;
    
    // if we have data in sbuf, use the stack to exchange and increase the buffer
    // this is neccessary to avoid heap fragmentation
    char tmp[sbuf->size] = {};
    if (sbuf->data) {
        memcpy(tmp, sbuf->data, sbuf->size);
        free(sbuf->data);
    }

    sbuf->data = (char *) malloc(sbuf->size + len);
    memcpy(sbuf->data, tmp, sbuf->size);
    memcpy(sbuf->data + sbuf->size, buf, len);
    sbuf->size += len;

    return 0;
}

PacketBuffer sign(ManagedString message) {
    uint32_t _deviceId = __builtin_bswap32(microbit_serial_number());
    unsigned char hardwareSerial[UBIRCH_PROTOCOL_UUID_SIZE] = {};
    for (int i = 0; i < 4; i++) memcpy(hardwareSerial + 4 * i, &_deviceId, 4);

    // initialize ubirch protocol
    msgpack_sbuffer sbuf = {};
    ubirch_protocol proto = {};
    ubirch_protocol_init(&proto, proto_chained, 0x00,
                         &sbuf, msgpack_string_write, ed25519_sign, hardwareSerial);
    memcpy(proto.signature, prev_sig, UBIRCH_PROTOCOL_SIGN_SIZE);

    // create a packer for ubirch protocol
    msgpack_packer pk = {};
    msgpack_packer_init(&pk, &proto, ubirch_protocol_write);

    ubirch_protocol_start(&proto, &pk);
    msgpack_pack_raw(&pk, static_cast<size_t>(message.length()));
    msgpack_pack_raw_body(&pk, message.toCharArray(), static_cast<size_t>(message.length()));
    ubirch_protocol_finish(&proto, &pk);

    // copy the last signature into our local buffer
    memcpy(prev_sig, proto.signature, UBIRCH_PROTOCOL_SIGN_SIZE);

    // hexdump("PKT", reinterpret_cast<const uint8_t *>(sbuf.data), sbuf.size);

    // create a packetbuffer from the buffer and free sbuf
    PacketBuffer buf(reinterpret_cast<uint8_t *>(sbuf.data), sbuf.size);
    free(sbuf.data);

    return buf;
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
