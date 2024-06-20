/**
 ******************************************************************************
 * @file    mx25l512.c
 * @Author  MCD Application Team
 * @brief   This file provides the MX25L512 QSPI drivers.
 ******************************************************************************
 * MX25L512 action :
 *   QE(Quad Enable, Non-volatile) bit of Status Register
 *   QE = 0; WP# & RESET# pin active
 *           Accept 1-1-1, 1-1-2, 1-2-2 commands
 *   QE = 1; WP# become SIO2 pin, RESET# become SIO3 pin
 *           Accept 1-1-1, 1-1-2, 1-2-2, 1-1-4, 1-4-4 commands
 *   Enter QPI mode by issue EQIO(0x35) command from 1-1-1 mode
 *           Accept 4-4-4 commands
 *   Exit QPI mode by issue RSTQIO(0xF5) command from 4-4-4 mode
 *           Accept commands, dependents on QE bit status
 *   Memory Read commands support STR(Single Transfer Rate) &
 *   DTR(Double Transfer Rate) mode
 *
 * MX25L512 Dummy Clock Cycle :
 *     Dummy clock cycle configured by bit 7, 6 of Configuration Register.
 *   The setting related to SCLK frequency & I/O interface number.
 *          |             Instruction format
 *    DUMMY |1-1-1, 1-1-2     |1-2-2     |1-4-4
 *    [1:0] |1-1-4, 1-1-1(DTR)|1-2-2(DTR)|4-4-4, 4-4-4(DTR)
 *   -------+-----------------+----------+-----------------
 *     00   |    8 clocks     | 4 clocks |    6 clocks
 *     01   |    6 clocks     | 6 clocks |    4 clocks
 *     10   |    8 clocks     | 8 clocks |    8 clocks
 *     11   |   10 clocks     |10 clocks |   10 clocks    <-- Driver setting
 *   DUMMY{1:0] = 11, means 10 dummy clock cycle at max. SCLK frequency supported.
 *
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

/* Includes ------------------------------------------------------------------*/
#include "mx25l512.h"

/** @addtogroup BSP
  * @{
  */
  
/** @addtogroup Components
  * @{
  */ 

/** @defgroup MX25L512 MX25L512
  * @{
  */

/** @defgroup MX25L512_Exported_Functions MX25L512 Exported Functions
  * @{
  */

/**
  * @brief  Get Flash information
  * @param  Component object pointer
  * @retval QSPI memory status
  */
int32_t MX25L512_GetFlashInfo(MX25L512_Info_t *pInfo)
{
  /* Configure the structure with the memory configuration */
  pInfo->FlashSize              = MX25L512_FLASH_SIZE;
  pInfo->EraseSectorSize        = MX25L512_BLOCK_64K;
  pInfo->EraseSectorsNumber     = (MX25L512_FLASH_SIZE/MX25L512_BLOCK_64K);
  pInfo->EraseSubSectorSize     = MX25L512_SECTOR_4K;
  pInfo->EraseSubSectorNumber   = (MX25L512_FLASH_SIZE/MX25L512_SECTOR_4K);
  pInfo->EraseSubSector1Size    = MX25L512_SECTOR_4K;
  pInfo->EraseSubSector1Number  = (MX25L512_FLASH_SIZE/MX25L512_SECTOR_4K);
  pInfo->ProgPageSize           = MX25L512_PAGE_SIZE;
  pInfo->ProgPagesNumber        = (MX25L512_FLASH_SIZE/MX25L512_PAGE_SIZE);  
  return MX25L512_OK;
}

/**
  * @brief  Polling WIP(Write In Progress) bit become to 0
  *         SPI/QPI; 1-0-1/4-0-4
  * @param  Ctx Component object pointer
  * @param  Mode Interface mode
  * @retval QSPI memory status
  */
int32_t MX25L512_AutoPollingMemReady(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode)
{
  QSPI_CommandTypeDef      s_command;
  QSPI_AutoPollingTypeDef  s_config;

  /* Configure automatic polling mode to wait for memory ready */
  s_command.InstructionMode   = (Mode == MX25L512_QPI_MODE) ? QSPI_INSTRUCTION_4_LINES : QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction       = MX25L512_READ_STATUS_REG_CMD;
  s_command.AddressMode       = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DummyCycles       = 0;
  s_command.DataMode          = (Mode == MX25L512_QPI_MODE) ? QSPI_DATA_4_LINES : QSPI_DATA_1_LINE;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  s_config.Match           = 0;
  s_config.Mask            = MX25L512_SR_WIP;
  s_config.MatchMode       = QSPI_MATCH_MODE_AND;
  s_config.StatusBytesSize = 1;
  s_config.Interval        = 0x10;
  s_config.AutomaticStop   = QSPI_AUTOMATIC_STOP_ENABLE;

  if (HAL_QSPI_AutoPolling(Ctx, &s_command, &s_config, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L512_ERROR_AUTOPOLLING;
  }

  return MX25L512_OK;
}


/* Read/Write Array Commands (4 Byte Address Command Set) *********************/
/**
  * @brief  Reads an amount of data from the QSPI memory on STR mode.
  *         SPI/QPI; 1-1-1/1-2-2/1-4-4/4-4-4
  * @param  Ctx Component object pointer
  * @param  Mode Interface mode
  * @param  pData Pointer to data to be read
  * @param  ReadAddr Read start address
  * @param  Size Size of data to read
  * @retval QSPI memory status
  */
int32_t MX25L512_ReadSTR(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode, uint8_t *pData, uint32_t ReadAddr, uint32_t Size)
{
  QSPI_CommandTypeDef s_command;
  QSPI_HandleTypeDef *hQSPI = Ctx;

  switch(Mode)
  {
  case MX25L512_SPI_MODE :               /* 1-1-1 commands, Power on H/W default setting */
    s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction     = MX25L512_4_BYTE_ADDR_FAST_READ_CMD;
    s_command.AddressMode     = QSPI_ADDRESS_1_LINE;

    s_command.DummyCycles     = MX25L512_DUMMY_CLOCK_STR_11x;
    s_command.DataMode        = QSPI_DATA_1_LINE;
    break;

  case MX25L512_SPI_2IO_MODE :           /* 1-2-2 read commands */
    s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction     = MX25L512_4_BYTE_ADDR_2IO_FAST_READ_CMD;
    s_command.AddressMode     = QSPI_ADDRESS_2_LINES;

    s_command.DummyCycles     = MX25L512_DUMMY_CLOCK_STR_122;
    s_command.DataMode        = QSPI_DATA_2_LINES;
    break;

  case MX25L512_SPI_4IO_MODE :           /* 1-4-4 read commands */
    s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction     = MX25L512_4_BYTE_ADDR_4IO_FAST_READ_CMD;
    s_command.AddressMode     = QSPI_ADDRESS_4_LINES;

    s_command.DummyCycles     = MX25L512_DUMMY_CLOCK_STR_x44;
    s_command.DataMode        = QSPI_DATA_4_LINES;
    break;

  case MX25L512_QPI_MODE :               /* 4-4-4 commands */
    s_command.InstructionMode = QSPI_INSTRUCTION_4_LINES;
    s_command.Instruction     = MX25L512_4_BYTE_ADDR_4IO_FAST_READ_CMD;
    s_command.AddressMode     = QSPI_ADDRESS_4_LINES;

    s_command.DummyCycles     = MX25L512_DUMMY_CLOCK_STR_x44;
    s_command.DataMode        = QSPI_DATA_4_LINES;
    break;
  }

  /* Initialize the read command */
  s_command.AddressSize       = QSPI_ADDRESS_32_BITS;
  s_command.Address           = ReadAddr;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.NbData            = Size;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  /* Configure the command */
  if (HAL_QSPI_Command(Ctx, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L512_ERROR_COMMAND;
  }

  /* Set S# timing for Read command */
  MODIFY_REG(hQSPI->Instance->DCR, QUADSPI_DCR_CSHT, QSPI_CS_HIGH_TIME_1_CYCLE);

  /* Reception of the data */
  if (HAL_QSPI_Receive(Ctx, pData, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L512_ERROR_RECEIVE;
  }

  /* Restore S# timing for nonRead commands */
  MODIFY_REG(hQSPI->Instance->DCR, QUADSPI_DCR_CSHT, QSPI_CS_HIGH_TIME_4_CYCLE);

  return MX25L512_OK;
}

/**
  * @brief  Reads an amount of data from the QSPI memory on DTR mode.
  *         SPI/QPI; 1-1-1/1-2-2/1-4-4/4-4-4
  * @param  Ctx Component object pointer
  * @param  Mode Interface mode
  * @param  pData Pointer to data to be read
  * @param  ReadAddr Read start address
  * @param  Size Size of data to read
  * @retval QSPI memory status
  */
int32_t MX25L512_ReadDTR(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode, uint8_t *pData, uint32_t ReadAddr, uint32_t Size)
{
  QSPI_CommandTypeDef s_command;

  switch(Mode)
  {
  case MX25L512_SPI_MODE :               /* 1-1-1 commands, Power on H/W default setting */
    s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction     = MX25L512_4_BYTE_ADDR_FAST_READ_DTR_CMD;
    s_command.AddressMode     = QSPI_ADDRESS_1_LINE;

    s_command.DummyCycles     = MX25L512_DUMMY_CLOCK_DTR_11x;
    s_command.DataMode        = QSPI_DATA_1_LINE;
    break;

  case MX25L512_SPI_2IO_MODE :           /* 1-2-2 read commands */
    s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction     = MX25L512_4_BYTE_ADDR_2IO_FAST_READ_DTR_CMD;
    s_command.AddressMode     = QSPI_ADDRESS_2_LINES;

    s_command.DummyCycles     = MX25L512_DUMMY_CLOCK_DTR_122;
    s_command.DataMode        = QSPI_DATA_2_LINES;
    break;

  case MX25L512_SPI_4IO_MODE :           /* 1-4-4 read commands */
    s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction     = MX25L512_4_BYTE_ADDR_4IO_FAST_READ_DTR_CMD;
    s_command.AddressMode     = QSPI_ADDRESS_4_LINES;

    s_command.DummyCycles     = MX25L512_DUMMY_CLOCK_DTR_x44;
    s_command.DataMode        = QSPI_DATA_4_LINES;
    break;

  case MX25L512_QPI_MODE :               /* 4-4-4 commands */
    s_command.InstructionMode = QSPI_INSTRUCTION_4_LINES;
    s_command.Instruction     = MX25L512_4_BYTE_ADDR_4IO_FAST_READ_DTR_CMD;
    s_command.AddressMode     = QSPI_ADDRESS_4_LINES;

    s_command.DummyCycles     = MX25L512_DUMMY_CLOCK_DTR_x44;
    s_command.DataMode        = QSPI_DATA_4_LINES;
    break;
  }

  /* Initialize the read command */
  s_command.AddressSize       = QSPI_ADDRESS_32_BITS;
  s_command.Address           = ReadAddr;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.NbData            = Size;
  s_command.DdrMode           = QSPI_DDR_MODE_ENABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_HALF_CLK_DELAY;    /* DTR mode used */
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  /* Configure the command */
  if (HAL_QSPI_Command(Ctx, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L512_ERROR_COMMAND;
  }

  /* Reception of the data */
  if (HAL_QSPI_Receive(Ctx, pData, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L512_ERROR_RECEIVE;
  }

  return MX25L512_OK;
}


/**
  * @brief  Writes an amount of data to the QSPI memory.
  *         SPI/QPI; 1-1-1/1-4-4/4-4-4
  * @param  Ctx Component object pointer
  * @param  Mode Interface mode
  * @param  pData Pointer to data to be written
  * @param  WriteAddr Write start address
  * @param  Size Size of data to write. Range 1 ~ 256
  * @retval QSPI memory status
  */
int32_t MX25L512_PageProgram(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode, uint8_t *pData, uint32_t WriteAddr, uint32_t Size)
{
  QSPI_CommandTypeDef s_command;

  /* Setup erase command */
  switch(Mode)
  {
  case MX25L512_SPI_4IO_MODE :               /* 1-4-4 program commands */
    s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction     = MX25L512_4_BYTE_ADDR_QUAD_PAGE_PROG_CMD;
    s_command.AddressMode     = QSPI_ADDRESS_4_LINES;
    s_command.DataMode        = QSPI_DATA_2_LINES;
    break;

  case MX25L512_QPI_MODE :                   /* 4-4-4 commands */
    s_command.InstructionMode = QSPI_INSTRUCTION_4_LINES;
    s_command.Instruction     = MX25L512_4_BYTE_ADDR_PAGE_PROG_CMD;
    s_command.AddressMode     = QSPI_ADDRESS_4_LINES;

    s_command.DataMode        = QSPI_DATA_4_LINES;
    break;
    
  case MX25L512_SPI_MODE :                   /* 1-1-1 commands, Power on H/W default setting */
  case MX25L512_SPI_2IO_MODE :               /* 1-1-2, 1-2-2 commands */
  default :
    s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction     = MX25L512_4_BYTE_ADDR_PAGE_PROG_CMD;
    s_command.AddressMode     = QSPI_ADDRESS_1_LINE;
    s_command.DataMode        = QSPI_DATA_1_LINE;
    break;    
  }

  s_command.AddressSize       = QSPI_ADDRESS_32_BITS;
  s_command.Address           = WriteAddr;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DummyCycles       = 0;
  s_command.NbData            = Size;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  /* Configure the command */
  if (HAL_QSPI_Command(Ctx, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L512_ERROR_COMMAND;
  }

  /* Transmission of the data */
  if (HAL_QSPI_Transmit(Ctx, pData, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L512_ERROR_TRANSMIT;
  }

  return MX25L512_OK;
}

/**
  * @brief  Erases the specified block of the QSPI memory.
  *         MX25L512 support 4K, 32K, 64K size block erase commands.
  *         SPI/QPI; 1-1-0/4-4-0
  * @param  Ctx Component object pointer
  * @param  Mode Interface mode  
  * @param  BlockAddress Block address to erase
  * @retval QSPI memory status
  */
int32_t MX25L512_BlockErase(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode, uint32_t BlockAddress, MX25L512_Erase_t BlockSize)
{
  QSPI_CommandTypeDef s_command;

  /* Setup erase command */
  switch(BlockSize)
  {
  default :
  case MX25L512_ERASE_4K :
    s_command.Instruction     = MX25L512_4_BYTE_ADDR_SECTOR_ERASE_4K_CMD;
    break;

  case MX25L512_ERASE_32K :
    s_command.Instruction     = MX25L512_4_BYTE_ADDR_BLOCK_ERASE_32K_CMD;
    break;

  case MX25L512_ERASE_64K :
    s_command.Instruction     = MX25L512_4_BYTE_ADDR_BLOCK_ERASE_64K_CMD;
    break;

  case MX25L512_ERASE_CHIP :
    return MX25L512_ChipErase(Ctx, Mode);
  }

  /* Initialize the erase command */
  s_command.InstructionMode   = (Mode == MX25L512_QPI_MODE) ? QSPI_INSTRUCTION_4_LINES : QSPI_INSTRUCTION_1_LINE;
  s_command.AddressMode       = (Mode == MX25L512_QPI_MODE) ? QSPI_ADDRESS_4_LINES : QSPI_ADDRESS_1_LINE;
  s_command.AddressSize       = QSPI_ADDRESS_32_BITS;
  s_command.Address           = BlockAddress;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DummyCycles       = 0;
  s_command.DataMode          = QSPI_DATA_NONE;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  /* Send the command */
  if (HAL_QSPI_Command(Ctx, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L512_ERROR_COMMAND;
  }

  return MX25L512_OK;
}

/**
  * @brief  Whole chip erase.
  *         SPI/QPI; 1-0-0/4-0-0
  * @param  Ctx Component object pointer
  * @param  Mode Interface mode
  * @retval QSPI memory status
  */
int32_t MX25L512_ChipErase(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode)
{
  QSPI_CommandTypeDef s_command;

  /* Initialize the erase command */
  s_command.InstructionMode   = (Mode == MX25L512_QPI_MODE) ? QSPI_INSTRUCTION_4_LINES : QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction       = MX25L512_CHIP_ERASE_CMD;
  s_command.AddressMode       = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DummyCycles       = 0;
  s_command.DataMode          = QSPI_DATA_NONE;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  /* Send the command */
  if (HAL_QSPI_Command(Ctx, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L512_ERROR_COMMAND;
  }

  return MX25L512_OK;
}

/**
  * @brief  Reads an amount of data from the QSPI memory on STR mode.
  *         SPI/QPI; 1-1-1/1-2-2/1-4-4/4-4-4
  * @param  Ctx Component object pointer
  * @param  Mode Interface mode
  * @retval QSPI memory status
  */
int32_t MX25L512_EnableMemoryMappedModeSTR(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode)
{
  QSPI_CommandTypeDef      s_command;
  QSPI_MemoryMappedTypeDef s_mem_mapped_cfg;

  switch(Mode)
  {
  case MX25L512_SPI_MODE :               /* 1-1-1 commands, Power on H/W default setting */
    s_command.InstructionMode    = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction        = MX25L512_4_BYTE_ADDR_FAST_READ_CMD;
    s_command.AddressMode        = QSPI_ADDRESS_1_LINE;

    s_command.AlternateByteMode  = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DummyCycles        = MX25L512_DUMMY_CLOCK_STR_11x;
    s_command.DataMode           = QSPI_DATA_1_LINE;

    s_command.SIOOMode           = QSPI_SIOO_INST_EVERY_CMD;
    break;

  case MX25L512_SPI_2IO_MODE :           /* 1-2-2 read commands */
    s_command.InstructionMode    = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction        = MX25L512_4_BYTE_ADDR_2IO_FAST_READ_CMD;
    s_command.AddressMode        = QSPI_ADDRESS_2_LINES;

    s_command.AlternateByteMode  = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DummyCycles        = MX25L512_DUMMY_CLOCK_STR_122;
    s_command.DataMode           = QSPI_DATA_2_LINES;

    s_command.SIOOMode           = QSPI_SIOO_INST_EVERY_CMD;
    break;

  case MX25L512_SPI_4IO_MODE :           /* 1-4-4 read commands */
    s_command.InstructionMode    = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction        = MX25L512_4_BYTE_ADDR_4IO_FAST_READ_CMD;
    s_command.AddressMode        = QSPI_ADDRESS_4_LINES;

    s_command.AlternateByteMode  = CONF_MX25L512_READ_ENHANCE ? QSPI_ALTERNATE_BYTES_4_LINES : QSPI_ALTERNATE_BYTES_NONE;
    s_command.AlternateBytesSize = QSPI_ALTERNATE_BYTES_8_BITS;
    s_command.AlternateBytes     = 0x5A;
    s_command.DummyCycles        = CONF_MX25L512_READ_ENHANCE ? (MX25L512_DUMMY_CLOCK_STR_x44 - 2) : MX25L512_DUMMY_CLOCK_STR_x44;
    s_command.DataMode           = QSPI_DATA_4_LINES;

    s_command.SIOOMode           = CONF_MX25L512_READ_ENHANCE ? QSPI_SIOO_INST_ONLY_FIRST_CMD : QSPI_SIOO_INST_EVERY_CMD;
    break;

  case MX25L512_QPI_MODE :               /* 4-4-4 commands */
    s_command.InstructionMode    = QSPI_INSTRUCTION_4_LINES;
    s_command.Instruction        = MX25L512_4_BYTE_ADDR_4IO_FAST_READ_CMD;
    s_command.AddressMode        = QSPI_ADDRESS_4_LINES;

    s_command.AlternateByteMode  = CONF_MX25L512_READ_ENHANCE ? QSPI_ALTERNATE_BYTES_4_LINES : QSPI_ALTERNATE_BYTES_NONE;
    s_command.AlternateBytesSize = QSPI_ALTERNATE_BYTES_8_BITS;
    s_command.AlternateBytes     = 0x5A;
    s_command.DummyCycles        = CONF_MX25L512_READ_ENHANCE ? (MX25L512_DUMMY_CLOCK_STR_x44 - 2) : MX25L512_DUMMY_CLOCK_STR_x44;
    s_command.DataMode           = QSPI_DATA_4_LINES;

    s_command.SIOOMode           = CONF_MX25L512_READ_ENHANCE ? QSPI_SIOO_INST_ONLY_FIRST_CMD : QSPI_SIOO_INST_EVERY_CMD;
    break;
  }

  /* Configure the command for the read instruction */
  s_command.AddressSize       = QSPI_ADDRESS_32_BITS;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;

  /* Configure the memory mapped mode */
  s_mem_mapped_cfg.TimeOutActivation = QSPI_TIMEOUT_COUNTER_DISABLE;
  s_mem_mapped_cfg.TimeOutPeriod     = 0;

  if (HAL_QSPI_MemoryMapped(Ctx, &s_command, &s_mem_mapped_cfg) != HAL_OK)
  {
    return MX25L512_ERROR_MEMORYMAPPED;
  }

  return MX25L512_OK;
}

/**
  * @brief  Reads an amount of data from the QSPI memory on DTR mode.
  *         SPI/QPI; 1-1-1/1-2-2/1-4-4/4-4-4
  * @param  Ctx Component object pointer
  * @param  Mode Interface mode
  * @retval QSPI memory status
  */
int32_t MX25L512_EnableMemoryMappedModeDTR(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode)
{
  QSPI_CommandTypeDef      s_command;
  QSPI_MemoryMappedTypeDef s_mem_mapped_cfg;

  switch(Mode)
  {
  case MX25L512_SPI_MODE :               /* 1-1-1 commands, Power on H/W default setting */
    s_command.InstructionMode    = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction        = MX25L512_4_BYTE_ADDR_FAST_READ_DTR_CMD;
    s_command.AddressMode        = QSPI_ADDRESS_1_LINE;

    s_command.AlternateByteMode  = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DummyCycles        = MX25L512_DUMMY_CLOCK_DTR_11x;
    s_command.DataMode           = QSPI_DATA_1_LINE;

    s_command.SIOOMode           = QSPI_SIOO_INST_EVERY_CMD;
    break;

  case MX25L512_SPI_2IO_MODE :           /* 1-2-2 read commands */
    s_command.InstructionMode    = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction        = MX25L512_4_BYTE_ADDR_2IO_FAST_READ_DTR_CMD;
    s_command.AddressMode        = QSPI_ADDRESS_2_LINES;

    s_command.AlternateByteMode  = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DummyCycles        = MX25L512_DUMMY_CLOCK_DTR_122;
    s_command.DataMode           = QSPI_DATA_2_LINES;

    s_command.SIOOMode           = QSPI_SIOO_INST_EVERY_CMD;
    break;

  case MX25L512_SPI_4IO_MODE :           /* 1-4-4 read commands */
    s_command.InstructionMode    = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction        = MX25L512_4_BYTE_ADDR_4IO_FAST_READ_DTR_CMD;
    s_command.AddressMode        = QSPI_ADDRESS_4_LINES;

    s_command.AlternateByteMode  = CONF_MX25L512_READ_ENHANCE ? QSPI_ALTERNATE_BYTES_4_LINES : QSPI_ALTERNATE_BYTES_NONE;
    s_command.AlternateBytesSize = QSPI_ALTERNATE_BYTES_8_BITS;
    s_command.AlternateBytes     = 0x5A;
    s_command.DummyCycles        = CONF_MX25L512_READ_ENHANCE ? (MX25L512_DUMMY_CLOCK_DTR_x44 - 1) : MX25L512_DUMMY_CLOCK_DTR_x44;
    s_command.DataMode           = QSPI_DATA_4_LINES;

    s_command.SIOOMode           = CONF_MX25L512_READ_ENHANCE ? QSPI_SIOO_INST_ONLY_FIRST_CMD : QSPI_SIOO_INST_EVERY_CMD;
    break;

  case MX25L512_QPI_MODE :               /* 4-4-4 commands */
    s_command.InstructionMode    = QSPI_INSTRUCTION_4_LINES;
    s_command.Instruction        = MX25L512_4_BYTE_ADDR_4IO_FAST_READ_DTR_CMD;
    s_command.AddressMode        = QSPI_ADDRESS_4_LINES;

    s_command.AlternateByteMode  = CONF_MX25L512_READ_ENHANCE ? QSPI_ALTERNATE_BYTES_4_LINES : QSPI_ALTERNATE_BYTES_NONE;
    s_command.AlternateBytesSize = QSPI_ALTERNATE_BYTES_8_BITS;
    s_command.AlternateBytes     = 0x5A;
    s_command.DummyCycles        = CONF_MX25L512_READ_ENHANCE ? (MX25L512_DUMMY_CLOCK_DTR_x44 - 1) : MX25L512_DUMMY_CLOCK_DTR_x44;
    s_command.DataMode           = QSPI_DATA_4_LINES;

    s_command.SIOOMode           = CONF_MX25L512_READ_ENHANCE ? QSPI_SIOO_INST_ONLY_FIRST_CMD : QSPI_SIOO_INST_EVERY_CMD;
    break;
  }

  /* Configure the command for the read instruction */
  s_command.AddressSize       = QSPI_ADDRESS_32_BITS;
  s_command.DdrMode           = QSPI_DDR_MODE_ENABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_HALF_CLK_DELAY;

  /* Configure the memory mapped mode */
  s_mem_mapped_cfg.TimeOutActivation = QSPI_TIMEOUT_COUNTER_DISABLE;
  s_mem_mapped_cfg.TimeOutPeriod     = 0;

  if (HAL_QSPI_MemoryMapped(Ctx, &s_command, &s_mem_mapped_cfg) != HAL_OK)
  {
    return MX25L512_ERROR_MEMORYMAPPED;
  }

  return MX25L512_OK;
}

/* Register/Setting Commands **************************************************/
/**
  * @brief  This function send a Write Enable and wait it is effective.
  *         SPI/QPI; 1-0-0/4-0-0
  * @param  Ctx Component object pointer
  * @param  Mode Interface mode
  * @retval QSPI memory status
  */
int32_t MX25L512_WriteEnable(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode)
{
  QSPI_CommandTypeDef     s_command;
  QSPI_AutoPollingTypeDef s_config;

  /* Initialize the read ID command */
  s_command.InstructionMode   = (Mode == MX25L512_QPI_MODE) ? QSPI_INSTRUCTION_4_LINES : QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction       = MX25L512_WRITE_ENABLE_CMD;
  s_command.AddressMode       = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DummyCycles       = 0;
  s_command.DataMode          = QSPI_DATA_NONE;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  /* Configure the command */
  if (HAL_QSPI_Command(Ctx, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L512_ERROR_COMMAND;
  }

  /* Configure automatic polling mode to wait for write enabling */
  s_config.Match           = MX25L512_SR_WREN;
  s_config.Mask            = MX25L512_SR_WREN;
  s_config.MatchMode       = QSPI_MATCH_MODE_AND;
  s_config.StatusBytesSize = 1;
  s_config.Interval        = 0x10;
  s_config.AutomaticStop   = QSPI_AUTOMATIC_STOP_ENABLE;

  s_command.Instruction    = MX25L512_READ_STATUS_REG_CMD;
  s_command.DataMode       = (Mode == MX25L512_QPI_MODE) ? QSPI_DATA_4_LINES : QSPI_DATA_1_LINE;

  if (HAL_QSPI_AutoPolling(Ctx, &s_command, &s_config, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L512_ERROR_AUTOPOLLING;
  }

  return MX25L512_OK;
}

/**
  * @brief  This function reset the (WEL) Write Enable Latch bit.
  *         SPI/QPI; 1-0-0/4-0-0
  * @param  Ctx Component object pointer
  * @param  Mode Interface mode
  * @retval QSPI memory status
  */
int32_t MX25L512_WriteDisable(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode)
{
  QSPI_CommandTypeDef     s_command;

  /* Initialize the read ID command */
  s_command.InstructionMode   = (Mode == MX25L512_QPI_MODE) ? QSPI_INSTRUCTION_4_LINES : QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction       = MX25L512_WRITE_DISABLE_CMD;
  s_command.AddressMode       = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DummyCycles       = 0;
  s_command.DataMode          = QSPI_DATA_NONE;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  /* Configure the command */
  if (HAL_QSPI_Command(Ctx, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L512_ERROR_COMMAND;
  }

  return MX25L512_OK;
}

/**
  * @brief  Factory mode enable. For accelerate erase speed. MX25L512 command.
  *         Suspend command is not acceptable under factory mode.
  *         SPI/QPI; 1-0-0/4-0-0
  * @param  Ctx Component object pointer
  * @param  Mode Interface mode
  * @retval QSPI memory status
  */
int32_t MX25L512_FactoryModeEnable(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode)
{
  QSPI_CommandTypeDef s_command;

  /* Initialize Factory mode enable command */
  s_command.InstructionMode   = (Mode == MX25L512_QPI_MODE) ? QSPI_INSTRUCTION_4_LINES : QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction       = MX25L512_FACTORY_MODE_ENABLE_CMD;
  s_command.AddressMode       = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DummyCycles       = 0;
  s_command.DataMode          = QSPI_DATA_NONE;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  /* Send the command */
  if (HAL_QSPI_Command(Ctx, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L512_ERROR_COMMAND;
  }

  return MX25L512_OK;
}

/**
  * @brief  Read Flash Status register value
  *         SPI/QPI; 1-0-1/4-0-4
  * @param  Ctx Component object pointer
  * @param  Mode Interface mode 
  * @param  Value pointer to status register value   
  * @retval QSPI memory status
  */
int32_t MX25L512_ReadStatusRegister(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode, uint8_t *Value)
{
  QSPI_CommandTypeDef s_command;

  /* Initialize the reading of status register */
  s_command.InstructionMode   = (Mode == MX25L512_QPI_MODE) ? QSPI_INSTRUCTION_4_LINES : QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction       = MX25L512_READ_STATUS_REG_CMD;
  s_command.AddressMode       = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DummyCycles       = 0;
  s_command.DataMode          = (Mode == MX25L512_QPI_MODE) ? QSPI_DATA_4_LINES : QSPI_DATA_1_LINE;
  s_command.NbData            = 1;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  /* Configure the command */
  if (HAL_QSPI_Command(Ctx, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L512_ERROR_COMMAND;
  }

  /* Reception of the data */
  if (HAL_QSPI_Receive(Ctx, Value, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L512_ERROR_RECEIVE;
  }

  return MX25L512_OK;
}

/**
  * @brief  Read Flash Configuration register value
  *         SPI/QPI; 1-0-1/4-0-4
  * @param  Ctx Component object pointer
  * @param  Mode Interface mode
  * @param  Value pointer to configuration register value  
  * @retval QSPI memory status
  */
int32_t MX25L512_ReadConfigurationRegister(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode, uint8_t *Value)
{
  QSPI_CommandTypeDef s_command;

  /* Initialize the reading of configuration register */
  s_command.InstructionMode   = (Mode == MX25L512_QPI_MODE) ? QSPI_INSTRUCTION_4_LINES : QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction       = MX25L512_READ_CFG_REG_CMD;
  s_command.AddressMode       = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DummyCycles       = 0;
  s_command.DataMode          = (Mode == MX25L512_QPI_MODE) ? QSPI_DATA_4_LINES : QSPI_DATA_1_LINE;
  s_command.NbData            = 1;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  /* Configure the command */
  if (HAL_QSPI_Command(Ctx, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L512_ERROR_COMMAND;
  }

  /* Reception of the data */
  if (HAL_QSPI_Receive(Ctx, Value, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L512_ERROR_RECEIVE;
  }

  return MX25L512_OK;
}

/**
  * @brief  Write status & configuration register.
  *         SPI/QPI; 1-0-1/4-0-4
  * @param  Ctx Component object pointer
  * @param  Mode Interface mode
  * @param  Value for write to status & configuration register.
  * @param  Value for write to status & configuration register.
  * @param  Length command length  
  * @retval QSPI memory status
  */
int32_t MX25L512_WriteStatusConfigurationRegister(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode, uint8_t *Value, uint32_t Length)
{
  QSPI_CommandTypeDef s_command;

  /* Update the configuration register with new value */
  s_command.InstructionMode   = (Mode == MX25L512_QPI_MODE) ? QSPI_INSTRUCTION_4_LINES : QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction       = MX25L512_WRITE_STATUS_CFG_REG_CMD;
  s_command.AddressMode       = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DummyCycles       = 0;
  s_command.DataMode          = (Mode == MX25L512_QPI_MODE) ? QSPI_DATA_4_LINES : QSPI_DATA_1_LINE;
  s_command.NbData            = Length;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  /* Configure the write volatile configuration register command */
  if (HAL_QSPI_Command(Ctx, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L512_ERROR_COMMAND;
  }

  /* Transmission of the data */
  if (HAL_QSPI_Transmit(Ctx, Value, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L512_ERROR_TRANSMIT;
  }

  return MX25L512_OK;
}

/**
  * @brief  Read Extended Address Register.
  *         This register use with 3 Byte address command
  *         SPI/QPI; 1-0-1/4-0-4
  * @param  Ctx Component object pointer
  * @param  Mode Interface mode
  * @retval QSPI memory status
  */
int32_t MX25L512_ReadExtendedAddressRegister(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode, uint8_t *EAR)
{
  QSPI_CommandTypeDef s_command;

  /* Initialize the read ID command */
  s_command.InstructionMode   = (Mode == MX25L512_QPI_MODE) ? QSPI_INSTRUCTION_4_LINES : QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction       = MX25L512_READ_EXTENDED_ADDR_REG_CMD;
  s_command.AddressMode       = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DummyCycles       = 0;
  s_command.DataMode          = (Mode == MX25L512_QPI_MODE) ? QSPI_DATA_4_LINES : QSPI_DATA_1_LINE;
  s_command.NbData            = 1;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  /* Configure the command */
  if (HAL_QSPI_Command(Ctx, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L512_ERROR_COMMAND;
  }

  /* Reception of the data */
  if (HAL_QSPI_Receive(Ctx, EAR, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L512_ERROR_RECEIVE;
  }

  return MX25L512_OK;
}

/**
  * @brief  Write Extended Address Register.
  *         This register use with 3 Byte address command
  *         SPI/QPI; 1-0-1/4-0-4
  * @param  Ctx Component object pointer
  * @param  Mode Interface mode
  * @retval QSPI memory status
  */
int32_t MX25L512_WriteExtendedAddressRegister(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode, uint8_t EAR)
{
  QSPI_CommandTypeDef s_command;

  /* Initialize the read ID command */
  s_command.InstructionMode   = (Mode == MX25L512_QPI_MODE) ? QSPI_INSTRUCTION_4_LINES : QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction       = MX25L512_WRITE_EXTENDED_ADDR_REG_CMD;
  s_command.AddressMode       = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DummyCycles       = 0;
  s_command.DataMode          = (Mode == MX25L512_QPI_MODE) ? QSPI_DATA_4_LINES : QSPI_DATA_1_LINE;
  s_command.NbData            = 1;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  /* Configure the command */
  if (HAL_QSPI_Command(Ctx, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L512_ERROR_COMMAND;
  }

  /* Transmission of the data */
  if (HAL_QSPI_Transmit(Ctx, &EAR, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L512_ERROR_TRANSMIT;
  }

  return MX25L512_OK;
}

/**
  * @brief  Write Protect Selection; WPSEL,
  *         Enter and enable individual block protect mode
  *         SPI/QPI; 1-0-0/4-0-0
  * @param  Ctx Component object pointer
  * @param  Mode Interface mode
  * @retval QSPI memory status
  */
int32_t MX25L512_WriteProtectSelection(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode)
{
  QSPI_CommandTypeDef s_command;

  /* Initialize Write Protect Selection command */
  s_command.InstructionMode   = (Mode == MX25L512_QPI_MODE) ? QSPI_INSTRUCTION_4_LINES : QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction       = MX25L512_WRITE_PROTECT_SELECTION_CMD;
  s_command.AddressMode       = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DummyCycles       = 0;
  s_command.DataMode          = QSPI_DATA_NONE;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  /* Configure the command */
  if (HAL_QSPI_Command(Ctx, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L512_ERROR_COMMAND;
  }

  return MX25L512_OK;
}

/**
  * @brief  This function put QSPI memory in QPI mode (Quad I/O) from SPI mode.
  *         SPI -> QPI; 1-x-x -> 4-4-4
  *         SPI; 1-0-0
  * @param  Ctx Component object pointer
  * @param  Mode Interface mode
  * @retval QSPI memory status
  */
int32_t MX25L512_EnterQPIMode(QSPI_HandleTypeDef *Ctx)
{
  QSPI_CommandTypeDef      s_command;

  /* Initialize the QPI enable command */
  /* QSPI memory is supported to be in SPI mode, so CMD on 1 LINE */
  s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction       = MX25L512_ENABLE_QSPI_CMD;
  s_command.AddressMode       = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DummyCycles       = 0;
  s_command.DataMode          = QSPI_DATA_NONE;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  /* Send the command */
  if (HAL_QSPI_Command(Ctx, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L512_ERROR_COMMAND;
  }

  return MX25L512_OK;
}

/**
  * @brief  This function put QSPI memory in SPI mode (Single I/O) from QPI mode.
  *         QPI -> SPI; 4-4-4 -> 1-x-x
  *         QPI; 4-0-0
  * @param  Ctx Component object pointer
  * @param  Mode Interface mode
  * @retval QSPI memory status
  */
int32_t MX25L512_ExitQPIMode(QSPI_HandleTypeDef *Ctx)
{
  QSPI_CommandTypeDef      s_command;

  /* Initialize the read ID command */
  s_command.InstructionMode   = QSPI_INSTRUCTION_4_LINES;
  s_command.Instruction       = MX25L512_RESET_QSPI_CMD;
  s_command.AddressMode       = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DummyCycles       = 0;
  s_command.DataMode          = QSPI_DATA_NONE;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  /* Configure the command */
  if (HAL_QSPI_Command(Ctx, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L512_ERROR_COMMAND;
  }

  return MX25L512_OK;
}

/**
  * @brief  Flash enter 4 Byte address mode. Effect 3/4 address byte commands only.
  *         SPI/QPI; 1-0-0/4-0-0
  * @param  Ctx Component object pointer
  * @param  Mode Interface mode
  * @retval QSPI memory status
  */
int32_t MX25L512_Enter4BytesAddressMode(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode)
{
  QSPI_CommandTypeDef s_command;

  /* Initialize the read ID command */
  s_command.InstructionMode   = (Mode == MX25L512_QPI_MODE) ? QSPI_INSTRUCTION_4_LINES : QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction       = MX25L512_ENTER_4_BYTE_ADDR_MODE_CMD;
  s_command.AddressMode       = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DummyCycles       = 0;
  s_command.DataMode          = QSPI_DATA_NONE;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  /* Configure the command */
  if (HAL_QSPI_Command(Ctx, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L512_ERROR_COMMAND;
  }

  return MX25L512_OK;
}

/**
  * @brief  Flash exit 4 Byte address mode. Effect 3/4 address byte commands only.
  *         SPI/QPI; 1-0-0/4-0-0
  * @param  Ctx Component object pointer
  * @param  Mode Interface mode
  * @retval QSPI memory status
  */
int32_t MX25L512_Exit4BytesAddressMode(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode)
{
  QSPI_CommandTypeDef s_command;

  /* Initialize the read ID command */
  s_command.InstructionMode   = (Mode == MX25L512_QPI_MODE) ? QSPI_INSTRUCTION_4_LINES : QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction       = MX25L512_EXIT_4_BYTE_ADDR_MODE_CMD;
  s_command.AddressMode       = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DummyCycles       = 0;
  s_command.DataMode          = QSPI_DATA_NONE;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  /* Configure the command */
  if (HAL_QSPI_Command(Ctx, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L512_ERROR_COMMAND;
  }

  return MX25L512_OK;
}

/*
  * @brief  Program/Erases suspend. Interruption Program/Erase operations.
  *         After the device has entered Erase-Suspended mode,
  *         system can read any address except the block/sector being Program/Erased.
  *         SPI/QPI; 1-0-0/4-0-0
  * @param  Ctx Component object pointer
  * @param  Mode Interface mode
  * @retval QSPI memory status
  */
int32_t MX25L512_ProgEraseSuspend(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode)
{
  QSPI_CommandTypeDef s_command;

  /* Initialize the read ID command */
  s_command.InstructionMode   = (Mode == MX25L512_QPI_MODE) ? QSPI_INSTRUCTION_4_LINES : QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction       = MX25L512_PROG_ERASE_SUSPEND_CMD;
  s_command.AddressMode       = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DummyCycles       = 0;
  s_command.DataMode          = QSPI_DATA_NONE;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  /* Configure the command */
  if (HAL_QSPI_Command(Ctx, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L512_ERROR_COMMAND;
  }

  return MX25L512_OK;
}

/*
  * @brief  Program/Erases resume.
  *         SPI/QPI; 1-0-0/4-0-0
  * @param  Ctx Component object pointer
  * @param  Mode Interface mode
  * @retval QSPI memory status
  */
int32_t MX25L512_ProgEraseResume(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode)
{
  QSPI_CommandTypeDef s_command;

  /* Initialize the read ID command */
  s_command.InstructionMode   = (Mode == MX25L512_QPI_MODE) ? QSPI_INSTRUCTION_4_LINES : QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction       = MX25L512_PROG_ERASE_RESUME_CMD;
  s_command.AddressMode       = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DummyCycles       = 0;
  s_command.DataMode          = QSPI_DATA_NONE;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  /* Configure the command */
  if (HAL_QSPI_Command(Ctx, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L512_ERROR_COMMAND;
  }

  return MX25L512_OK;
}

/*
  * @brief  Deep power down.
  *         The device is not active and all Write/Program/Erase instruction are ignored.
  *         SPI/QPI; 1-0-0/4-0-0
  * @param  Ctx Component object pointer
  * @param  Mode Interface mode
  * @retval QSPI memory status
  */
int32_t MX25L512_EnterDeepPowerDown(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode)
{
  QSPI_CommandTypeDef s_command;

  /* Initialize the read ID command */
  s_command.InstructionMode   = (Mode == MX25L512_QPI_MODE) ? QSPI_INSTRUCTION_4_LINES : QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction       = MX25L512_DEEP_POWER_DOWN_CMD;
  s_command.AddressMode       = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DummyCycles       = 0;
  s_command.DataMode          = QSPI_DATA_NONE;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  /* Configure the command */
  if (HAL_QSPI_Command(Ctx, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L512_ERROR_COMMAND;
  }

  return MX25L512_OK;
}

/*
  * @brief  Release from deep power down.
  *         After CS# go high, system need wait tRES1 time for device ready.
  *         SPI/QPI; 1-0-0/4-0-0
  * @param  Ctx Component object pointer
  * @param  Mode Interface mode
  * @retval QSPI memory status
  */
int32_t MX25L512_ReleaseFromDeepPowerDown(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode)
{
  QSPI_CommandTypeDef s_command;

  /* Initialize the read ID command */
  s_command.InstructionMode   = (Mode == MX25L512_QPI_MODE) ? QSPI_INSTRUCTION_4_LINES : QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction       = MX25L512_RELEASE_FROM_DEEP_POWER_DOWN_CMD;
  s_command.AddressMode       = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DummyCycles       = 0;
  s_command.DataMode          = QSPI_DATA_NONE;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  /* Configure the command */
  if (HAL_QSPI_Command(Ctx, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L512_ERROR_COMMAND;
  }

  return MX25L512_OK;
}

/**
  * @brief  Set burst length.
  *         The SPI and QPI mode 4READ and 4READ4B read commands support the wrap around feature.
  *         Read commands QPI "EBh" "ECh" and SPI "EBh" "ECh" support this feature.
  *         SPI/QPI; 1-0-1/4-0-4
  * @param  Ctx Component object pointer
  * @param  Mode Interface mode
  * @param  BurstLength Value of burst length.
  * @retval QSPI memory status
  */
int32_t MX25L512_SetBurstLength(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode, MX25L512_WrapLength_t BurstLength)
{
  QSPI_CommandTypeDef s_command;
  uint8_t reg = BurstLength;

  /* Initialize the set burst length command */
  s_command.InstructionMode   = (Mode == MX25L512_QPI_MODE) ? QSPI_INSTRUCTION_4_LINES : QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction       = MX25L512_SET_BURST_LENGTH_CMD;
  s_command.AddressMode       = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DummyCycles       = 0;
  s_command.DataMode          = (Mode == MX25L512_QPI_MODE) ? QSPI_DATA_4_LINES : QSPI_DATA_1_LINE;
  s_command.NbData            = 1;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  /* Configure the command */
  if (HAL_QSPI_Command(Ctx, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L512_ERROR_COMMAND;
  }

  /* Transmission of the data */
  if (HAL_QSPI_Transmit(Ctx, &reg, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L512_ERROR_TRANSMIT;
  }

  return MX25L512_OK;
}

/**
  * @brief  Read flash fast boot register value, SPI only.
  *         SPI; 1-0-1
  * @param  Ctx Component object pointer
  * @param  FastBoot pointer to fast boot register value 
  * @retval QSPI memory status
  */
int32_t MX25L512_ReadFastBootRegister(QSPI_HandleTypeDef *Ctx, uint8_t *FastBoot)
{
  QSPI_CommandTypeDef s_command;

  /* Initialize the reading of fast boot register command */
  s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction       = MX25L512_READ_FAST_BOOT_REGISTER_CMD;
  s_command.AddressMode       = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DummyCycles       = 0;
  s_command.DataMode          = QSPI_DATA_1_LINE;
  s_command.NbData            = 4;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  /* Configure the command */
  if (HAL_QSPI_Command(Ctx, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L512_ERROR_COMMAND;
  }

  /* Reception of the data */
  if (HAL_QSPI_Receive(Ctx, FastBoot, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L512_ERROR_RECEIVE;
  }

  return MX25L512_OK;
}

/**
  * @brief  Write fast boot register. Always write 4 byte data, SPI only.
  *         SPI; 1-0-1
  * @param  Ctx Component object pointer
  * @param  Value for write to status register.
  * @param  FastBoot value to write to fast boot register  
  * @retval QSPI memory status
  */
int32_t MX25L512_WriteFastBootRegister(QSPI_HandleTypeDef *Ctx, uint8_t *FastBoot)
{
  QSPI_CommandTypeDef s_command;

  /* Initialize the write fast boot register command */
  s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction       = MX25L512_WRITE_FAST_BOOT_REGISTER_CMD;
  s_command.AddressMode       = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DummyCycles       = 0;
  s_command.DataMode          = QSPI_DATA_1_LINE;
  s_command.NbData            = 4;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  /* Configure the command */
  if (HAL_QSPI_Command(Ctx, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L512_ERROR_COMMAND;
  }

  /* Transmission of the data */
  if (HAL_QSPI_Transmit(Ctx, FastBoot, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L512_ERROR_TRANSMIT;
  }

  return MX25L512_OK;
}

/**
  * @brief  Erases fast boot register, SPI only.
  *         SPI; 1-0-0
  * @param  Ctx Component object pointer
  * @retval QSPI memory status
  */
int32_t MX25L512_EraseFastBootRegister(QSPI_HandleTypeDef *Ctx)
{
  QSPI_CommandTypeDef s_command;

  /* Initialize the Erases fast boot register command */
  s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction       = MX25L512_ERASE_FAST_BOOT_REGISTER_CMD;
  s_command.AddressMode       = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DummyCycles       = 0;
  s_command.DataMode          = QSPI_DATA_NONE;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  /* Send the command */
  if (HAL_QSPI_Command(Ctx, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L512_ERROR_COMMAND;
  }

  return MX25L512_OK;
}


/* ID/Security Commands *******************************************************/
/**
  * @brief  Read Flash 3 Byte IDs.
  *         Manufacturer ID, Memory type, Memory density
  *         SPI/QPI; 1-0-1/4-0-4
  * @param  Ctx Component object pointer
  * @param  Mode Interface mode
  * @param  ID pointer to flash id value  
  * @retval QSPI memory status
  */
int32_t MX25L512_ReadID(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode, uint8_t *ID)
{
  QSPI_CommandTypeDef s_command;

  /* Initialize the read ID command */
  s_command.InstructionMode   = (Mode == MX25L512_QPI_MODE) ? QSPI_INSTRUCTION_4_LINES : QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction       = (Mode == MX25L512_QPI_MODE) ? MX25L512_MULTIPLE_IO_READ_ID_CMD : MX25L512_READ_ID_CMD;
  s_command.AddressMode       = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DummyCycles       = 0;
  s_command.DataMode          = (Mode == MX25L512_QPI_MODE) ? QSPI_DATA_4_LINES : QSPI_DATA_1_LINE;
  s_command.NbData            = 3;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  /* Configure the command */
  if (HAL_QSPI_Command(Ctx, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L512_ERROR_COMMAND;
  }

  /* Reception of the data */
  if (HAL_QSPI_Receive(Ctx, ID, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L512_ERROR_RECEIVE;
  }

  return MX25L512_OK;
}

/**
  * @brief  Reads an amount of SFDP data from the QSPI memory.
  *         SFDP : Serial Flash Discoverable Parameter
  *         SPI/QPI; 1-1-1/4-4-4
  * @param  Ctx Component object pointer
  * @param  Mode Interface mode
  * @param  pData Pointer to data to be read
  * @param  ReadAddr Read start address
  * @param  Size Size of data to read in Byte
  * @retval QSPI memory status
  */
int32_t MX25L512_ReadSFDP(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode, uint8_t *pData, uint32_t ReadAddr, uint32_t Size)
{
  QSPI_CommandTypeDef s_command;

  /* Initialize the read SFDP command */
  s_command.InstructionMode   = (Mode == MX25L512_QPI_MODE) ? QSPI_INSTRUCTION_4_LINES : QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction       = MX25L512_READ_SERIAL_FLASH_DISCO_PARAM_CMD;
  s_command.AddressMode       = (Mode == MX25L512_QPI_MODE) ? QSPI_ADDRESS_4_LINES : QSPI_ADDRESS_1_LINE;
  s_command.AddressSize       = QSPI_ADDRESS_24_BITS;
  s_command.Address           = ReadAddr;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DummyCycles       = MX25L512_DUMMY_CLOCK_READ_SFDP;
  s_command.DataMode          = (Mode == MX25L512_QPI_MODE) ? QSPI_DATA_4_LINES : QSPI_DATA_1_LINE;
  s_command.NbData            = Size;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  /* Configure the command */
  if (HAL_QSPI_Command(Ctx, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L512_ERROR_COMMAND;
  }

  /* Reception of the data */
  if (HAL_QSPI_Receive(Ctx, pData, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L512_ERROR_RECEIVE;
  }

  return MX25L512_OK;
}

/**
  * @brief  Enter Secured OTP mode
  *         SPI/QPI; 1-0-0, 4-0-0
  * @param  Ctx Component object pointer
  * @param  Mode Interface mode
  * @retval QSPI memory status
  */
int32_t MX25L512_EnterSecuredOTP(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode)
{
  QSPI_CommandTypeDef s_command;

  /* Initialize the Enter Secured OTP command */
  s_command.InstructionMode   = (Mode == MX25L512_QPI_MODE) ? QSPI_INSTRUCTION_4_LINES : QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction       = MX25L512_ENTER_SECURED_OTP_CMD;
  s_command.AddressMode       = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DummyCycles       = 0;
  s_command.DataMode          = QSPI_DATA_NONE;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  /* Send the command */
  if (HAL_QSPI_Command(Ctx, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L512_ERROR_COMMAND;
  }

  return MX25L512_OK;
}

/**
  * @brief  Exit Secured OTP mode
  *         SPI/QPI; 1-0-0, 4-0-0
  * @param  Ctx Component object pointer
  * @param  Mode Interface mode
  * @retval QSPI memory status
  */
int32_t MX25L512_ExitSecuredOTP(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode)
{
  QSPI_CommandTypeDef s_command;

  /* Initialize the Exit Secured OTP command */
  s_command.InstructionMode   = (Mode == MX25L512_QPI_MODE) ? QSPI_INSTRUCTION_4_LINES : QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction       = MX25L512_EXIT_SECURED_OTP_CMD;
  s_command.AddressMode       = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DummyCycles       = 0;
  s_command.DataMode          = QSPI_DATA_NONE;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  /* Send the command */
  if (HAL_QSPI_Command(Ctx, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L512_ERROR_COMMAND;
  }

  return MX25L512_OK;
}

/**
  * @brief  Read Security Register value
  *         SPI/QPI; 1-0-1, 4-0-4
  * @param  Ctx Component object pointer
  * @param  Mode Interface mode
  * @param  Security pointer to security value  
  * @retval QSPI memory status
  */
int32_t MX25L512_ReadSecurityRegister(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode, uint8_t *Security)
{
  QSPI_CommandTypeDef s_command;

  /* Initialize the Read Security Register command */
  s_command.InstructionMode   = (Mode == MX25L512_QPI_MODE) ? QSPI_INSTRUCTION_4_LINES : QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction       = MX25L512_READ_SECURITY_REGISTER_CMD;
  s_command.AddressMode       = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DummyCycles       = 0;
  s_command.DataMode          = (Mode == MX25L512_QPI_MODE) ? QSPI_DATA_4_LINES : QSPI_DATA_1_LINE;
  s_command.NbData            = 1;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  /* Configure the command */
  if (HAL_QSPI_Command(Ctx, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L512_ERROR_COMMAND;
  }

  /* Reception of the data */
  if (HAL_QSPI_Receive(Ctx, Security, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L512_ERROR_RECEIVE;
  }

  return MX25L512_OK;
}

/**
  * @brief  Write Security Register. To set the "Lock-Down" bit as 1.
  *         SPI/QPI; 1-0-0, 4-0-0
  * @param  Ctx Component object pointer
  * @param  Mode Interface mode
  * @retval QSPI memory status
  */
int32_t MX25L512_WriteSecurityRegister(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode)
{
  QSPI_CommandTypeDef s_command;

  /* Write security register with new value */
  s_command.InstructionMode   = (Mode == MX25L512_QPI_MODE) ? QSPI_INSTRUCTION_4_LINES : QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction       = MX25L512_WRITE_SECURITY_REGISTER_CMD;
  s_command.AddressMode       = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DummyCycles       = 0;
  s_command.DataMode          = QSPI_DATA_NONE;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  /* Configure the write security register command */
  if (HAL_QSPI_Command(Ctx, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L512_ERROR_COMMAND;
  }

  return MX25L512_OK;
}

/**
  * @brief  Gang block lock. Whole chip write protect.
  *         SPI/QPI; 1-0-0, 4-0-0
  * @param  Ctx Component object pointer
  * @param  Mode Interface mode
  * @retval QSPI memory status
  */
int32_t MX25L512_GangBlockLock(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode)
{
  QSPI_CommandTypeDef s_command;

  /* Initialize gang block lock command */
  s_command.InstructionMode   = (Mode == MX25L512_QPI_MODE) ? QSPI_INSTRUCTION_4_LINES : QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction       = MX25L512_GANG_BLOCK_LOCK_CMD;
  s_command.AddressMode       = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DummyCycles       = 0;
  s_command.DataMode          = QSPI_DATA_NONE;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  /* Send the command */
  if(HAL_QSPI_Command(Ctx, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L512_ERROR_COMMAND;
  }

  return MX25L512_OK;
}

/**
  * @brief  Gang block unlock. Whole chip unprotect.
  *         SPI/QPI; 1-0-0, 4-0-0
  * @param  Ctx Component object pointer
  * @param  Mode Interface mode
  * @retval QSPI memory status
  */
int32_t MX25L512_GangBlockUnlock(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode)
{
  QSPI_CommandTypeDef s_command;

  /* Initialize the gang block unlock command */
  s_command.InstructionMode   = (Mode == MX25L512_QPI_MODE) ? QSPI_INSTRUCTION_4_LINES : QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction       = MX25L512_GANG_BLOCK_UNLOCK_CMD;
  s_command.AddressMode       = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DummyCycles       = 0;
  s_command.DataMode          = QSPI_DATA_NONE;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  /* Send the command */
  if(HAL_QSPI_Command(Ctx, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L512_ERROR_COMMAND;
  }

  return MX25L512_OK;
}

/**
  * @brief  Write lock register. 2 Byte, SPI only
  *         SPI; 1-0-1
  * @param  Ctx Component object pointer
  * @param  Mode Interface mode  
  * @param  Lock Value for write to register.
  * @retval QSPI memory status
  */
int32_t MX25L512_WriteLockRegister(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode, uint8_t *Lock)
{
  QSPI_CommandTypeDef s_command;

  /* Initialize the Write Lock Register command */
  s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction       = MX25L512_WRITE_LOCK_REG_CMD;
  s_command.AddressMode       = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DummyCycles       = 0;
  s_command.DataMode          = QSPI_DATA_1_LINE;
  s_command.NbData            = 2;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  /* Configure the command */
  if (HAL_QSPI_Command(Ctx,  &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L512_ERROR_COMMAND;
  }

  /* Transmission of the data */
  if (HAL_QSPI_Transmit(Ctx, Lock, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L512_ERROR_TRANSMIT;
  }

  return MX25L512_OK;
}

/**
  * @brief  Read Lock Register value. 2 Byte, SPI only
  *         SPI; 1-0-1
  * @param  Ctx Component object pointer
  * @param  Mode Interface mode  
  * @param  Lock Read back data store pointer
  * @retval QSPI memory status
  */
int32_t MX25L512_ReadLockRegister(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode, uint8_t *Lock)
{
  QSPI_CommandTypeDef s_command;

  /* Initialize the Read Lock Register command */
  s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction       = MX25L512_READ_LOCK_REG_CMD;
  s_command.AddressMode       = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DummyCycles       = 0;
  s_command.DataMode          = QSPI_DATA_1_LINE;
  s_command.NbData            = 2;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  /* Configure the command */
  if (HAL_QSPI_Command(Ctx, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L512_ERROR_COMMAND;
  }

  /* Reception of the data */
  if (HAL_QSPI_Receive(Ctx, Lock, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L512_ERROR_RECEIVE;
  }

  return MX25L512_OK;
}

/**
  * @brief  Write password register. Always write 8 byte data.
  *         SPI; 1-0-1
  * @param  Ctx Component object pointer
  * @param  Mode Interface mode  
  * @param  Password Value for write to status register.
  * @retval QSPI memory status
  */
int32_t MX25L512_WritePasswordRegister(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode, uint8_t *Password)
{
  QSPI_CommandTypeDef s_command;

  /* Initialize the Write password register command */
  s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction       = MX25L512_WRITE_PASSWORD_REGISTER_CMD;
  s_command.AddressMode       = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DummyCycles       = 0;
  s_command.DataMode          = QSPI_DATA_1_LINE;
  s_command.NbData            = 8;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  /* Configure the command */
  if (HAL_QSPI_Command(Ctx, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L512_ERROR_COMMAND;
  }

  /* Transmission of the data */
  if (HAL_QSPI_Transmit(Ctx, Password, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L512_ERROR_TRANSMIT;
  }

  return MX25L512_OK;
}

/**
  * @brief  Read password register value. 8 Byte, SPI only.
  *         SPI; 1-0-1
  * @param  Ctx Component object pointer
  * @param  Mode Interface mode  
  * @param  Password 8 Byte password pointer.
  * @retval QSPI memory status
  */
int32_t MX25L512_ReadPasswordRegister(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode, uint8_t *Password)
{
  QSPI_CommandTypeDef s_command;

  /* Initialize the Read password register command */
  s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction       = MX25L512_READ_PASSWORD_REGISTER_CMD;
  s_command.AddressMode       = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DummyCycles       = 0;
  s_command.DataMode          = QSPI_DATA_1_LINE;
  s_command.NbData            = 8;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  /* Configure the command */
  if (HAL_QSPI_Command(Ctx, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L512_ERROR_COMMAND;
  }

  /* Reception of the data */
  if (HAL_QSPI_Receive(Ctx, Password, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L512_ERROR_RECEIVE;
  }

  return MX25L512_OK;
}

/**
  * @brief  Password unlock. Always write 8 byte data.
  *         SPI; 1-0-1
  * @param  Ctx Component object pointer
  * @param  Mode Interface mode  
  * @param  Password Value for write to status register.
  * @retval QSPI memory status
  */
int32_t MX25L512_PasswordUnlock(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode, uint8_t *Password)
{
  QSPI_CommandTypeDef s_command;

  /* Initialize the Password unlock command */
  s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction       = MX25L512_PASSWORD_UNLOCK_CMD;
  s_command.AddressMode       = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DummyCycles       = 0;
  s_command.DataMode          = QSPI_DATA_1_LINE;
  s_command.NbData            = 8;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  /* Configure the command */
  if (HAL_QSPI_Command(Ctx, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L512_ERROR_COMMAND;
  }

  /* Transmission of the data */
  if (HAL_QSPI_Transmit(Ctx, Password, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L512_ERROR_TRANSMIT;
  }

  return MX25L512_OK;
}

/**
  * @brief  Write SPB. SPB bit program.
  *         SPI; 1-1-0
  * @param  Ctx Component object pointer
  * @param  Mode Interface mode
  * @param  WriteAddr write address  
  * @retval QSPI memory status
  */
int32_t MX25L512_WriteSPB(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode, uint32_t WriteAddr)
{
  QSPI_CommandTypeDef s_command;

  /* Initialize the Write SPB command */
  s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction       = MX25L512_WRITE_SPB_CMD;
  s_command.AddressMode       = QSPI_ADDRESS_1_LINE;
  s_command.AddressSize       = QSPI_ADDRESS_32_BITS;
  s_command.Address           = WriteAddr;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DummyCycles       = 0;
  s_command.DataMode          = QSPI_DATA_NONE;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  /* Configure the command */
  if (HAL_QSPI_Command(Ctx, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L512_ERROR_COMMAND;
  }

  return MX25L512_OK;
}

/**
  * @brief  Erase SPB (ESSPB), All SPB bit erase.
  *         SPI; 1-0-0
  * @param  Ctx Component object pointer
  * @param  Mode Interface mode
  * @retval QSPI memory status
  */
int32_t MX25L512_EraseSPB(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode)
{
  QSPI_CommandTypeDef s_command;

  /* Initialize the All SPB bit erase command */
  s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction       = MX25L512_ERASE_SPB_CMD;
  s_command.AddressMode       = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DummyCycles       = 0;
  s_command.DataMode          = QSPI_DATA_NONE;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  /* Send the command */
  if (HAL_QSPI_Command(Ctx, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L512_ERROR_COMMAND;
  }

  return MX25L512_OK;
}

/**
  * @brief  Read SPB status.
  *         SPI; 1-1-1
  * @param  Ctx Component object pointer
  * @param  Mode Interface mode
  * @param  ReadAddr read address
  * @param  SPBStatus pointer to SPB status value   
  * @retval QSPI memory status
  */
int32_t MX25L512_ReadSPBStatus(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode, uint32_t ReadAddr, uint8_t *SPBStatus)
{
  QSPI_CommandTypeDef s_command;

  /* Initialize the reading of SPB lock status command */
  s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction       = MX25L512_READ_SPB_STATUS_CMD;
  s_command.AddressMode       = QSPI_ADDRESS_1_LINE;
  s_command.AddressSize       = QSPI_ADDRESS_32_BITS;
  s_command.Address           = ReadAddr;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DummyCycles       = 0;
  s_command.DataMode          = QSPI_DATA_1_LINE;
  s_command.NbData            = 1;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  /* Configure the command */
  if (HAL_QSPI_Command(Ctx, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L512_ERROR_COMMAND;
  }

  /* Reception of the data */
  if (HAL_QSPI_Receive(Ctx, SPBStatus, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L512_ERROR_RECEIVE;
  }

  return MX25L512_OK;
}

/**
  * @brief  SPB lock set
  *         SPI; 1-0-0
  * @param  Ctx Component object pointer
  * @param  Mode Interface mode  
  * @retval QSPI memory status
   */
int32_t MX25L512_SPBLockSet(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode)
{
  QSPI_CommandTypeDef s_command;

  /* Initialize the SPB lock set command */
  s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction       = MX25L512_SPB_LOCK_SET_CMD;
  s_command.AddressMode       = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DummyCycles       = 0;
  s_command.DataMode          = QSPI_DATA_NONE;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  /* Send the command */
  if (HAL_QSPI_Command(Ctx, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L512_ERROR_COMMAND;
  }

  return MX25L512_OK;
}

/**
  * @brief  Read SPB lock register value.
  *         SPI; 1-0-1
  * @param  Ctx Component object pointer
  * @param  Mode Interface mode
  * @param  SPBRegister pointer to SPBRegister value   
  * @retval QSPI memory status
  */
int32_t MX25L512_ReadSPBLockRegister(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode, uint8_t *SPBRegister)
{
  QSPI_CommandTypeDef s_command;

  /* Initialize the reading of SPB lock register command */
  s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction       = MX25L512_READ_SPB_LOCK_REGISTER_CMD;
  s_command.AddressMode       = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DummyCycles       = 0;
  s_command.DataMode          = QSPI_DATA_1_LINE;
  s_command.NbData            = 1;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  /* Configure the command */
  if (HAL_QSPI_Command(Ctx, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L512_ERROR_COMMAND;
  }

  /* Reception of the data */
  if (HAL_QSPI_Receive(Ctx, SPBRegister, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L512_ERROR_RECEIVE;
  }

  return MX25L512_OK;
}

/**
  * @brief  Write DPB register.
  *         SPI; 1-1-1
  * @param  Ctx Component object pointer
  * @param  Mode Interface mode 
  * @param  WriteAddr write address
  * @param  DPBRegister value to write to DPBRegister    
  * @retval QSPI memory status
  */
int32_t MX25L512_WriteDPBRegister(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode, uint32_t WriteAddr, uint8_t DPBStatus)
{
  QSPI_CommandTypeDef s_command;
  uint8_t reg = DPBStatus;

  /* Initialize the Write DPB register command */
  s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction       = MX25L512_WRITE_DPB_REGISTER_CMD;
  s_command.AddressMode       = QSPI_ADDRESS_1_LINE;
  s_command.AddressSize       = QSPI_ADDRESS_32_BITS;
  s_command.Address           = WriteAddr;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DummyCycles       = 0;
  s_command.DataMode          = QSPI_DATA_1_LINE;
  s_command.NbData            = 1;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  /* Configure the command */
  if (HAL_QSPI_Command(Ctx, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L512_ERROR_COMMAND;
  }

  /* Transmission of the data */
  if (HAL_QSPI_Transmit(Ctx, &reg, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L512_ERROR_TRANSMIT;
  }

  return MX25L512_OK;
}

/**
  * @brief  Read DPB register, 1 Byte.
  *         SPI; 1-1-1
  * @param  Ctx Component object pointer
  * @param  Mode Interface mode 
  * @param  ReadAddr read address
  * @param  DPBRegister pointer to DPBRegister value  
  * @retval QSPI memory status
  */
int32_t MX25L512_ReadDPBRegister(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode, uint32_t ReadAddr, uint8_t *DPBRegister)
{
  QSPI_CommandTypeDef s_command;

  /* Initialize the Read DPB register command */
  s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction       = MX25L512_READ_DPB_REGISTER_CMD;
  s_command.AddressMode       = QSPI_ADDRESS_1_LINE;
  s_command.AddressSize       = QSPI_ADDRESS_32_BITS;
  s_command.Address           = ReadAddr;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode          = QSPI_DATA_1_LINE;
  s_command.DummyCycles       = 0;
  s_command.NbData            = 1;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  /* Configure the command */
  if (HAL_QSPI_Command(Ctx, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L512_ERROR_COMMAND;
  }

  /* Reception of the data */
  if (HAL_QSPI_Receive(Ctx, DPBRegister, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L512_ERROR_RECEIVE;
  }

  return MX25L512_OK;
}


/* Reset Commands *************************************************************/
/**
  * @brief  No Operation command for exit from reset enable mode
  *         SPI/QPI; 1-0-0, 4-0-0
  * @param  Ctx Component object pointer
  * @param  Mode Interface mode
  * @retval QSPI memory status
  */
int32_t MX25L512_NoOperation(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode)
{
  QSPI_CommandTypeDef s_command;

  /* Initialize the reset enable command */
  s_command.InstructionMode   = (Mode == MX25L512_QPI_MODE) ? QSPI_INSTRUCTION_4_LINES : QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction       = MX25L512_NO_OPERATION_CMD;
  s_command.AddressMode       = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DummyCycles       = 0;
  s_command.DataMode          = QSPI_DATA_NONE;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  /* Send the command */
  if (HAL_QSPI_Command(Ctx, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L512_ERROR_COMMAND;
  }

  return MX25L512_OK;
}

/**
  * @brief  Flash reset enable command
  *         SPI/QPI; 1-0-0, 4-0-0
  * @param  Ctx Component object pointer
  * @param  Mode Interface mode
  * @retval QSPI memory status
  */
int32_t MX25L512_ResetEnable(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode)
{
  QSPI_CommandTypeDef s_command;

  /* Initialize the reset enable command */
  s_command.InstructionMode   = (Mode == MX25L512_QPI_MODE) ? QSPI_INSTRUCTION_4_LINES : QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction       = MX25L512_RESET_ENABLE_CMD;
  s_command.AddressMode       = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DummyCycles       = 0;
  s_command.DataMode          = QSPI_DATA_NONE;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  /* Send the command */
  if (HAL_QSPI_Command(Ctx, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L512_ERROR_COMMAND;
  }

  return MX25L512_OK;
}

/**
  * @brief  Flash reset memory command
  *         SPI/QPI; 1-0-0, 4-0-0
  * @param  Ctx Component object pointer
  * @param  Mode Interface mode
  * @retval QSPI memory status
  */
int32_t MX25L512_ResetMemory(QSPI_HandleTypeDef *Ctx, MX25L512_Interface_t Mode)
{
  QSPI_CommandTypeDef s_command;

  /* Initialize the reset enable command */
  s_command.InstructionMode   = (Mode == MX25L512_QPI_MODE) ? QSPI_INSTRUCTION_4_LINES : QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction       = MX25L512_RESET_MEMORY_CMD;
  s_command.AddressMode       = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DummyCycles       = 0;
  s_command.DataMode          = QSPI_DATA_NONE;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  /* Send the command */
  if (HAL_QSPI_Command(Ctx, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L512_ERROR_COMMAND;
  }

  return MX25L512_OK;
}

/**
  * @}
  */

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
