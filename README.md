# PicoFlight
PicoFlight is an open-source flight controller project targeting the Raspberry
Pi Pico 2 (RP2350) Microcontroller.

## Dependencies
- The Raspberry Pi Pico SDK [https://github.com/raspberrypi/pico-sdk](https://github.com/raspberrypi/pico-sdk)

## Supported Microcontroller Targets
- Raspberry Pi Pico 2 (RP2350)

The RP2040 may be added in the future but its performance will be significantly
inferior to the RP2350.

## Build Instructions
```
cmake -B build
```
```
cmake --build build -j $(nproc)
```

## Documentation
Check the docs folder for crude project layout documentation and usage
directions
