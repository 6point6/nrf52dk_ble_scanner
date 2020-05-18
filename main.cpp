/*
    Structure based on https://github.com/ARMmbed/mbed-os-example-ble/blob/master/BLE_LEDBlinker/source/main.cpp


 */

// Platform Libs
#include <events/mbed_events.h>
#include <mbed.h>

// BLE libs
#include "ble/BLE.h"
#include "ble/gap/AdvertisingDataParser.h"

// C++ libs
#include <cstring>
#include <sstream>
#include <map>

// Constants
#define BLINKING_RATE_MS        500               // LED blinking rate in milliseconds
#define PC_SERIAL_BAUD          115200            // Serial baud rate

/*
    Active scanning sends scan requests to elicit scan responses
 */
#define ACTIVE_SCANNING         true              

/*
    Scan interval is the time it waits on a single advertising channel
 */
#define SCAN_INTERVAL           1000              

/*
    From the API documentation:
    The scanning window divided by the interval determines the duty cycle for scanning. For example, if the interval is 100ms and the window is 10ms, then the controller will scan for 10 percent of the time. It is possible to have the interval and window set to the same value. In this case, scanning is continuous, with a change of scanning frequency once every interval.
 */
#define SCAN_WINDOW             1000


#define ADDR_STRING_LEN         12      // chars in hex string address

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

// Private scanner class variables and methods
private:
    // private global scanner variables
    BLE &_ble;
    events::EventQueue &_event_queue;
    DigitalOut _alive_led;

    // print our address
    void print_local_address()
    {
        /* show what address we are using now */
        Gap::AddressType_t addr_type;
        Gap::Address_t address;
        _ble.gap().getAddress(&addr_type, address);
        printf("Device address: ");
        _pc_serial.printf("Local address: %02x:%02x:%02x:%02x:%02x:%02x.\r\n",
            address[5], address[4], address[3], address[2], address[1], address[0]);
    }
  

    /** Callback triggered when the ble initialization process has finished */
    void on_init_complete(BLE::InitializationCompleteCallbackContext *params) {
        
        // report error or success
        if (params->error != BLE_ERROR_NONE) {
            _pc_serial.printf("Ble initialisation failed.\r\n");
            return;
        }

        _pc_serial.printf("Ble initialisation complete.\r\n");

        // print local address
        print_local_address();

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

    // Convert the provided source address into a char array in dst_address_chars
    int get_address_as_char_array(const ble::address_t src_address, char dst_address_chars[ADDR_STRING_LEN]) {
        for (int i = 0; i < Gap::ADDR_LEN; i++) {
            sprintf(dst_address_chars+(i*2), "%02x", src_address[i]);
        }

        return 1;
    }


    int print_adv_data(ble::AdvertisingDataParser adv_data) {
        // parse the advertising payload
        while (adv_data.hasNext()) {
            ble::AdvertisingDataParser::element_t field = adv_data.next();

            // Print name fields
            if(field.type == ble::adv_data_type_t::COMPLETE_LOCAL_NAME || field.type == ble::adv_data_type_t::SHORTENED_LOCAL_NAME) {
                
                if(field.type == ble::adv_data_type_t::COMPLETE_LOCAL_NAME) {
                    _pc_serial.printf("COMPLETE_LOCAL_NAME: \"");
                }
                else {
                    _pc_serial.printf("SHORTENED_LOCAL_NAME: ");
                }                

                for(int i = 0; i < field.value.size(); i++) {
                    _pc_serial.printf("%c", field.value.data()[i]);
                }

                _pc_serial.printf("\"\r\n");
            }


            if(field.type == ble::adv_data_type_t::MANUFACTURER_SPECIFIC_DATA) {
                _pc_serial.printf("MANUFACTURER_SPECIFIC_DATA: 0x");

                for(int i = 0; i < field.value.size(); i++) {
                    _pc_serial.printf("%02x", field.value.data()[i]);
                }

                _pc_serial.printf("\r\n");
            }

            // Print service data fields
            else if(field.type == ble::adv_data_type_t::SERVICE_DATA || field.type == ble::adv_data_type_t::SERVICE_DATA_128BIT_ID || field.type == ble::adv_data_type_t::SERVICE_DATA_16BIT_ID) {
                if(field.type == ble::adv_data_type_t::SERVICE_DATA)
                    _pc_serial.printf("SERVICE_DATA: 0x");
                else if(field.type == ble::adv_data_type_t::SERVICE_DATA)
                    _pc_serial.printf("SERVICE_DATA: 0x");
                else if(field.type == ble::adv_data_type_t::SERVICE_DATA_128BIT_ID)
                    _pc_serial.printf("SERVICE_DATA_128BIT_ID: 0x");
                else if(field.type == ble::adv_data_type_t::SERVICE_DATA_16BIT_ID)
                    _pc_serial.printf("SERVICE_DATA_16BIT_ID: 0x");
                
                for(int i = 0; i < field.value.size(); i++) {
                    _pc_serial.printf("%02x", field.value.data()[i]);
                }

                _pc_serial.printf("\r\n");
            }

            // print service IDs
            else if(field.type == ble::adv_data_type_t::INCOMPLETE_LIST_16BIT_SERVICE_IDS || field.type == ble::adv_data_type_t::INCOMPLETE_LIST_32BIT_SERVICE_IDS || field.type == ble::adv_data_type_t::INCOMPLETE_LIST_128BIT_SERVICE_IDS || field.type == ble::adv_data_type_t::COMPLETE_LIST_16BIT_SERVICE_IDS || field.type == ble::adv_data_type_t::COMPLETE_LIST_32BIT_SERVICE_IDS || field.type == ble::adv_data_type_t::COMPLETE_LIST_128BIT_SERVICE_IDS) {
                _pc_serial.printf("SERVICE IDs: 0x");
                
                for(int i = 0; i < field.value.size(); i++) {
                    _pc_serial.printf("%02x", field.value.data()[i]);
                }

                _pc_serial.printf("\r\n");
            }
        }

        _pc_serial.printf("\r\n");

        return 1;
    }

    
    // Called on receipt of an advertising report
    void onAdvertisingReport(const ble::AdvertisingReportEvent &event) {
        
        // get the address and RSSI from the event
        const ble::address_t address = event.getPeerAddress();
        const ble::rssi_t rssi = event.getRssi();

        _pc_serial.printf("Received advertising data from address %02x:%02x:%02x:%02x:%02x:%02x, RSSI: %ddBm\r\n",
            address[5], address[4], address[3], address[2], address[1], address[0], rssi);
        
        // convert to chars
        char address_string[Gap::ADDR_LEN] = {00};
        get_address_as_char_array(address, address_string);

        //_pc_serial.printf("Address string: %s\r\n", address_string);

        // TODO unique the devices

        // print advertising data
        ble::AdvertisingDataParser adv_data(event.getPayload());
        print_adv_data(adv_data);
    }    
}; /*  /Inner scanner class */


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

    // Run the scanner
    _pc_serial.printf("Setup done. Running.\r\n");
    scanner.scan();
 
    return 0;
}