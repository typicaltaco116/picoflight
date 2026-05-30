# PicoFlight
PicoFlight is an open-source flight controller project targeting the Raspberry
Pi Pico 2 (RP2350) Microcontroller.

## Dependencies
- The Raspberry Pi Pico SDK [https://github.com/raspberrypi/pico-sdk](https://github.com/raspberrypi/pico-sdk)
- pico-pio-loader [https://github.com/cadouthat/pico-pio-loader/tree/main](https://github.com/cadouthat/pico-pio-loader/tree/main)

## Supported Microcontroller Targets
- Raspberry Pi Pico 2 (RP2350)

The RP2040 may be added in the future but its performance will be significantly
inferior to the RP2350.

## Build Instructions
1. Create a build directory.

```
mkdir build && cd build
```

2. Use CMake to generate platform specific cross compilation build files.

```
cmake ..
```

3. Execute build (compilation platform specific).

```
make -j4
```

4. Upload the compiled UF2 container to the Pico device. This can be achieved 
picotool or by simply holding the BOOTSEL button on the Pico while powering up
to enter mass storage mode and copying over the UF2 container.

```
picotool load picoflight.uf2 -f
```

## Documentation
Check the docs folder for crude project layout documentation and usage
directions
