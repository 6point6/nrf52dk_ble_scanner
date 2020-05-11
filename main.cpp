/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

// Platform Libs
#include "mbed.h"
#include "platform/mbed_thread.h"

// C++ libs
#include <string> // for string class
#include <cstdio>

// define the Serial object
Serial pc(USBTX, USBRX);

// Blinking rate in milliseconds
#define BLINKING_RATE_MS 2000

// Serial baud rate
#define PC_SERIAL_BAUD 115200

using namespace std;

// Function converts digital 0 or 1 to "OFF" and "ON"
string convertDigitalReadToString(int status) {
    if (status == 0) {
        return "OFF";
    } else {
        return "ON";
    }
}


int main()
{
    // Initialise the digital pin LED1 as an output
    DigitalOut led(LED1);

    // setup serial connection
    pc.baud(PC_SERIAL_BAUD);

    // Print something over the serial connection
    pc.printf("Commencing blinking.\r\n");

    while (true) {
        led = !led;
        thread_sleep_for(BLINKING_RATE_MS);
        
        // print LED state
        pc.printf("Blink! LED is now %s.\r\n", convertDigitalReadToString(led.read()).c_str());
    }
}