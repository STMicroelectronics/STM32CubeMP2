/**
  ******************************************************************************
  * @file    NonSecure/Src/main.c
  * @author  MCD Application Team
  * @brief   Main program body (non-secure)
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stdbool.h"
#include "stdio.h"
#include "copro_sync.h"
#include "config_tfm.h"
#include "psa/crypto.h"
/** @addtogroup STM32MP2xx_Application
  * @{
  */

/** @addtogroup SPE_NSCubeProjects
  * @{
  */


/* Callbacks prototypes */
/* Global variables ----------------------------------------------------------*/
IPCC_HandleTypeDef   hipcc;
/* External function prototypes ----------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* Comment below macro to perform
 * multi-part crypto operation only */
#define TFM_CRYPTO_SINGLE_SHOT

/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void Error_Handler      (void);
static void MX_IPCC_Init       (void);

static void psa_aead_encrypt_decrypt(const psa_key_type_t   key_type,
                                     const psa_algorithm_t  alg,
                                     const uint8_t         *key,
                                     size_t                 key_bits);

/**
  * @brief  Main program
  * @retval None
  */
int main(void)
{
  HAL_Init();

  /* Initialize Display destination */
#if defined(__VALID_OUTPUT_TERMINAL_IO__) && defined (__GNUC__)
  initialise_monitor_handles();
#elif defined(__VALID_OUTPUT_UART_IO__)
  COM_InitTypeDef COM_Conf;

  COM_Conf.BaudRate   = 115200;
  COM_Conf.HwFlowCtl  = COM_HWCONTROL_NONE;
  COM_Conf.Parity     = COM_PARITY_NONE;
  COM_Conf.StopBits   = COM_STOPBITS_1;
  COM_Conf.WordLength = COM_WORDLENGTH_8B;

  /* Initialize and select COM1 which is the COM port associated with current Core */
  BSP_COM_Init(COM_VCP_CM33, &COM_Conf);
  BSP_COM_SelectLogPort(COM_VCP_CM33);
#endif  /* __VALID_OUTPUT_TERMINAL_IO__ or __VALID_OUTPUT_UART_IO__ */

  /* Initialize led */
  BSP_LED_Init(LED3);
  BSP_LED_On(LED3);

  if(!IS_DEVELOPER_BOOT_MODE())
  {
    /* IPCC initialization */
    MX_IPCC_Init();
    /*Corpo Sync Initialization*/
    CoproSync_Init();
  }

  /* Add your non-secure example code here */

  printf("\r\n***** TFM_Crypto M33 NonSecure *****\r\n");

  printf("\033[1;34mSTM32Cube FW version: v%ld.%ld.%ld-rc%ld \033[0m\r\n",
            ((HAL_GetHalVersion() >> 24) & 0x000000FF),    /* Main Version */
            ((HAL_GetHalVersion() >> 16) & 0x000000FF),    /* SubVersion-1 */
            ((HAL_GetHalVersion() >> 8 ) & 0x000000FF),    /* SubVersion-2 */
            ( HAL_GetHalVersion()        & 0x000000FF));   /* RC Version   */

  printf("PSA Crypto example started.\r\n");
  printf("PSA Crypto API Version %d.%d\r\n",
          PSA_CRYPTO_API_VERSION_MAJOR, PSA_CRYPTO_API_VERSION_MINOR);

  psa_aead_encrypt_decrypt(PSA_KEY_TYPE_AES, PSA_ALG_CCM,
                           test_key_128, BIT_SIZE_TEST_KEY);
  /* psa_aead_encrypt_decrypt will not return in case of any failure */
  printf("PSA Crypto example executed successfully.\r\n");

  printf("*****    End of Application    *****\r\n");

  while (1)
  {
    BSP_LED_Toggle(LED3);
    HAL_Delay(1000);
  }
}


static void psa_aead_encrypt_decrypt(const psa_key_type_t   key_type,
                                     const psa_algorithm_t  alg,
                                     const uint8_t         *key,
                                     size_t                 key_bits)
{
  psa_status_t          status        = PSA_SUCCESS;
  uint8_t               error_flag    = 0;
  psa_aead_operation_t  encop         = psa_aead_operation_init();
  psa_aead_operation_t  decop         = psa_aead_operation_init();
  psa_key_id_t          key_id_local  = PSA_KEY_ID_NULL;
  const size_t          nonce_length  = 12;
  const uint8_t         nonce[]       = "01234567890";
  const uint8_t         plain_text[PLAIN_DATA_SIZE_AEAD] =
                        "Plain text message used here to demonstrate encryption & decryption";
  const uint8_t         associated_data[] =
                        "Associated data required for the algorithm";
  uint8_t               decrypted_data[PLAIN_DATA_SIZE_AEAD]     = {0};
  uint8_t               encrypted_data[ENC_DEC_BUFFER_SIZE_AEAD] = {0};
  size_t                encrypted_data_length                    =  0;
  size_t                decrypted_data_length                    =  0;
  size_t                total_output_length                      =  0;
  size_t                total_encrypted_length                   =  0;
  uint32_t              comp_result                              =  0xFF;
  psa_key_attributes_t  key_attributes                           =  psa_key_attributes_init();
  psa_key_attributes_t  retrieved_attributes                     =  psa_key_attributes_init();
  psa_key_usage_t       usage                                    = (PSA_KEY_USAGE_ENCRYPT |
                                                                    PSA_KEY_USAGE_DECRYPT);
#ifdef TFM_CRYPTO_SINGLE_SHOT
  uint8_t               encrypted_data_single_shot[ENC_DEC_BUFFER_SIZE_AEAD] = {0};
#endif /* TFM_CRYPTO_SINGLE_SHOT */

  /* Variables required for Multi-Part operations */
  uint8_t  *tag        = &encrypted_data[PLAIN_DATA_SIZE_AEAD];
  size_t    tag_length = 0;
  size_t    tag_size   = PSA_AEAD_TAG_LENGTH(key_type,
                                             psa_get_key_bits(&key_attributes),
                                             alg);

  if (sizeof(encop) != sizeof(uint32_t))
  {
    printf("Error : The test client is not picking up the client side definitions\r\n");
    Error_Handler();
  }

  /* Setup the key policy */
  psa_set_key_usage_flags(&key_attributes, usage);
  psa_set_key_algorithm(&key_attributes, alg);
  psa_set_key_type(&key_attributes, key_type);

  /* Import a key */
  status = psa_import_key(&key_attributes, key, PSA_BITS_TO_BYTES(key_bits),
                            &key_id_local);

  if (status != PSA_SUCCESS)
  {
    printf("Error : Failed importing a key\r\n");
    Error_Handler();
  }

  status = psa_get_key_attributes(key_id_local, &retrieved_attributes);
  if (status != PSA_SUCCESS)
  {
    printf("Error : Failed getting key metadata\r\n");
    error_flag = 1;
    goto destroy_key_aead;
  }

  if (psa_get_key_bits(&retrieved_attributes) != key_bits)
  {
    printf("Error : The number of key bits is different from expected\r\n");
    error_flag = 1;
    goto destroy_key_aead;
  }

  if (psa_get_key_type(&retrieved_attributes) != key_type)
  {
    printf("Error : The type of the key is different from expected\r\n");
    error_flag = 1;
    goto destroy_key_aead;
  }

  psa_reset_key_attributes(&retrieved_attributes);

#ifdef TFM_CRYPTO_SINGLE_SHOT
  /* Perform AEAD encryption */
  printf("Performing AEAD encryption (Single-Shot)\r\n");
  status = psa_aead_encrypt(key_id_local, alg      ,
                            nonce, nonce_length    ,
                            associated_data        ,
                            sizeof(associated_data),
                            plain_text             ,
                            sizeof(plain_text)     ,
                            encrypted_data         ,
                            sizeof(encrypted_data) ,
                            &encrypted_data_length);

  if (status != PSA_SUCCESS)
  {
    if (status == PSA_ERROR_NOT_SUPPORTED)
    {
      printf("Error : Algorithm NOT SUPPORTED by the implementation\r\n");
      error_flag = 1;
      goto destroy_key_aead;
    }
    printf("Error : Failed while performing AEAD encryption\r\n");
    error_flag = 1;
    goto destroy_key_aead;
  }

  if (encrypted_data_length
      != PSA_AEAD_ENCRYPT_OUTPUT_SIZE(key_type, alg, sizeof(plain_text)))
  {
    printf("Error : Encrypted data length is different than expected\r\n");
    error_flag = 1;
    goto destroy_key_aead;
  }

  if (encrypted_data_length > ENC_DEC_BUFFER_SIZE_AEAD)
  {
    printf("Error : Encrypted data length is above the maximum expected value\r\n");
    error_flag = 1;
    goto destroy_key_aead;
  }

  /* Store a copy of the encrypted data for later checking it against
  * Multi-Part results */
  memcpy(encrypted_data_single_shot, encrypted_data, encrypted_data_length);

  /* Perform AEAD decryption */
  printf("Performing AEAD decryption (Single-Shot)\r\n");
  status = psa_aead_decrypt(key_id_local, alg      ,
                            nonce, nonce_length    ,
                            associated_data        ,
                            sizeof(associated_data),
                            encrypted_data         ,
                            encrypted_data_length  ,
                            decrypted_data         ,
                            sizeof(decrypted_data) ,
                            &decrypted_data_length);

  if (status != PSA_SUCCESS)
  {
    if (status == PSA_ERROR_NOT_SUPPORTED)
    {
      printf("Error : Algorithm NOT SUPPORTED by the implementation\r\n");
    }
    else
    {
      printf("Error : Failed while performing AEAD decryption\r\n");
    }

    error_flag = 1;
    goto destroy_key_aead;
  }

  if (sizeof(plain_text) != decrypted_data_length)
  {
    printf("Error : Decrypted data length is different from plain text\r\n");
    error_flag = 1;
    goto destroy_key_aead;
  }

  /* Check that the decrypted data is the same as the original data */
  comp_result = memcmp(plain_text, decrypted_data, sizeof(plain_text));
  if (comp_result != 0)
  {
    printf("Error : Decrypted data doesn't match with plain text\r\n");
    error_flag = 1;
    goto destroy_key_aead;
  }
  else
  {
    printf(" --> Decrypted data is same as the original data (Single-Shot)\r\n");
  }
#endif /* TFM_CRYPTO_SINGLE_SHOT */

  /* Setup the encryption object */
  status = psa_aead_encrypt_setup(&encop, key_id_local, alg);
  if (status != PSA_SUCCESS)
  {
   /* Implementations using the PSA Crypto Driver APIs, that don't
    * support Multi-Part API flows, will return PSA_ERROR_NOT_SUPPORTED
    * when calling psa_aead_encrypt_setup(). In this case, it's fine
    * just to skip the Multi-Part APIs flow from this point on */
    if (status == PSA_ERROR_NOT_SUPPORTED)
    {
      printf("Error : psa_aead_encrypt_setup(): Algorithm NOT SUPPORTED by"\
             " the implementation - skip multi-part API flow\r\n");
    }
    else
    {
      printf("Error : Failed while setting up encryption object\r\n");
    }

    error_flag = 1;
    goto destroy_key_aead;
  }

  /* Set lengths */
  status = psa_aead_set_lengths(&encop,
                                 sizeof(associated_data),
                                 sizeof(plain_text));
  if (status != PSA_SUCCESS)
  {
   /* Implementations using the mbed TLS _ALT APIs, that don't support
    * Multi-Part API flows in CCM mode, will return PSA_ERROR_NOT_SUPPORTED
    * when calling psa_aead_set_lengths(). In this case, it's fine just
    * to skip the Multi-Part APIs flow from this point on */
    if (PSA_ALG_AEAD_WITH_DEFAULT_LENGTH_TAG(alg) == PSA_ALG_CCM
        && status == PSA_ERROR_NOT_SUPPORTED)
    {
      printf("Error : psa_aead_set_lengths(): Algorithm NOT SUPPORTED by the "\
             "implementation - skip Multi-Part API flow\r\n");
    }
    else
    {
      printf("Error : Failed while setting lengths\r\n");
    }

    status = psa_aead_abort(&encop);
    if (status != PSA_SUCCESS)
    {
      printf("Error : Failed aborting the operation\r\n");
    }

    error_flag = 1;
    goto destroy_key_aead;
  }

  /* Set nonce */
  status = psa_aead_set_nonce(&encop, nonce, nonce_length);
  if (status != PSA_SUCCESS)
  {
   /* Implementations using the mbed TLS _ALT APIs, that don't support
    * Multi-Part API flows in GCM, will return PSA_ERROR_NOT_SUPPORTED when
    * calling psa_aead_set_nonce(). In this case, it's fine just to skip
    * the Multi-Part APIs flow from this point on */
    if (PSA_ALG_AEAD_WITH_DEFAULT_LENGTH_TAG(alg) == PSA_ALG_GCM
        && status == PSA_ERROR_NOT_SUPPORTED)
    {
      printf("Error : psa_aead_set_nonce(): Algorithm NOT SUPPORTED by the "\
             "implementation - skip Multi-Part API flow\r\n");
    }
    else
    {
      printf("Error : Failed while setting nonce\r\n");
    }
    status = psa_aead_abort(&encop);
    if (status != PSA_SUCCESS)
    {
      printf("Error : Failed aborting the operation\r\n");
    }

    error_flag = 1;
    goto destroy_key_aead;
  }

  /* Update additional data */
  status = psa_aead_update_ad(&encop,
                               associated_data,
                               sizeof(associated_data));

  if (status != PSA_SUCCESS)
  {
    printf("Error : Failed while updating additional data\r\n");
    status = psa_aead_abort(&encop);
    if (status != PSA_SUCCESS)
    {
      printf("Error : Failed aborting the operation\r\n");
    }

    error_flag = 1;
    goto destroy_key_aead;
  }

  /* Encrypt one chunk of information at a time */
  printf("Performing AEAD Encryption (Multi-Part)\r\n");
  for (size_t i = 0; i <= sizeof(plain_text)/BYTE_SIZE_CHUNK; i++)
  {
    size_t size_to_encrypt =
                    (sizeof(plain_text) - i*BYTE_SIZE_CHUNK) > BYTE_SIZE_CHUNK ?
                     BYTE_SIZE_CHUNK : (sizeof(plain_text) - i*BYTE_SIZE_CHUNK);

    status = psa_aead_update(&encop,
                              plain_text + i*BYTE_SIZE_CHUNK,
                              size_to_encrypt,
                              encrypted_data + total_encrypted_length,
                              sizeof(encrypted_data) -
                              total_encrypted_length,
                              &encrypted_data_length);

    if (status != PSA_SUCCESS)
    {
      printf("Error : Failed while encrypting one chunk of information\r\n");
      status = psa_aead_abort(&encop);
      if (status != PSA_SUCCESS)
      {
        printf("Error : Failed aborting the operation\r\n");
      }

      error_flag = 1;
      goto destroy_key_aead;
    }
    total_encrypted_length += encrypted_data_length;
  }

  /* Finish the aead operation */
  status = psa_aead_finish(&encop,
                            encrypted_data + total_encrypted_length,
                            sizeof(encrypted_data) - total_encrypted_length,
                            &encrypted_data_length,
                            tag,
                            tag_size,
                            &tag_length);
  if (status != PSA_SUCCESS)
  {
    printf("Error : Failed while finalizing the AEAD operation\r\n");
    status = psa_aead_abort(&encop);
    if (status != PSA_SUCCESS)
    {
      printf("Error : Failed aborting the operation\r\n");
    }

    error_flag = 1;
    goto destroy_key_aead;
  }
  total_encrypted_length += encrypted_data_length;

#ifdef TFM_CRYPTO_SINGLE_SHOT
  /* Compare tag between Single-Shot and Multi-Part case */
  comp_result = memcmp(&encrypted_data_single_shot[total_encrypted_length],
                        tag, tag_length);
  if (comp_result != 0)
  {
    printf("Error : Single-Shot tag does not match with Multi-Part tag\r\n");
    error_flag = 1;
    goto destroy_key_aead;
  }

  /* Compare encrypted data between Single-Shot and Multi-Part case */
  comp_result = memcmp(encrypted_data_single_shot,
                       encrypted_data, total_encrypted_length);
  if (comp_result != 0)
  {
    printf("Error : Single-Shot encrypted data does not match with Multi-Part encrypted data\r\n");
    error_flag = 1;
    goto destroy_key_aead;
  }
  else
  {
    printf(" --> Single-Shot encrypted data matches with Multi-Part encrypted data\r\n");
  }
#endif /* TFM_CRYPTO_SINGLE_SHOT */

  /* Setup up the decryption object */
  status = psa_aead_decrypt_setup(&decop, key_id_local, alg);
  if (status != PSA_SUCCESS)
  {
    printf("Error : Failed while setting up AEAD object\r\n");
    error_flag = 1;
    goto destroy_key_aead;
  }

  /* Set lengths */
  status = psa_aead_set_lengths(&decop,
                                sizeof(associated_data),
                                sizeof(plain_text));
  if (status != PSA_SUCCESS)
  {
    printf("Error : Failed while setting lengths\r\n");
    status = psa_aead_abort(&decop);
    if (status != PSA_SUCCESS)
    {
      printf("Error : Failed aborting the operation\r\n");
    }

    error_flag = 1;
    goto destroy_key_aead;
  }

  /* Set nonce */
  status = psa_aead_set_nonce(&decop, nonce, nonce_length);
  if (status != PSA_SUCCESS)
  {
    printf("Error : Failed while setting nonce\r\n");
    status = psa_aead_abort(&decop);
    if (status != PSA_SUCCESS)
    {
      printf("Error : Failed aborting the operation\r\n");
    }

    error_flag = 1;
    goto destroy_key_aead;
  }

  /* Update additional data */
  status = psa_aead_update_ad(&decop,
                              associated_data,
                              sizeof(associated_data));
  if (status != PSA_SUCCESS)
  {
    printf("Error : Failed while updating additional data\r\n");
    status = psa_aead_abort(&decop);
    if (status != PSA_SUCCESS)
    {
      printf("Error : Failed aborting the operation\r\n");
    }

    error_flag = 1;
    goto destroy_key_aead;
  }

  /* Decrypt */
  printf("Performing AEAD Decryption (Multi-Part)\r\n");
  for (size_t i = 0; i <= total_encrypted_length/BYTE_SIZE_CHUNK; i++)
  {
    size_t size_to_decrypt =
          (total_encrypted_length - i*BYTE_SIZE_CHUNK) > BYTE_SIZE_CHUNK ?
           BYTE_SIZE_CHUNK : (total_encrypted_length - i*BYTE_SIZE_CHUNK);

    status = psa_aead_update(&decop,
                              encrypted_data + i*BYTE_SIZE_CHUNK,
                              size_to_decrypt,
                              decrypted_data + total_output_length,
                              sizeof(decrypted_data) - total_output_length,
                              &decrypted_data_length);

    if (status != PSA_SUCCESS)
    {
      printf("Error : Failed during decryption\r\n");
      status = psa_aead_abort(&decop);
      if (status != PSA_SUCCESS)
      {
        printf("Error : Failed aborting the operation\r\n");
      }

      error_flag = 1;
      goto destroy_key_aead;
    }

    total_output_length += decrypted_data_length;
  }

  /* Verify the AEAD operation */
  status = psa_aead_verify(&decop,
                            decrypted_data + total_output_length,
                            sizeof(decrypted_data) - total_output_length,
                            &decrypted_data_length,
                            tag,
                            tag_size);

  if (status != PSA_SUCCESS)
  {
    printf("Error : Failed verifying the AEAD operation\r\n");
    status = psa_aead_abort(&decop);
    if (status != PSA_SUCCESS)
    {
      printf("Error : Failed aborting the operation\r\n");
    }

    error_flag = 1;
    goto destroy_key_aead;
  }
  total_output_length += decrypted_data_length;

  if (total_output_length != sizeof(plain_text))
  {
    printf("Error : Total decrypted length does not match size of plain text\r\n");
    status = psa_aead_abort(&decop);
    if (status != PSA_SUCCESS)
    {
      printf("Error : Failed aborting the operation\r\n");
    }

    error_flag = 1;
    goto destroy_key_aead;
  }

  /* Check that the decrypted data is the same as the original data */
  comp_result = memcmp(plain_text, decrypted_data, sizeof(plain_text));
  if (comp_result != 0)
  {
    printf("Error : Decrypted data doesn't match with plain text (Multi-Part)\r\n");
    Error_Handler();
  }
  else
  {
    printf(" --> Decrypted data matches with plain text (Multi-Part)\r\n");
  }

destroy_key_aead:
  /* Destroy the key */
  status = psa_destroy_key(key_id_local);
  if (status != PSA_SUCCESS)
  {
    printf("Error : Failed destroying a key\r\n");
    Error_Handler();
  }

  if(error_flag != 0)
  {
    Error_Handler();
  }
}


/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
static void Error_Handler(void)
{
  printf("Error : Something went wrong\r\n");
  while(1)
  {
    /* Error if LED3 is ON */
    BSP_LED_On(LED3);
    while (1)
    {
      HAL_Delay(1000);
    }
  }
}

/**
  * @brief  Initialize the IPCC Peripheral
  * @retval None
  */
static void MX_IPCC_Init(void)
{

  hipcc.Instance = IPCC1;
  if (HAL_IPCC_Init(&hipcc) != HAL_OK)
  {
     Error_Handler();
  }
  /* IPCC interrupt Init */
  HAL_NVIC_SetPriority(IPCC1_RX_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(IPCC1_RX_IRQn);
}

/**
  * @brief  Callback from IPCC Interrupt Handler: Remote Processor asks local processor to shutdown
  * @param  hipcc IPCC handle
  * @param  ChannelIndex Channel number
  * @param  ChannelDir Channel direction
  * @retval None
  */
void CoproSync_ShutdownCb(IPCC_HandleTypeDef * hipcc, uint32_t ChannelIndex, IPCC_CHANNELDirTypeDef ChannelDir)
{
  BSP_COM_DeInit(COM_VCP_CM33);

  /* Deinitialize the LED3 */
  BSP_LED_DeInit(LED3);

  /* When ready, notify the remote processor that we can be shut down */
  HAL_IPCC_NotifyCPU(hipcc, ChannelIndex, IPCC_CHANNEL_DIR_RX);

  /* Wait for complete shutdown */
  while(1);
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file pointer to the source file name
  * @param  line assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: local_printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  printf("Error : Assert failed in file : %s at line %ld\r\n", file, line);

  /* Error if LED3 is ON */
  BSP_LED_On(LED3);
  while (1)
  {
    HAL_Delay(1000);
  }
}
#endif

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
