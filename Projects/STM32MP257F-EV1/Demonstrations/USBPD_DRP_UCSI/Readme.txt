# USBPD_DRP_UCSI Build

For compilation please do following:
- export path of any arm cross compiler toolchain with `arm-none-eabi-gcc` to your PCs PATH environment Variable.
	- e.g. for STMCubeIDEs D3 Toolchain: C:\ST\STM32CubeIDE_1.12.0.23_MP2-D3\STM32CubeIDE\plugins\com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.10.3-2021.10.win32_1.0.200.202301161003\tools\bin


To build:
- cmake -G"Unix Makefiles" -B build
- make -C build all

Generate Signed Binary:
- cd build && python ../../../../../Utilities/optee_os/scripts/sign_rproc_fw.py --in USBPD_DRP_UCSI_CM33_NonSecure_stripped.elf --out USBPD_DRP_UCSI_CM33_NonSecure_sign.bin --key ../../../../../Utilities/optee_os/keys/default.pem --plat-tlv BOOTADDR 0x80100000 && cd ..

Output:
- Elf Binary: `USBPD_DRP_UCSI_CM33_NonSecure.elf` in build folder



