```shell
$ git submodule add https://github.com/ARM-software/CMSIS_5.git external/CMSIS_5
$ git submodule add https://github.com/STMicroelectronics/cmsis_device_f1.git external/cmsis_device_f1
$ git submodule add https://github.com/STMicroelectronics/stm32f1xx_hal_driver.git external/stm32f1xx_hal_driver
$ cat sources.mk
include ports/stm32/bluepill/target.mk
[...]
```
