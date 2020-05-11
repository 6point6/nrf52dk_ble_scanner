/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mbed.h"
#include "platform/mbed_thread.h"

// define the Serial object
Serial pc(USBTX, USBRX);

// Blinking rate in milliseconds
#define BLINKING_RATE_MS 2000

// Serial baud rate
#define PC_SERIAL_BAUD 115200

int main()
{
    // Initialise the digital pin LED1 as an output
    DigitalOut led(LED1);

    // setup serial connection
    pc.baud(PC_SERIAL_BAUD);

    // Print something over the serial connection
    pc.printf("Blink! LED is now %d\r\n", led.read());

    while (true) {
        led = !led;
        thread_sleep_for(BLINKING_RATE_MS);
        pc.printf("Blink!\r\n");
    }
}
