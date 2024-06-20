/**
  @page LTDC_Display_3rdLayer LTDC Display 3 layer example

  @verbatim
  ******************************************************************************
  * @file    LTDC/LTDC_Display_3rdLayer/readme.txt
  * @author  MCD Application Team
  * @brief   Description of the LTDC Display 3rd layer example.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  @endverbatim

@par Example Description

  How to configure the LTDC peripheral to display 3rd layer. 
  The goal of this example is to demonstrate CM33 controlled 3rd Layer non-protected while
  displaying standard HMI on linux side.   
  
  Steps to configure the 3rd Layer of LTDC :
   + Read back common registers to determine Panel info (height and width) 
   + Configure the 3rd Layer with noReload trigger
          -- Window Sizing        (Full Screen and Partial Screen Mode) controlled by "FULLSCREEN" Preprocessor 
          -- Opacity              (controlled by constA : set in such a way HMI will be visible)
          -- Input Pixel Format   Input Pixel Format will be RGB565
   + set Immediate reload type (reload)
  
  3rd Layer Display Content:
  - Splash screen will be displayed with content (Example description , and ST logo)
  - Splash screen will be disappear after 20 second timeout (SUCCESS)  
                   
  After LCD initialization, the LCD layer 3 is configured as following :
  - FULLSCREEN Mode (with "FULLSCREEN" Preprocessor) layer 3 is configured to display a framebuffer content from DDR with direct
    color (RGB565) as pixel format and 1024x600 px size.
  - Partial Screen Mode (with no "FULLSCREEN" Preprocessor) Layer 3 is configured to display a framebuffer content from DDR with direct
    color (RGB565) as pixel format and 720x480 px size.

  Example : Display composition 
						+-------------------+
						|    Layer 3        |
						|  (Top Layer)      | ------------------------> CM33 Controlled layer (Used to Demostrate 3rd layer Nonsecure Cube example by M33 )
						+-------------------+
						|    Layer 2        |
						|                   | -------------+
						+-------------------+              |----------> CA35 Controlled Layers (Used to display Standard HMI content by OSTL)
						|    Layer 1        | -------------+
						|  (Foreground)     |
						+-------------------+

  RIF configuration: Required modification in RIF settings on external DT in OPTEE as follows:
    
      - "ltdc_m33_layer" region of 4M size has been used for framebuffer (fb_address: 0x81300000 of 4MB) 
  
      - To be able to write via M33 and read via LTDC axi DMA engine in NSEC state to this framebuffer region,
        CID2 and CID4 are listed into read and write list of RISAF region 12  
        
      - LTDC_L2_ID resource under RIFSC has been statically allocated to CID2 and set to NSecure access 
    
        Refer below file code: 
        @"external-dt/optee/stm32mp257f-ev1-ca35tdcid-ostl-m33-examples-resmem.dtsi"
        +  ltdc_m33_layer: ltdc-m33-layer@81300000 {
        +         reg = <0x0 0x81300000 0x0 0x400000>;
        +         no-map;
        +  };
        
        @"external-dt/optee/stm32mp257f-ev1-ca35tdcid-ostl-m33-examples-rif.dtsi" 
        +   RIFPROT(STM32MP25_RIFSC_LTDC_L2_ID, RIF_UNUSED, RIF_UNLOCK, RIF_NSEC, RIF_PRIV, RIF_CID2, RIF_SEM_DIS, RIF_CFEN)
        
        +   &ltdc_m33_layer {
        +      st,protreg = <RISAFPROT(RISAF_REG_ID(12), RIF_CID2_BF|RIF_CID4_BF, RIF_CID2_BF|RIF_CID4_BF, RIF_UNUSED, RIF_NSEC, RIF_ENC_DIS, RIF_BREN_EN)>;
        +   };
        
        +   &risaf4 {
        +      memory-region= <&tfm_code>, <&cm33_cube_fw>, <&tfm_data>, <&cm33_cube_data>, <&ipc_shmem>, <&spare1>, <&bl31_context>, <&op_tee>, <&linuxkernel1>, <&gpu_reserved>, <&ltdc_m33_layer>, <&ltdc_sec_layer>, <&ltdc_sec_rotation>, <&linuxkernel2>;
        +   };
        
        @"external-dt/linux/stm32mp257f-ev1-ca35tdcid-ostl-m33-examples-resmem.dtsi"                 
        +  ltdc_m33_layer: ltdc-m33-layer@81300000 {
        +         reg = <0x0 0x81300000 0x0 0x400000>;
        +         no-map;
        +  };
        
             
@note Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use IRQ_SetPriority() function.

@note The application need to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.


@par Directory contents

    - LTDC/LTDC_Display_3rdLayer/CM33/NonSecure/Inc/stm32mp2xx_hal_conf.h     HAL configuration file
    - LTDC/LTDC_Display_3rdLayer/CM33/NonSecure/Inc/stm32mp257f_eval_conf.h   STM32MP257f-EV board configuration file
    - LTDC/LTDC_Display_3rdLayer/CM33/NonSecure/Inc/stm32mp2xx_it.h           Interrupt handlers header file
    - LTDC/LTDC_Display_3rdLayer/CM33/NonSecure/Inc/main.h                    Main configuration file
    - LTDC/LTDC_Display_3rdLayer/CM33/NonSecure/Src/main.c                    Main program
    - LTDC/LTDC_Display_3rdLayer/CM33/NonSecure/Src/stm32mp2xx_hal_msp.c      HAL MSP module
    - LTDC/LTDC_Display_3rdLayer/CM33/NonSecure/Src/stm32mp2xx_it.c           Interrupt handlers
    - LTDC/LTDC_Display_3rdLayer/CM33/NonSecure/Inc/stlogoSMALL.h             ST logo to be displayed on 3rd Layer content :  (180x127) in RGB565
    - LTDC/LTDC_Display_3rdLayer/CM33/NonSecure/Inc/stlogoBIG.h -             ST logo to be displayed on 3rd Layer content :  (240x169) in RGB565

@par Hardware and Software environment

  - This example runs on STM32MP2xx devices.
  - This example has been tested with STM32MP257F-EV1 board and can be
    easily tailored to any other supported device and development board.

@par How to use it ?
 
In order to make the program work, you must do the following:
- Open STM32CubeIDE
- Build with config: CA35TDCID_m33_ns_sign
- ssh root@192.168.7.1 'mkdir -p /home/root/LTDC_Display_3rdLayer/lib/firmware'
- scp the signed firmware LTDC_Display_3rdLayer_CM33_NonSecure_sign.bin to root@192.168.7.1:/home/root/LTDC_Display_3rdLayer/lib/firmware
- scp the script Firmware/Utilities/scripts/fw_cortex_m33.sh to root@192.168.7.1:/home/root/LTDC_Display_3rdLayer/
- To run the example on target: 
	On target shell run:
	- cd /home/root/LTDC_Display_3rdLayer
	- ./fw_cortex_m33.sh start


 */