#!/bin/sh

cwd=$(pwd)

export IDF_PATH=$cwd/external/esp-idf
export IDF_TOOLS_PATH=$HOME/.local/espressif
export IDF_PYTHON_ENV_PATH=$IDF_TOOLS_PATH/python_env/idf4.4_py3.8_env

export PATH=\
$IDF_TOOLS_PATH/tools/xtensa-esp32-elf/esp-2020r3-8.4.0/xtensa-esp32-elf/bin:\
$IDF_TOOLS_PATH/tools/openocd-esp32/v0.10.0-esp32-20200709/openocd-esp32/bin:\
$IDF_TOOLS_PATH/python_env/idf4.3_py3.8_env/bin:\
$PATH

export ESPPORT=/dev/tty.SLAB_USBtoUART
