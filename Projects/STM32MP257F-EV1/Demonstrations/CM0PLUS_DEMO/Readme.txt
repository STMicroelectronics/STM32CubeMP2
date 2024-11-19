====  CM0PLUS Demo application ====

The main use case is
- Run CM0PLUS applications and check logs traces on LPUART1

STM32CubeIDE Build configs
- CM0plus application with Console on LPUART1
    - CA35TDCID_m0plus      : non signed firmware
    - CA35TDCID_m0plus_sign : signed firmware

STM32CubeIDE Debug configuration specifics
- M0Plus debug configuration
    Create debug configuration on CM0Plus application
  1) Debug configuration->Startup->Initialization Commands
     Ensure "mon reset" and "mon halt" are present
     For Semihosting configurations, add "mon arm semihosting enable"
  2) If you want to use MP2 Validation OpenOCD scripts, you shall select the .cfg file to use in you debug configuration
	 Ensure you have launched STM32CubeIDE with STM32CUBEIDE_RESOURCES environment variable pointing to <mp2_master>\Firmware\Utilities\STM32CubeIDE_Patches\MP2_Patch\STM32CubeIDE
  3) Enable LPUART predefined configuration for CM0PLUS (as CM0PLUS has no access to RCC so need to configure in from OpenOCD)
	 OpenOCD Options: -bc "set mp2_preset_file MP2_PRESET_LPUART1_M0P.cfg"

