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
#include "utils.h"

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
