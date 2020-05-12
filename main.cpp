/*
    Structure based on https://github.com/ARMmbed/mbed-os-example-ble/blob/master/BLE_LEDBlinker/source/main.cpp


 */

// Platform Libs
#include <events/mbed_events.h>
#include <mbed.h>
#include "ble/BLE.h"

// C++ language libs
#include <string> // for string class

// Local libs
#include "string_helpers.h"

// Constants
#define BLINKING_RATE_MS        500               // Blinking rate in milliseconds
#define PC_SERIAL_BAUD          115200            // Serial baud rate
#define ACTIVE_SCANNING         true              // Active scanning sends scan requests
#define SCAN_INTERVAL           1800              // Scan interval is the time it waits on a single channel
#define SCAN_WINDOW             1500              // Scan window?


// Global Objects
static EventQueue event_queue(/* event count */ 16 * EVENTS_EVENT_SIZE);
Serial _pc_serial(USBTX, USBRX);            // define the Serial object

/*
    Inner scanner class
 */
class BLEScanner : ble::Gap::EventHandler {
public:
    // constructor
    BLEScanner(BLE &ble, events::EventQueue &event_queue) :
        _ble(ble),
        _event_queue(event_queue),
        _alive_led(LED1, 1) { }

    // destructor
    ~BLEScanner() { }

    // scan method 
    void scan() {
        // sets the instance of the scanner class as the event handler
        _ble.gap().setEventHandler(this);

        // Initialise the BLE stack, start scanning if successful
        _ble.init(this, &BLEScanner::on_init_complete);

        // add the LED blinker as a recurring event on the event queue
        _event_queue.call_every(BLINKING_RATE_MS, this, &BLEScanner::blink);

        // run infinitely
        _event_queue.dispatch_forever();
    }

private:
    /** Callback triggered when the ble initialization process has finished */
    void on_init_complete(BLE::InitializationCompleteCallbackContext *params) {
        
        // report error or success
        if (params->error != BLE_ERROR_NONE) {
            _pc_serial.printf("Ble initialization failed.");
            return;
        }

        _pc_serial.printf("Ble initialization complete.");

        // setup scan with custom parameters
        ble::ScanParameters scan_params;
        scan_params.set1mPhyConfiguration(ble::scan_interval_t(SCAN_INTERVAL), ble::scan_window_t(SCAN_WINDOW), ACTIVE_SCANNING);
        _ble.gap().setScanParameters(scan_params);
        
        // start scanning
        _ble.gap().startScan();
    }

    // Blink the alive LED
    void blink() {
        _alive_led = !_alive_led;
    }

// Private scanner class methods
private:
    // Called on receipt of an advertising report
    void onAdvertisingReport(const ble::AdvertisingReportEvent &event) {
        
        // get the address and RSSI from the event
        ble::address_t address = event.getPeerAddress();
        ble::rssi_t rssi = event.getRssi();
        
        //ble::AdvertisingDataParser adv_data(event.getPayload());

        _pc_serial.printf("Received advertising data from address %02x:%02x:%02x:%02x:%02x:%02x.\r\n",
            address[5], address[4], address[3], address[2], address[1], address[0]);
    }

// private global scanner variables
private:
    BLE &_ble;
    events::EventQueue &_event_queue;
    DigitalOut _alive_led;
};

/** Schedule processing of events from the BLE middleware in the event queue. */
void schedule_ble_events(BLE::OnEventsToProcessCallbackContext *context) {
    event_queue.call(Callback<void()>(&context->ble, &BLE::processEvents));
}

// Main method sets up peripherals, runs infinite loop
int main()
{   
    // setup serial connection
    _pc_serial.baud(PC_SERIAL_BAUD);

    // create BLE instance
    BLE &ble = BLE::Instance();

    // attach the callback to the event queue
    ble.onEventsToProcess(schedule_ble_events);

    // Setup scanner instance
    BLEScanner scanner(ble, event_queue);

    // Run
    _pc_serial.printf("Setup done. Running.\r\n");
    scanner.scan();
 
    return 0;
}