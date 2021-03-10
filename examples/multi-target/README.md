## Getting Started

### nrf52
[RedBear Nano
2](https://github.com/redbear/nRF5x/tree/master/nRF52832#ble-nano-2) is used for
the example.

1. Download [nRF5 SDK](https://www.nordicsemi.com/Software-and-tools/Software/nRF5-SDK/Download) under `external` directory
2. Set `SDK_ROOT` to the path in the `ports/nrf52/sdk.mk`
3. `make nrf52`

### esp32
[ESP32 DevKitC](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/hw-reference/modules-and-boards-previous.html#esp32-core-board-v2-esp32-devkitc) is used for the example.

1. Follow [the esp-idf installation
   guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/#installation-step-by-step)
2. `make esp32`

## stm32
[Blue Pill](https://stm32-base.org/boards/STM32F103C8T6-Blue-Pill.html) is used
for the example.

1. Download and install
   [STM32CubeMX](https://www.st.com/en/development-tools/stm32cubemx.html#get-software)
2. Run STM32CubeMX and load project `ports/stm32/bluepill/bluepill.ioc`
3. Set *Project Manager* > *Application Structure* as *Advanced*
4. Check *Do not generate the main function*
5. Set *Project Manager* > *Toolchain* as *Makefile*
6. Click *GENERATE CODE*
7. `make bluepill`
