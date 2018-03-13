/*!
 * @file
 * @brief Small helpful utility functions.
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
#ifndef NBIOT_CPP_TEMPLATE_UTILS_H
#define NBIOT_CPP_TEMPLATE_UTILS_H

#include <cstddef>
#include <stdint.h>

/**
 * Convert the input into a hex encoded string.
 * @param input the input
 * @return the hex encoded string
 */
ManagedString stringToHex(ManagedString &input);

/**
 * Dump an array in hex-dump style with a prefix.
 * @param prefix the prefix prepended to each line
 * @param b the byte array to dump
 * @param size the size of the byte array
 */
void hexdump(const char *prefix, const uint8_t *b, size_t size);

/**
 * Print the byte array as hex encoded string.
 * @param b the byte array to print
 * @param size the size of the byte array
 */
void hexprint(const uint8_t  *b, size_t size);

#endif //NBIOT_CPP_TEMPLATE_UTILS_H
