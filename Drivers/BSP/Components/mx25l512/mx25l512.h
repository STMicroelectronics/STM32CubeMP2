/**
  ******************************************************************************
  * @file    mx25l512.h
  * @author  MCD Application Team
  * @brief   This file contains all the description of the MX25L512 QSPI memory.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2018 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef MX25L512_H
#define MX25L512_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "mx25l512_conf.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup Components
  * @{
  */

/** @addtogroup MX25L512
  * @{
  */

/** @defgroup MX25L512_Exported_Constants MX25L512 Exported Constants
  * @{
  */
/* Device ID & Secure OTP length *********************************************/
#define MX25L512_MANUFACTURER_ID             ((uint8_t)0xC2)
#define MX25L512_DEVICE_ID_MEM_TYPE          ((uint8_t)0x20)
#define MX25L512_DEVICE_ID_MEM_CAPACITY      ((uint8_t)0x1A)
#define MX25L512_SECURE_OTP_LENGTH           ((uint32_t)0x200)             /*!< 4K-bit => 512Bytes secured OTP */    
 
#define MX25L512_BLOCK_64K                   (uint32_t) (64 * 1024)        /*!< 256 blocks of 64KBytes */
#define MX25L512_BLOCK_32K                   (uint32_t) (32 * 1024)        /*!< 512 blocks of 32KBytes */
#define MX25L512_SECTOR_4K                   (uint32_t) (4  * 1024)        /*!< 4096 sectors of 4KBytes */

#define MX25L512_FLASH_SIZE                  (uint32_t) (512*1024*1024/8)  /*!< 512 MBits => 64MBytes */
#define MX25L512_SECTOR_SIZE                 MX25L512_SECTOR_4K
#define MX25L512_PAGE_SIZE                   (uint32_t) 256                /*!< 262144 pages of 256 Bytes */

/**
  * @brief  MX25L512 Configuration
  */
#define MX25L512_DUMMY_CLOCK_READ_SFDP       8U

#define MX25L512_BULK_ERASE_MAX_TIME         600000U
#define MX25L512_SECTOR_ERASE_MAX_TIME       2000U
#define MX25L512_SUBSECTOR_ERASE_MAX_TIME    800U

/* MX25L512 Component Error codes *********************************************/
#define MX25L512_OK                           0
#define MX25L512_ERROR_INIT                  -1
#define MX25L512_ERROR_COMMAND               -2
#define MX25L512_ERROR_TRANSMIT              -3
#define MX25L512_ERROR_RECEIVE               -4
#define MX25L512_ERROR_AUTOPOLLING           -5
#define MX25L512_ERROR_MEMORYMAPPED          -6

/******************************************************************************
  * @brief  MX25L512 Commands
  ****************************************************************************/
/***** Read/Write Array Commands (3/4 Byte Address Command Set) **************/
/* Read Operations */
#define MX25L512_READ_CMD                             0x03   /*!< READ, Normal Read 3/4 Byte Address; SPI 1-1-1                    */
#define MX25L512_FAST_READ_CMD                        0x0B   /*!< FAST READ, Fast Read 3/4 Byte Address; SPI 1-1-1                 */
#define MX25L512_1I2O_FAST_READ_CMD                   0x3B   /*!< DREAD, 1I 2O Read 3/4 Byte Address; SPI 1-1-2                    */
#define MX25L512_2IO_FAST_READ_CMD                    0xBB   /*!< 2READ, 2 x I/O Read 3/4 Byte Address; SPI 1-2-2                  */
#define MX25L512_1I4O_FAST_READ_CMD                   0x6B   /*!< QREAD, 1I 4O Read 3/4 Byte Address; SPI 1-1-4                    */
#define MX25L512_4IO_FAST_READ_CMD                    0xEB   /*!< 4READ, 4 I/O Read 3/4 Byte Address; SPI/QPI 1-1-1/4-4-4          */
#define MX25L512_FAST_FAST_READ_DTR_CMD               0x0D   /*!< FASTDTRD, Fast DTR Read 3/4 Byte Address; SPI 1-1-1              */
#define MX25L512_2IO_FAST_READ_DTR_CMD                0xBD   /*!< 2DTRD, Dual I/O DTR Read 3/4 Byte Address; SPI 1-2-2             */
#define MX25L512_4IO_FAST_READ_DTR_CMD                0xED   /*!< 4DTRD, Quad I/O DTR Read 3/4 Byte Address; SPI/QPI 1-1-1/4-4-4   */
                                                                                                                                  
/* Program Operations */                                                                                                          
#define MX25L512_PAGE_PROG_CMD                        0x02   /*!< PP, Page Program 3/4 Byte Address; SPI/QPI 1-1-1/4-4-4           */
#define MX25L512_QUAD_PAGE_PROG_CMD                   0x38   /*!< 4PP, Quad Page Program 3/4 Byte Address; SPI 1-4-4               */
                                                                                                                                   
/* Erase Operations */                                                                                                             
#define MX25L512_SECTOR_ERASE_4K_CMD                  0x20   /*!< SE, Sector Erase 4KB 3/4 Byte Address; SPI/QPI 1-1-0/4-4-0       */
#define MX25L512_BLOCK_ERASE_32K_CMD                  0x52   /*!< BE32K, Block Erase 32KB 3/4 Byte Address; SPI/QPI 1-1-0/4-4-0    */
#define MX25L512_BLOCK_ERASE_64K_CMD                  0xD8   /*!< BE, Block Erase 64KB 3/4 Byte Address; SPI/QPI 1-1-0/4-4-0       */
#define MX25L512_CHIP_ERASE_CMD                       0xC7   /*!< CE, Chip Erase 0 Byte Address; SPI/QPI 1-0-0/4-0-0               */
#define MX25L512_BULK_ERASE_CMD                       0x60   /*!< CE, Chip Erase 0 Byte Address; SPI/QPI 1-0-0/4-0-0               */

/***** Read/Write Array Commands (4 Byte Address Command Set) ****************/
/* Read Operations */
#define MX25L512_4_BYTE_ADDR_READ_CMD                 0x13   /*!< READ4B, Normal Read 4 Byte address; SPI 1-1-1                   */
#define MX25L512_4_BYTE_ADDR_FAST_READ_CMD            0x0C   /*!< FAST READ4B, Fast Read 4 Byte address; SPI 1-1-1                */
#define MX25L512_4_BYTE_ADDR_2IO_FAST_READ_CMD        0xBC   /*!< 2READ4B, Read by 2 x I/O 4 Byte address; SPI 1-2-2              */
#define MX25L512_4_BYTE_ADDR_1I2O_FAST_READ_CMD       0x3C   /*!< DREAD4B, Read by Dual Output 4 Byte address; SPI 1-1-2          */
#define MX25L512_4_BYTE_ADDR_4IO_FAST_READ_CMD        0xEC   /*!< 4READ4B, Read by 4 x I/O 4 Byte address; SPI/QPI 1-4-4/4-4-4    */
#define MX25L512_4_BYTE_ADDR_1I4O_FAST_READ_CMD       0x6C   /*!< QREAD4B, Read by Quad Output 4 Byte address; SPI 1-1-4          */
#define MX25L512_4_BYTE_ADDR_FAST_READ_DTR_CMD        0x0E   /*!< FRDTRD4B, Fast DTR Read 4 Byte address; SPI 1-1-1               */
#define MX25L512_4_BYTE_ADDR_2IO_FAST_READ_DTR_CMD    0xBE   /*!< 2DTRD4B, DTR read by 2 x I/O 4 Byte address; SPI 1-2-2          */
#define MX25L512_4_BYTE_ADDR_4IO_FAST_READ_DTR_CMD    0xEE   /*!< 4DTRD4B, Quad I/O DTR Read 4 Byte address; SPI/QPI 1-4-4/4-4-4  */
                                                                                                                                  
/* Program Operations */                                                                                                          
#define MX25L512_4_BYTE_ADDR_PAGE_PROG_CMD            0x12   /*!< PP4B, Page Program 4 Byte address; SPI/QPI 1-1-1/4-4-4          */
#define MX25L512_4_BYTE_ADDR_QUAD_PAGE_PROG_CMD       0x3E   /*!< 4PP4B, Quad Input Page Program 4 Byte address; SPI 1-4-4        */
                                                                                                                                  
/* Erase Operations */                                                                                                            
#define MX25L512_4_BYTE_ADDR_BLOCK_ERASE_64K_CMD      0xDC   /*!< BE4B, Block Erase 64KB 4 Byte address; SPI/QPI 1-1-0/4-4-0      */
#define MX25L512_4_BYTE_ADDR_BLOCK_ERASE_32K_CMD      0x5C   /*!< BE32K4B, Block Erase 32KB 4 Byte address; SPI/QPI 1-1-0/4-4-0   */
#define MX25L512_4_BYTE_ADDR_SECTOR_ERASE_4K_CMD      0x21   /*!< SE4B, Sector Erase 4KB 4 Byte address; SPI/QPI 1-1-0/4-4-0      */

/***** Register/Setting Commands *********************************************/
#define MX25L512_WRITE_ENABLE_CMD                     0x06   /*!< WREN, Write Enable; SPI/QPI                        */
#define MX25L512_WRITE_DISABLE_CMD                    0x04   /*!< WRDI, Write Disable; SPI/QPI                       */
                                                                                                                     
#define MX25L512_FACTORY_MODE_ENABLE_CMD              0x41   /*!< FMEN, Factory Mode Enable; SPI/QPI                 */
                                                                                                                     
#define MX25L512_READ_STATUS_REG_CMD                  0x05   /*!< RDSR, Read Status Register; SPI/QPI                */
#define MX25L512_READ_CFG_REG_CMD                     0x15   /*!< RDCR, Read Configuration Register; SPI/QPI         */
#define MX25L512_WRITE_STATUS_CFG_REG_CMD             0x01   /*!< WRSR, Write Status/Configuration Register; SPI/QPI */

#define MX25L512_READ_EXTENDED_ADDR_REG_CMD           0xC8   /*!< RDEAR, Read Extended Address Register; SPI/QPI   */
#define MX25L512_WRITE_EXTENDED_ADDR_REG_CMD          0xC5   /*!< WREAR, Write Extended Address Register; SPI/QPI  */
#define MX25L512_WRITE_PROTECT_SELECTION_CMD          0x68   /*!< WPSEL, Write Protect Selection; SPI/QPI          */

#define MX25L512_ENABLE_QSPI_CMD                      0x35   /*!< EQIO, Enable QPI; Enter QPI; SPI */
#define MX25L512_RESET_QSPI_CMD                       0xF5   /*!< RSTQIO, Reset QPI; Exit QPI; QPI */

#define MX25L512_ENTER_4_BYTE_ADDR_MODE_CMD           0xB7   /*!< EN4B, Enter 4-Byte mode (3/4 Byte address commands); SPI/QPI */
#define MX25L512_EXIT_4_BYTE_ADDR_MODE_CMD            0xE9   /*!< EX4B, Exit 4-Byte mode (3/4 Byte address commands); SPI/QPI  */

#define MX25L512_PROG_ERASE_SUSPEND_CMD               0xB0   /*!< PGM/ERS Suspend, Suspends Program/Erase; SPI/QPI */
#define MX25L512_PROG_ERASE_RESUME_CMD                0x30   /*!< PGM/ERS Resume, Resumes Program/Erase; SPI/QPI   */

#define MX25L512_DEEP_POWER_DOWN_CMD                  0xB9   /*!< DP, Deep power down; SPI/QPI */
#define MX25L512_RELEASE_FROM_DEEP_POWER_DOWN_CMD     0xAB   /*!< RDP, Release from Deep Power down; SPI/QPI */

#define MX25L512_SET_BURST_LENGTH_CMD                 0xC0   /*!< SBL, Set burst length; SPI/QPI */

#define MX25L512_READ_FAST_BOOT_REGISTER_CMD          0x16   /*!< RDFBR, Read Fast Boot Register; SPI  */
#define MX25L512_WRITE_FAST_BOOT_REGISTER_CMD         0x17   /*!< WRFBR, Write Fast Boot Register; SPI */
#define MX25L512_ERASE_FAST_BOOT_REGISTER_CMD         0x18   /*!< ESFBR, Erase Fast Boot Register; SPI */

/***** ID/Security Commands **************************************************/
/* Identification Operations */
#define MX25L512_READ_ID_CMD                          0x9F   /*!< RDID, Read IDentification; SPI                         */
#define MX25L512_READ_ELECTRONIC_ID_CMD               0xAB   /*!< RES, Read Electronic ID; SPI/QPI                       */
#define MX25L512_READ_ELECTRONIC_MANFACTURER_ID_CMD   0x90   /*!< REMS, Read Electronic Manufacturer ID & Device ID; SPI */
#define MX25L512_MULTIPLE_IO_READ_ID_CMD              0xAF   /*!< QPIID, QPI ID Read; QPI                                */

#define MX25L512_READ_SERIAL_FLASH_DISCO_PARAM_CMD    0x5A   /*!< RDSFDP, Read Serial Flash Discoverable Parameter; SPI/QPI */

#define MX25L512_ENTER_SECURED_OTP_CMD                0xB1   /*!< ENSO, Enter Secured OTP; SPI/QPI */
#define MX25L512_EXIT_SECURED_OTP_CMD                 0xC1   /*!< EXSO, Exit Secured OTP; SPI/QPI  */

#define MX25L512_READ_SECURITY_REGISTER_CMD           0x2B   /*!< RDSCUR, Read Security Register; SPI/QPI  */
#define MX25L512_WRITE_SECURITY_REGISTER_CMD          0x2F   /*!< WRSCUR, Write Security Register; SPI/QPI */

#define MX25L512_GANG_BLOCK_LOCK_CMD                  0x7E   /*!< GBLK, Gang Block Lock; SPI/QPI     */
#define MX25L512_GANG_BLOCK_UNLOCK_CMD                0x98   /*!< GBULK, Gang Block Unlock; SPI/QPI  */
                                                                                                     
#define MX25L512_WRITE_LOCK_REG_CMD                   0x2C   /*!< WRLR, Write Lock Register; SPI */
#define MX25L512_READ_LOCK_REG_CMD                    0x2D   /*!< RDLR, Read Lock Register; SPI  */

#define MX25L512_WRITE_PASSWORD_REGISTER_CMD          0x28   /*!< WRPASS, Write Password Register; SPI */
#define MX25L512_READ_PASSWORD_REGISTER_CMD           0x27   /*!< RDPASS, Read Password Register; SPI  */
#define MX25L512_PASSWORD_UNLOCK_CMD                  0x29   /*!< PASSULK, Password Unlock; SPI        */

#define MX25L512_WRITE_SPB_CMD                        0xE3   /*!< WRSPB, SPB bit program 4 Byte address; SPI */ 
#define MX25L512_ERASE_SPB_CMD                        0xE4   /*!< ESSPB, All SPB bit Erase; SPI              */
#define MX25L512_READ_SPB_STATUS_CMD                  0xE2   /*!< RDSPB, Read SPB Status 4 Byte address; SPI */

#define MX25L512_SPB_LOCK_SET_CMD                     0xA6   /*!< SPBLK, SPB Lock set; SPI              */
#define MX25L512_READ_SPB_LOCK_REGISTER_CMD           0xA7   /*!< RDSPBLK, SPB Lock Register Read; SPI  */

#define MX25L512_WRITE_DPB_REGISTER_CMD               0xE1   /*!< WRDPB, Write DPB Register; SPI   */
#define MX25L512_READ_DPB_REGISTER_CMD                0xE0   /*!< RDDPB, Read DPB Register; SPI    */
                                                                                                   
/***** Reset Commands ********************************************************/
#define MX25L512_NO_OPERATION_CMD                     0x00   /*!< NOP, No Operation; SPI/QPI     */
#define MX25L512_RESET_ENABLE_CMD                     0x66   /*!< RSETEN, Reset Enable; SPI/QPI  */
#define MX25L512_RESET_MEMORY_CMD                     0x99   /*!< RST, Reset Memory; SPI/QPI     */


/******************************************************************************
  * @brief  MX25L512 Registers
  ****************************************************************************/
/* Status Register */
#define MX25L512_SR_WIP                      ((uint8_t)0x01)    /*!< Write in progress */
#define MX25L512_SR_WREN                     ((uint8_t)0x02)    /*!< Write enable latch */
#define MX25L512_SR_BLOCKPR                  ((uint8_t)0x3C)    /*!< Block protected against program and erase operations */
#define MX25L512_SR_QUADEN                   ((uint8_t)0x40)    /*!< QE bit; Quad IO mode enabled if =1 */
#define MX25L512_SR_SRWD                     ((uint8_t)0x80)    /*!< Status register write enable/disable */

/* Configuration Register */
#define MX25L512_CR_ODS                      ((uint8_t)0x07)    /*!< Output driver strength */
#define MX25L512_CR_TB                       ((uint8_t)0x08)    /*!< Top/Bottom bit used to configure the block protect area */
#define MX25L512_CR_PBE                      ((uint8_t)0x10)    /*!< Preamble Bit Enable */
#define MX25L512_CR_4BYTE                    ((uint8_t)0x20)    /*!< 3-Bytes or 4-Bytes addressing */
#define MX25L512_CR_DUMMY                    ((uint8_t)0xC0)    /*!< Dummy Clock Cycles setting */

/* Fast Boot Register */
#define MX25L512_FBR_FBE                     ((uint32_t)0x00000001) /*!< FastBoot Enable */
#define MX25L512_FBR_FBSD                    ((uint32_t)0x00000006) /*!< FastBoot Start Delay Cycle */
#define MX25L512_FBR_FBSD_7                  ((uint32_t)0x00000000) /*!< FastBoot Start 7 Delay Cycle */
#define MX25L512_FBR_FBSD_9                  ((uint32_t)0x00000001) /*!< FastBoot Start 9 Delay Cycle */
#define MX25L512_FBR_FBSD_11                 ((uint32_t)0x00000002) /*!< FastBoot Start 11 Delay Cycle */
#define MX25L512_FBR_FBSD_13                 ((uint32_t)0x00000003) /*!< FastBoot Start 12 Delay Cycle */
#define MX25L512_FBR_FBSA                    ((uint32_t)0xFFFFFFF0) /*!< FastBoot Start Address */

/* Security Register */
#define MX25L512_SCUR_OTP_INDICATOR          ((uint8_t)0x01)    /*!< Secured OTP indicator bit */
#define MX25L512_SCUR_LDSO                   ((uint8_t)0x02)    /*!< Indicate if Lock-down Secured OTP */
#define MX25L512_SCUR_PSB                    ((uint8_t)0x04)    /*!< Program Suspend bit */
#define MX25L512_SCUR_ESB                    ((uint8_t)0x08)    /*!< Erase Suspend bit */
#define MX25L512_SCUR_PROGRAM_FAIL           ((uint8_t)0x20)    /*!< Indicate Program failed */
#define MX25L512_SCUR_ERASE_FAIL             ((uint8_t)0x40)    /*!< Indicate Erase failed */
#define MX25L512_SCUR_WPSEL                  ((uint8_t)0x80)    /*!< Write Protection Selection */

/* Lock Register */
#define MX25L512_LR_SOLID_LOCK               ((uint16_t)0x0002) /*!< Solid Protection Mode Lock bit (OTP) */
#define MX25L512_LR_PASSWORD_LOCK            ((uint16_t)0x0004) /*!< Password Protection Mode Lock bit (OTP) */

/* SPB Lock Register */
#define MX25L512_SPBLR_SPBLK                 ((uint8_t)0x01)    /*!< SPB Lock Bit */

/**
  * @}
  */

/** @defgroup MX25L512_Exported_Types MX25L512 Exported Types
  * @{
  */    

typedef struct
{
  uint32_t FlashSize;                        /*!< Size of the flash                             */
  uint32_t EraseSectorSize;                  /*!< Size of sectors for the erase operation       */
  uint32_t EraseSectorsNumber;               /*!< Number of sectors for the erase operation     */
  uint32_t EraseSubSectorSize;               /*!< Size of subsector for the erase operation     */
  uint32_t EraseSubSectorNumber;             /*!< Number of subsector for the erase operation   */
  uint32_t EraseSubSector1Size;              /*!< Size of subsector 1 for the erase operation   */
  uint32_t EraseSubSector1Number;            /*!< Number of subsector 1 for the erase operation */
  uint32_t ProgPageSize;                     /*!< Size of pages for the program operation       */
  uint32_t ProgPagesNumber;                  /*!< Number of pages for the program operation     */
} MX25L512_Info_t;

typedef enum
{
  MX25L512_SPI_MODE = 0,                 /*!< 1-1-1 commands, Power on H/W default setting */
  MX25L512_SPI_2IO_MODE,                 /*!< 1-1-2, 1-2-2 read commands                   */
  MX25L512_SPI_4IO_MODE,                 /*!< 1-1-4, 1-4-4 read commands                   */
  MX25L512_QPI_MODE                      /*!< 4-4-4 commands                               */
} MX25L512_Interface_t;

typedef enum
{
  MX25L512_STR_TRANSFER = 0,             /* Single Transfer Rate */
  MX25L512_DTR_TRANSFER                  /* Double Transfer Rate */
} MX25L512_Transfer_t;

typedef enum 
{
  MX25L512_DUALFLASH_DISABLE = QSPI_DUALFLASH_DISABLE, /*!<  Single flash mode              */
} MX25L512_DualFlash_t;

typedef enum
{
  MX25L512_ERASE_4K = 0,                 /*!< 4K size Sector erase */
  MX25L512_ERASE_32K,                    /*!< 32K size Block erase */
  MX25L512_ERASE_64K,                    /*!< 64K size Block erase */
  MX25L512_ERASE_CHIP                    /*!< Whole chip erase     */
} MX25L512_Erase_t;

typedef enum
{
  MX25L512_BURST_READ_WRAP_8 = 0,
  MX25L512_BURST_READ_WRAP_16,
  MX25L512_BURST_READ_WRAP_32,
  MX25L512_BURST_READ_WRAP_64,
  MX25L512_BURST_READ_WRAP_NONE = 0x1F   /*!< Disable wrap function */
} MX25L512_WrapLength_t;

/* Configuration Register ODS & Dummy clock cycle setting ******************************/
typedef enum
{
  MX25L512_CR_ODS_90 = 1,                /*!< Output driver strength 90 ohms */
  MX25L512_CR_ODS_60,                    /*!< Output driver strength 60 ohms */
  MX25L512_CR_ODS_45,                    /*!< Output driver strength 45 ohms */
  MX25L512_CR_ODS_20 = 5,                /*!< Output driver strength 20 ohms */
  MX25L512_CR_ODS_15,                    /*!< Output driver strength 15 ohms */
  MX25L512_CR_ODS_30                     /*!< Output driver strength 30 ohms (default)*/
} MX25L512_ODSTypeDef;

/* MX25L512 Dummy Clock setting
 *          |             Instruction format
 *    DUMMY |1-1-1, 1-1-2     |1-2-2     |1-4-4
 *    [1:0] |1-1-4, 1-1-1(DTR)|1-2-2(DTR)|4-4-4, 4-4-4(DTR)
 *   -------+-----------------+----------+-----------------
 *     00   |    8 clocks     | 4 clocks |    6 clocks
 *     01   |    6 clocks     | 6 clocks |    4 clocks
 *     10   |    8 clocks     | 8 clocks |    8 clocks
 *     11   |   10 clocks     |10 clocks |   10 clocks
 */
typedef enum
{
  MX25L512_CR_DUMMY_00 = 0,     /*!<  8 clocks for 1-1-x; 4 clocks for 1-2-2; 6 clocks for x-4-4 */
  MX25L512_CR_DUMMY_01,         /*!<  6 clocks for 1-1-x; 6 clocks for 1-2-2; 4 clocks for x-4-4 */
  MX25L512_CR_DUMMY_10,         /*!<  8 clocks */
  MX25L512_CR_DUMMY_11          /*!< 10 clocks */
} MX25L512_DummyValue_t;

typedef enum
{
  MX25L512_CR_DUMMY11x_08 = 0,  /*!< 1-1-1 133MHz, 1-1-2 133MHz, 1-1-4 133MHz, 1-1-1 DTR 66MHz */
  MX25L512_CR_DUMMY11x_6,       /*!< 1-1-1 133MHz, 1-1-2 133MHz, 1-1-4 104MHz, 1-1-1 DTR 66MHz */
  MX25L512_CR_DUMMY11x_8,       /*!< 1-1-1 133MHz, 1-1-2 133MHz, 1-1-4 133MHz, 1-1-1 DTR 66MHz */
  MX25L512_CR_DUMMY11x_10       /*!< 1-1-1 166MHz, 1-1-2 166MHz, 1-1-4 166MHz, 1-1-1 DTR 83MHz */
} MX25L512_Dummy11x_t;

typedef enum
{
  MX25L512_CR_DUMMY12x_4 = 0,   /*!< 1-2-2  84MHz, 1-2-2 DTR 52MHz */
  MX25L512_CR_DUMMY12x_6,       /*!< 1-2-2 104MHz, 1-2-2 DTR 66MHz */
  MX25L512_CR_DUMMY12x_8,       /*!< 1-2-2 133MHz, 1-2-2 DTR 66MHz */
  MX25L512_CR_DUMMY12x_10       /*!< 1-2-2 166MHz, 1-2-2 DTR 83MHz */
} MX25L512_Dummy122_t;

typedef enum
{
  MX25L512_CR_DUMMYx44_6 = 0,   /*!< 1-4-4/4-4-4  84MHz, 4-4-4 DTR  52MHz */
  MX25L512_CR_DUMMYx44_4,       /*!< 1-4-4/4-4-4  70MHz, 4-4-4 DTR  42MHz */
  MX25L512_CR_DUMMYx44_8,       /*!< 1-4-4/4-4-4 104MHz, 4-4-4 DTR  66MHz */
  MX25L512_CR_DUMMYx44_10       /*!< 1-4-4/4-4-4 133MHz, 4-4-4 DTR 100MHz */
} MX25L512_Dummyx44_t;
/**
  * @}
  */

/** @defgroup MX25L512_Exported_Functions MX25L512 Exported Functions
  * @{
  */ 
/* Function by commands combined *********************************************/
int32_t MX25L512_GetFlashInfo(MX25L512_Info_t *pInfo);
int32_t MX25L512_AutoPollingMemReady(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode);

/* Read/Write Array Commands (4 Byte Address Command Set) ********************/
int32_t MX25L512_ReadSTR(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode, uint8_t *pData, uint32_t ReadAddr, uint32_t Size);
int32_t MX25L512_ReadDTR(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode, uint8_t *pData, uint32_t ReadAddr, uint32_t Size);
int32_t MX25L512_PageProgram(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode, uint8_t *pData, uint32_t WriteAddr, uint32_t Size);
int32_t MX25L512_BlockErase(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode, uint32_t BlockAddress, MX25L512_Erase_t BlockSize);
int32_t MX25L512_ChipErase(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode);

int32_t MX25L512_EnableMemoryMappedModeSTR(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode);
int32_t MX25L512_EnableMemoryMappedModeDTR(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode);

/* Register/Setting Commands *************************************************/
int32_t MX25L512_WriteEnable(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode);
int32_t MX25L512_WriteDisable(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode);
int32_t MX25L512_FactoryModeEnable(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode);
int32_t MX25L512_ReadStatusRegister(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode, uint8_t *Value);
int32_t MX25L512_ReadConfigurationRegister(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode, uint8_t *Value);
int32_t MX25L512_WriteStatusConfigurationRegister(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode, uint8_t *Value, uint32_t Length);
int32_t MX25L512_ReadExtendedAddressRegister(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode, uint8_t *EAR);
int32_t MX25L512_WriteExtendedAddressRegister(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode, uint8_t EAR);
int32_t MX25L512_WriteProtectSelection(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode);
int32_t MX25L512_EnterQPIMode(QSPI_HandleTypeDef *Ctx);
int32_t MX25L512_ExitQPIMode(QSPI_HandleTypeDef *Ctx);
int32_t MX25L512_Enter4BytesAddressMode(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode);
int32_t MX25L512_Exit4BytesAddressMode(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode);
int32_t MX25L512_ProgEraseSuspend(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode);
int32_t MX25L512_ProgEraseResume(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode);
int32_t MX25L512_EnterDeepPowerDown(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode);
int32_t MX25L512_ReleaseFromDeepPowerDown(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode);
int32_t MX25L512_SetBurstLength(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode, MX25L512_WrapLength_t BurstLength);
int32_t MX25L512_ReadFastBootRegister(QSPI_HandleTypeDef *Ctx, uint8_t *FastBoot);
int32_t MX25L512_WriteFastBootRegister(QSPI_HandleTypeDef *Ctx, uint8_t *FastBoot);
int32_t MX25L512_EraseFastBootRegister(QSPI_HandleTypeDef *Ctx);

/* ID/Security Commands ******************************************************/
int32_t MX25L512_ReadID(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode, uint8_t *ID);
int32_t MX25L512_ReadSFDP(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode, uint8_t *pData, uint32_t ReadAddr, uint32_t Size);
int32_t MX25L512_EnterSecuredOTP(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode);
int32_t MX25L512_ExitSecuredOTP(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode);
int32_t MX25L512_ReadSecurityRegister(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode, uint8_t *Security);
int32_t MX25L512_WriteSecurityRegister(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode);
int32_t MX25L512_GangBlockLock(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode);
int32_t MX25L512_GangBlockUnlock(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode);
int32_t MX25L512_WriteLockRegister(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode, uint8_t *Lock);
int32_t MX25L512_ReadLockRegister(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode, uint8_t *Lock);
int32_t MX25L512_WritePasswordRegister(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode, uint8_t *Password);
int32_t MX25L512_ReadPasswordRegister(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode, uint8_t *Password);
int32_t MX25L512_PasswordUnlock(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode, uint8_t *Password);
int32_t MX25L512_WriteSPB(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode, uint32_t WriteAddr);
int32_t MX25L512_EraseSPB(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode);
int32_t MX25L512_ReadSPBStatus(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode, uint32_t ReadAddr, uint8_t *SPBStatus);
int32_t MX25L512_SPBLockSet(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode);
int32_t MX25L512_ReadSPBLockRegister(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode, uint8_t *SPBRegister);
int32_t MX25L512_WriteDPBRegister(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode, uint32_t WriteAddr, uint8_t DPBStatus);
int32_t MX25L512_ReadDPBRegister(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode, uint32_t ReadAddr, uint8_t *DPBRegister);

/* Reset Commands ************************************************************/
int32_t MX25L512_NoOperation(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode);
int32_t MX25L512_ResetEnable(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode);
int32_t MX25L512_ResetMemory(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode);

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* MX25L512_H */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
