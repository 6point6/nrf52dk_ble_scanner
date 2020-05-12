/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

// Platform Libs
#include <events/mbed_events.h>
#include <mbed.h>
#include "ble/BLE.h"

// C++ language libs
#include <string> // for string class
#include <cstdio>
#include <map>

// Local libs
#include "string_helpers.h"

// Constants
#define BLINKING_RATE_MS 500               // Blinking rate in milliseconds
#define PC_SERIAL_BAUD 115200               // Serial baud rate

static const int URI_MAX_LENGTH = 18;       // Maximum size of service data in ADV packets

using namespace std;

// Global Objects
Serial _pc_serial(USBTX, USBRX);            // define the Serial object
DigitalOut _led1(LED1);              // Initialise the digital pin LED1 as an output
static EventQueue _eventQueue(/* event count */ 16 * EVENTS_EVENT_SIZE);


/* Do blinky on LED1 while we're waiting for BLE events. Called every BLINKING_RATE_MS */
void periodicCallback(void)
{
    _led1 = !_led1;
}


/*
 * This function is called every time we receive an advertisement.
 */
void advertisementCallback(const Gap::AdvertisementCallbackParams_t *params)
{
    _pc_serial.printf("Address: %02x:%02x:%02x:%02x:%02x:%02x\t\r\n",
           params->peerAddr[5], params->peerAddr[4], params->peerAddr[3], params->peerAddr[2], params->peerAddr[1], params->peerAddr[0]);
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
    
    /*
        DEPRECATED WAY OF STARTING A SCAN
     */
    // TODO replace with new version
    ble.gap().setScanParams(1800 /* scan interval */, 1500 /* scan window */, true /* active scanning */);
    
    // TODO replace with new version
    ble.gap().startScan(advertisementCallback);
}
 
 /*
    TODO comment
  */
void scheduleBleEventsProcessing(BLE::OnEventsToProcessCallbackContext* context) {
    //BLE &ble = BLE::Instance();
    _eventQueue.call(Callback<void()>(&context->ble, &BLE::processEvents));
}

// Main method sets up peripherals, runs infinite loop
int main()
{   
    // setup serial connection
    _pc_serial.baud(PC_SERIAL_BAUD);

    // Print something over the serial connection
    _pc_serial.printf("Serial link setup.\r\n");

    // setup BLE object
    _eventQueue.call_every(BLINKING_RATE_MS, periodicCallback);          // add LED blinking to the events queue
    
    BLE &ble = BLE::Instance();                             // instantiate the BLE object
    ble.onEventsToProcess(scheduleBleEventsProcessing);
    ble.init(bleInitComplete);

    // Run
    _pc_serial.printf("Setup done. Running.\r\n");
    _eventQueue.dispatch_forever();
 
    return 0;
}