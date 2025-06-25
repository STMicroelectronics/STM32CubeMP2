# LOW_POWER_SRAM Build

In this example, one EXTI line (EXTI2_8) is configured to generate
an interrupt on each falling edge.
In the interrupt routine a led connected to a specific GPIO pin is toggled.

In this example:
    - EXTI2_8 is connected to PG8 pin
      - when falling edge is detected on EXTI2_8 by pressing User PG8 push-button, LED3 toggles once
      - LED3 is ON in case of error in initialization

On STM32MP257F-EV1:
    - EXTI2_8 is connected to User PG8 push-button
	
For compilation please do following:
- export path of any arm cross compiler toolchain with `arm-none-eabi-gcc` to your PCs PATH environment Variable.
	- e.g. for STMCubeIDEs D3 Toolchain: C:\ST\STM32CubeIDE_1.12.0.23_MP2-D3\STM32CubeIDE\plugins\com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.10.3-2021.10.win32_1.0.200.202301161003\tools\bin

To build:
- cmake -G "Unix Makefiles" -B build
- make -C build all

Generate Signed Binary:
- cd build && python ../../../../../Utilities/optee_os/scripts/sign_rproc_fw.py --in LowPower_SRAM_Demo_CM33_NonSecure_stripped.elf --out LowPower_SRAM_Demo_CM33_NonSecure_sign.bin --key ../../../../../Utilities/optee_os/keys/default.pem --plat-tlv BOOTADDR 0xA061000 && cd ..

Output:
- Elf Binary: `LowPower_SRAM_Demo_CM33_NonSecure.elf` in build folder
- Signed Binary: `LowPower_SRAM_Demo_CM33_NonSecure_sign.bin` in build folder

Following commands should be used in Linux console to run the example :
	cd /usr/local/Cube-M33-examples/STM32MP257F-EV1/Demonstrations/LowPower_SRAM_Demo/
	./fw_cortex_m33.sh start
	stty -onlcr -echo -F /dev/ttyRPMSG0
	cat /dev/ttyRPMSG0 &

Usecase 1: RUN1->RUN2->STOP1->RUN2
		a) Initially system is in RUN1(Both Orange(M33) and Blue LED blinking(A35))
		b) Run below command to Execute power mode sequence:
			echo "*delayedstop5" > /dev/ttyRPMSG0
			echo s2idle > /sys/power/mem_sleep
			echo enabled > $(find /sys/bus/platform/devices/soc*/ -name usb1)/power/wakeup
			echo +15 > /sys/class/rtc/rtc0/wakealarm; systemctl suspend
			======> RUN2(Blue LED stop blinking, Orange LED stops blinking and turns on)
			======> STOP1(Orange LED turns off after 5 seconds)
		c) Press User2 button to got back to Run2
			======> RUN2(Orange LED start blnking on pressing User2 btton before 15 seconds)

Usecase 2: RUN1->STOP1->RUN2
		a) Initially system is in RUN1(Both Orange(M33) and Blue LED blinking(A35))
		b) Run below command to enter STOP1
			echo "*stop" > /dev/ttyRPMSG0
			echo s2idle > /sys/power/mem_sleep
			echo enabled > $(find /sys/bus/platform/devices/soc*/ -name usb1)/power/wakeup
			echo +15 > /sys/class/rtc/rtc0/wakealarm; systemctl suspend
			======> STOP1(Both Blue and Orange LED stop blinking)
		c) Press User2 button to got back to Run2
			======> RUN2(Orange LED start blnking on pressing User2 button before 15 seconds)

Usecase 3: RUN1->STOP1->RUN1
		a) Initially system is in RUN1(Both Orange(M33) and Blue LED blinking(A35))
		b) Run below command to enter STOP1 mode and come back to RUN1 after 15 secs.
			echo "*stop" > /dev/ttyRPMSG0
			echo s2idle > /sys/power/mem_sleep
			echo enabled > $(find /sys/bus/platform/devices/soc*/ -name usb1)/power/wakeup
			echo +15 > /sys/class/rtc/rtc0/wakealarm; systemctl suspend
			======> STOP1(Both Blue and Orange LED stop blinking for 15 sec)
			echo "Hi! from cm33" > /dev/ttyRPMSG0
			======> RUN1(After 15 seconds Both Blue and Orange LED start blinking)

Usecase 4: RUN1->LPSTOP1->RUN1
		a) Initially system is in RUN1(Both Orange(M33) and Blue LED blinking(A35))
		b) Run below commands to enter LPSTOP1 mode and come back to RUN1 after 15 secs.
			echo "*lpstop" > /dev/ttyRPMSG0
			echo s2idle > /sys/power/mem_sleep
			echo +15 > /sys/class/rtc/rtc0/wakealarm; systemctl suspend
		======> LPSTOP1(Both Blue and Orange LED stop blinking for 15 sec)
			echo "Hi! from cm33" > /dev/ttyRPMSG0
		======> RUN1(After 15 seconds Both Blue and Orange LED start blinking)

Now, Disable the USB wakeup by using command -
			echo disabled > $(find /sys/bus/platform/devices/soc*/ -name usb1)/power/wakeup

Usecase 5: RUN1->STOP2->RUN1
		a) Initially system is in RUN1(Both Orange(M33) and Blue LED blinking(A35))
		b) Run below command to enter STOP2 mode and come back to RUN1 after 15 secs.
			echo "*stop" > /dev/ttyRPMSG0
			echo deep > /sys/power/mem_sleep
			echo enabled > $(find /sys/bus/platform/devices/soc*/ -name usb1)/power/wakeup
			echo +15 > /sys/class/rtc/rtc0/wakealarm; systemctl suspend
			======> STOP2(Both Blue and Orange LED stop blinking for 15 sec)
			echo "Hi! from cm33" > /dev/ttyRPMSG0
			======> RUN1(After 15 seconds Both Blue and Orange LED start blinking)

Usecase 6: RUN1->RUN2->STOP2->RUN2
		a) Initially system is in RUN1(Both Orange(M33) and Blue LED blinking(A35))
		b) Run below command to Execute power mode sequence:
			echo "*delayedstop5" > /dev/ttyRPMSG0
			echo deep > /sys/power/mem_sleep
			echo +15 > /sys/class/rtc/rtc0/wakealarm; systemctl suspend
		======> RUN2(Blue LED stops blinking, Orange LED stops blinking and remains in on state)
		======> STOP2(Orange LED turns off after 5 seconds)
		c) Press User2 button to got back to Run2
		======> RUN2(Orange LED starts blnking on pressing User2 button before 15 seconds)

Usecase 7: RUN1->LPSTOP2->RUN2
		a) Initially system is in RUN1(Both Orange(M33) and Blue LED blinking(A35))
		b) Run below commands to enter LPSTOP2
			echo "*lpstop" > /dev/ttyRPMSG0
			echo deep > /sys/power/mem_sleep
			echo +15 > /sys/class/rtc/rtc0/wakealarm; systemctl suspend
		======> LPSTOP2(Both Blue and Orange LED stop blinking)
		c) Press User2 button to got back to Run2
		======> RUN2(Orange LED starts blnking on pressing User2 button before 15 seconds)

Usecase 8: RUN1->LPSTOP2->RUN1
		a) Initially system is in RUN1(Both Orange(M33) and Blue LED blinking(A35))
		b) Run below commands to enter LPSTOP2 mode and come back to RUN1 after 15 secs.
			echo "*lpstop" > /dev/ttyRPMSG0
			echo deep > /sys/power/mem_sleep
			echo +15 > /sys/class/rtc/rtc0/wakealarm; systemctl suspend
		======> LPSTOP2(Both Blue and Orange LED stop blinking for 15 sec)
			echo "Hi! from cm33" > /dev/ttyRPMSG0
		======> RUN1(After 15 seconds Both Blue and Orange LED start blinking)
