/**
  ******************************************************************************
  * @file    userapp_task.h
  * @author  MCD Application Team
  * @brief   Header file for User Application Task implementation.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

#ifndef USERAPP_TASK_H
#define USERAPP_TASK_H

#ifdef __cplusplus
extern "C" {
#endif

/* Exported functions prototypes ---------------------------------------------*/
/**
  * @brief  Initialize the User Application Task and its resources.
  * @retval None
  */
void UserAppTask_Init(void);

/**
  * @brief  De-initialize the User Application Task and release its resources.
  * @retval None
  */
void UserAppTask_DeInit(void);

#ifdef __cplusplus
}
#endif

#endif /* USERAPP_TASK_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/