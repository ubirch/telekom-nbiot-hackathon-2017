/*!
 * @file
 * @brief Generate random numbers using the nRF51 hardware RNG
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
#include <stdint.h>
#include <nrf_sdm.h>

/**
 * Get a single random number from the rng (this function is a copy of what is found in the microbit-dal)
 *
 * @return a single random number byte
 */
char getRandomNumber() {
    static uint32_t random_value = 0;
    uint8_t softdevice_enabled;

    sd_softdevice_is_enabled(&softdevice_enabled);

    if (softdevice_enabled == 1) {
        // If Bluetooth is enabled, we need to go through the Nordic software to safely do this.
        uint32_t result = sd_rand_application_vector_get((uint8_t *) &random_value, sizeof(random_value));

        // If we couldn't get the random bytes then at least make the seed non-zero.
        if (result != NRF_SUCCESS)
            random_value = 0xBBC5EED;
    } else {
        // Othwerwise we can access the hardware RNG directly.

        // Start the Random number generator. No need to leave it running... I hope. :-)
        NRF_RNG->TASKS_START = 1;

        for (int i = 0; i < 4; i++) {
            // Clear the VALRDY EVENT
            NRF_RNG->EVENTS_VALRDY = 0;

            // Wait for a number ot be generated.
            while (NRF_RNG->EVENTS_VALRDY == 0);

            random_value = (random_value << 8) | ((int) NRF_RNG->VALUE);
        }

        // Disable the generator to save power.
        NRF_RNG->TASKS_STOP = 1;
    }
    return (char) random_value;
}

void randombytes(unsigned char *x, unsigned long long xlen) {
    while (xlen > 0) {
        *x = (unsigned char) getRandomNumber();
        x++;
        xlen--;
    }
}
