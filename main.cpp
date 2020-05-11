/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

// Platform Libs
#include <events/mbed_events.h>
#include <mbed.h>
#include "ble/BLE.h"
//#include "pretty_printer.h"

//#include "gap/Gap.h"
//#include "gap/AdvertisingDataParser.h"
//#include "platform/mbed_thread.h"

// C++ libs
#include <string> // for string class
#include <cstdio>

// Local libs
#include "string_helpers.h"

// Constants
#define BLINKING_RATE_MS 2000               // Blinking rate in milliseconds
#define PC_SERIAL_BAUD 115200               // Serial baud rate

static const int URI_MAX_LENGTH = 18;       // Maximum size of service data in ADV packets

using namespace std;

// Global Objects
Serial _pc_serial(USBTX, USBRX);            // define the Serial object
DigitalOut _led1(LED1);              // Initialise the digital pin LED1 as an output
static EventQueue _eventQueue(/* event count */ 16 * EVENTS_EVENT_SIZE);

/* Do blinky on LED1 while we're waiting for BLE events */
void periodicCallback(void)
{
    _led1 = !_led1;

    // print LED state
    //_pc_serial.printf("Blink! LED is now %s.\r\n", convertDigitalReadToString(_led1.read()).c_str());
}

/*
 * This function is called every time we scan an advertisement.
 */
void advertisementCallback(const Gap::AdvertisementCallbackParams_t *params)
{
    _pc_serial.printf("Advertising data collected:\r\n");

    _pc_serial.printf("Address: %02x:%02x:%02x:%02x:%02x:%02x\t",
           params->peerAddr[5], params->peerAddr[4], params->peerAddr[3], params->peerAddr[2], params->peerAddr[1], params->peerAddr[0]);

    _pc_serial.printf("Data length: %d\t", params->advertisingDataLen);
    _pc_serial.printf("RSSI: %ddBm\r\n", params->rssi);
}

/*
    TODO comment
  */
void onBleInitError(BLE& ble, ble_error_t error)
{
   _pc_serial.printf("Error intitialising BLE object:\r\n%u\r\n", error);
}

/*
    TODO comment
  */ 
void bleInitComplete(BLE::InitializationCompleteCallbackContext *params)
{
    BLE&        ble   = params->ble;
    ble_error_t error = params->error;
 
    if (error != BLE_ERROR_NONE) {
        onBleInitError(ble, error);
        return;
    }
 
    if (ble.getInstanceID() != BLE::DEFAULT_INSTANCE) {
        return;
    }
    

    // TODO replace with new version
    ble.gap().setScanParams(1800 /* scan interval */, 1500 /* scan window */, true /* active scanning */);
    
    // TODO replace with new version
    ble.gap().startScan(advertisementCallback);
}
 
 /*
    TODO comment
  */
void scheduleBleEventsProcessing(BLE::OnEventsToProcessCallbackContext* context) {
    BLE &ble = BLE::Instance();
    _eventQueue.call(Callback<void()>(&ble, &BLE::processEvents));
}

// Main method sets up peripherals, runs infinite loop
int main()
{   
    // setup serial connection
    _pc_serial.baud(PC_SERIAL_BAUD);

    // Print something over the serial connection
    _pc_serial.printf("Commencing blinking at rate of once per %dms.\r\n", BLINKING_RATE_MS);

    // setup BLE object
    _eventQueue.call_every(500, periodicCallback);          // add LED blinking to the events queue
    BLE &ble = BLE::Instance();                             // instantiate the BLE object
    ble.onEventsToProcess(scheduleBleEventsProcessing);
    ble.init(bleInitComplete);

    // Run
    _pc_serial.printf("Setup done. Running.\r\n");
    _eventQueue.dispatch_forever();
 
    return 0;
}