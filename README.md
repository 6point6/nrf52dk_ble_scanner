# Description
An example C++ application for the Nordic NRF52 Development Kit, built and deployed with Mbed Studio. It listens for BLE advertising packets, and when received prints their details over the PC serial connection.

Uses the OS5 version of all the Mbed libraries. **Caution: Many of the example projects found on the Mbed pages use previous versions**. The [up to date libraries are here](https://os.mbed.com/teams/mbed-os-examples).

Requires the board to be running the [ARM DapLink bootloader](https://armmbed.github.io/DAPLink/?board=Nordic-nRF52-DK).

# Starting project
I started with the [mbed-os-example-blinky project](https://os.mbed.com/teams/mbed-os-examples/code/mbed-os-example-blinky/), which is one of the example projects in Mbed Studio. Then copied [this main class](https://os.mbed.com/teams/mbed-os-examples/code/mbed-os-example-ble-EddystoneObserver//file/092c08942a29/source/main.cpp/).
# Libraries
* [Mbed nRF51822 library](https://os.mbed.com/teams/Nordic-Semiconductor/code/nRF51822/)
* [BLE API library](https://os.mbed.com/teams/Bluetooth-Low-Energy/code/BLE_API/)

# Running it
Build and flash it to the board, and it should start running. Connect to it over serial (115200 baud rate, and you should see something like this:)
```
Received advertising data from address 74:03:0c:d6:5b:3f, RSSI: -90dBm
MANUFACTURER_SPECIFIC_DATA: 0x060001092002423ba51b2d02f260eefd8ad3912ead7167a62ccaae1cda

Received advertising data from address 00:7c:2d:e7:eb:31, RSSI: -72dBm
MANUFACTURER_SPECIFIC_DATA: 0x75004204034012170501007c2de7eb310000027c2de7eb300001

Received advertising data from address 53:4f:01:39:8b:89, RSSI: -83dBm
SERVICE IDs: 0x9ffe
SERVICE_DATA: 0x9ffe0000000000000000000000000000000000000000

Received advertising data from address 00:7c:2d:e7:eb:31, RSSI: -72dBm
MANUFACTURER_SPECIFIC_DATA: 0x75004204034012170501007c2de7eb310000027c2de7eb300001

Received advertising data from address 00:7c:2d:e7:eb:31, RSSI: -72dBm
MANUFACTURER_SPECIFIC_DATA: 0x75004204034012170501007c2de7eb310000027c2de7eb300001
```
