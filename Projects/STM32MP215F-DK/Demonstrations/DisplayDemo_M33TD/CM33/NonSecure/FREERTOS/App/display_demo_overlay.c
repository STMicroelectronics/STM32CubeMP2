/**
  ******************************************************************************
  * @file    display_demo_overlay.c
  * @author  MCD Application Team
  * @brief   Display Demo OLED overlay implementation.
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

/**
  * @addtogroup DisplayDemoOverlay
  * @{
  */

/* Includes ------------------------------------------------------------------*/
#include "display_demo_overlay.h"

#if defined(OVERLAY_FEATURE_ENABLED)

#include "display_task.h"
#include "main.h"
#include "remoteproc_task.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"

#include <stdio.h>

/* Private defines -----------------------------------------------------------*/
/** @brief Overlay refresh period in milliseconds. */
#define DISPLAY_DEMO_OVERLAY_REFRESH_PERIOD_MS 1000U

/* Private types -------------------------------------------------------------*/
/**
  * @brief Runtime state for the display demo overlay.
  */
typedef struct
{
  bool visible;
  osTimerId_t refresh_timer;
  RTC_HandleTypeDef rtc_handle;
} DisplayDemoOverlayState_t;

/* Private variables ---------------------------------------------------------*/
/** @brief Overlay state instance. */
static DisplayDemoOverlayState_t displayDemoOverlayState =
{
  .visible = false,
  .refresh_timer = NULL,
};

/** @brief RTOS timer attributes for overlay refresh. */
static const osTimerAttr_t displayDemoOverlayTimerAttr =
{
  .name = "DisplayDemoOverlayRefresh",
};

/** @brief Weekday strings used for RTC date formatting. */
static const char * const displayDemoOverlayDaysOfWeek[] =
{
  "Mon",
  "Tue",
  "Wed",
  "Thu",
  "Fri",
  "Sat",
  "Sun",
};

/* Private function prototypes -----------------------------------------------*/
static void DisplayDemoOverlay_RemoteProcCallback(RemoteProcState newState, void *context);
static void DisplayDemoOverlay_RefreshTimerCallback(void *argument);
static void DisplayDemoOverlay_WriteString(uint8_t x, uint8_t y, const char *text);
static const char *DisplayDemoOverlay_GetRemoteProcStatus(RemoteProcState state);
static const char *DisplayDemoOverlay_GetWeekdayString(uint8_t weekday);
static void DisplayDemoOverlay_GetDateTime(char *showDate,
                                           size_t showDateSize,
                                           char *showTime,
                                           size_t showTimeSize);
static void DisplayDemoOverlay_Render(void);

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Remote processor state callback used to trigger overlay refresh.
  * @param  newState Updated remote processor state.
  * @param  context User context registered with the callback.
  * @retval None
  */
static void DisplayDemoOverlay_RemoteProcCallback(RemoteProcState newState, void *context)
{
  const DisplayTaskCommand_t cmd =
  {
    .type = DISPLAY_CMD_UPDATE_OVERLAY,
    .param = DISPLAY_OVERLAY_UPDATE_FRAME_EVENT,
  };

  (void)newState;
  (void)context;
  (void)DisplayTask_PostCommand(&cmd);
}

/**
  * @brief  Periodic timer callback used to request overlay refresh.
  * @param  argument Unused timer argument.
  * @retval None
  */
static void DisplayDemoOverlay_RefreshTimerCallback(void *argument)
{
  const DisplayTaskCommand_t cmd =
  {
    .type = DISPLAY_CMD_UPDATE_OVERLAY,
    .param = DISPLAY_OVERLAY_UPDATE_FRAME_EVENT,
  };

  (void)argument;
  (void)DisplayTask_PostCommand(&cmd);
}

/**
  * @brief  Write one string on the OLED framebuffer.
  * @param  x Horizontal pixel position.
  * @param  y Vertical pixel position.
  * @param  text Null-terminated string to draw.
  * @retval None
  */
static void DisplayDemoOverlay_WriteString(uint8_t x, uint8_t y, const char *text)
{
  char textBuffer[32];

  if (text == NULL)
  {
    return;
  }

  (void)snprintf(textBuffer, sizeof(textBuffer), "%s", text);
  ssd1306_SetCursor(x, y);
  ssd1306_WriteString(textBuffer, Font_6x8, White);
}

/**
  * @brief  Convert remote processor state to displayable text.
  * @param  state Remote processor state.
  * @return Pointer to a static status string.
  */
static const char *DisplayDemoOverlay_GetRemoteProcStatus(RemoteProcState state)
{
  switch (state)
  {
  case REMOTEPROC_STATE_OFFLINE:
    return "offline";
  case REMOTEPROC_STATE_STARTING:
  case REMOTEPROC_STATE_RESUMING:
    return "started";
  case REMOTEPROC_STATE_RUNNING:
    return "running";
  case REMOTEPROC_STATE_SUSPENDING:
  case REMOTEPROC_STATE_SUSPENDED:
    return "suspended";
  case REMOTEPROC_STATE_STOPPING:
    return "offline";
  case REMOTEPROC_STATE_UNKNOWN:
  default:
    return "unknown";
  }
}

/**
  * @brief  Convert RTC weekday value to a displayable abbreviation.
  * @param  weekday RTC weekday value in the range 1..7.
  * @return Pointer to a static weekday string.
  */
static const char *DisplayDemoOverlay_GetWeekdayString(uint8_t weekday)
{
  if ((weekday < 1U) || (weekday > 7U))
  {
    return "Day";
  }

  return displayDemoOverlayDaysOfWeek[weekday - 1U];
}

/**
  * @brief  Read the current RTC date/time and format overlay strings.
  * @param  showDate Output buffer for the date string.
  * @param  showDateSize Size of the date buffer.
  * @param  showTime Output buffer for the time string.
  * @param  showTimeSize Size of the time buffer.
  * @retval None
  */
static void DisplayDemoOverlay_GetDateTime(char *showDate,
                                           size_t showDateSize,
                                           char *showTime,
                                           size_t showTimeSize)
{
  RTC_TimeTypeDef rtcTime = {0};
  RTC_DateTypeDef rtcDate = {0};
  const char *weekday;

  if ((showDate == NULL) || (showTime == NULL) || (showDateSize == 0U) || (showTimeSize == 0U))
  {
    return;
  }

  (void)snprintf(showDate, showDateSize, "%s", "Day dd/mm/yy");
  (void)snprintf(showTime, showTimeSize, "%s", "hh:mm:ss");

  if (HAL_RTC_GetTime(&displayDemoOverlayState.rtc_handle, &rtcTime, RTC_FORMAT_BIN) != HAL_OK)
  {
    return;
  }

  if (HAL_RTC_GetDate(&displayDemoOverlayState.rtc_handle, &rtcDate, RTC_FORMAT_BIN) != HAL_OK)
  {
    return;
  }

  weekday = DisplayDemoOverlay_GetWeekdayString(rtcDate.WeekDay);
  (void)snprintf(showTime,
                 showTimeSize,
                 "%02u:%02u:%02u",
                 rtcTime.Hours,
                 rtcTime.Minutes,
                 rtcTime.Seconds);
  (void)snprintf(showDate,
                 showDateSize,
                 "%s %02u/%02u/%02u",
                 weekday,
                 rtcDate.Date,
                 rtcDate.Month,
                 rtcDate.Year);
}

/**
  * @brief  Render the complete overlay content into the OLED framebuffer.
  * @retval None
  */
static void DisplayDemoOverlay_Render(void)
{
  char showDate[20] = "Day dd/mm/yy";
  char showTime[20] = "hh:mm:ss";
  char displayStr[32];
  const char *a35Status = DisplayDemoOverlay_GetRemoteProcStatus(RemoteProcTask_GetState());

  DisplayDemoOverlay_GetDateTime(showDate,
                                 sizeof(showDate),
                                 showTime,
                                 sizeof(showTime));

  ssd1306_Fill(Black);
  DisplayDemoOverlay_WriteString(8, 2, "M33TD Display Demo");
  DisplayDemoOverlay_WriteString(25, 14, showDate);
  DisplayDemoOverlay_WriteString(30, 24, showTime);
  DisplayDemoOverlay_WriteString(20, 38, "M33 : running");
  (void)snprintf(displayStr, sizeof(displayStr), "A35 : %s", a35Status);
  DisplayDemoOverlay_WriteString(20, 48, displayStr);

  ssd1306_UpdateScreen();
}

/* Exported functions --------------------------------------------------------*/
/**
  * @brief  Initialize overlay resources.
  * @retval 0 on success, negative value on failure.
  */
int DisplayDemoOverlay_Init(void)
{
  displayDemoOverlayState.rtc_handle.Instance = RTC;

  if (displayDemoOverlayState.refresh_timer == NULL)
  {
    displayDemoOverlayState.refresh_timer = osTimerNew(DisplayDemoOverlay_RefreshTimerCallback,
                                                       osTimerPeriodic,
                                                       NULL,
                                                       &displayDemoOverlayTimerAttr);
    if (displayDemoOverlayState.refresh_timer == NULL)
    {
      return -1;
    }
  }

  displayDemoOverlayState.visible = false;

  if (RemoteProcTask_RegisterCallback(DisplayDemoOverlay_RemoteProcCallback, NULL) != HAL_OK)
  {
    if (displayDemoOverlayState.refresh_timer != NULL)
    {
      (void)osTimerDelete(displayDemoOverlayState.refresh_timer);
      displayDemoOverlayState.refresh_timer = NULL;
    }

    return -1;
  }

  return 0;
}

/**
  * @brief  Deinitialize overlay resources.
  * @retval None
  */
void DisplayDemoOverlay_Deinit(void)
{
  (void)RemoteProcTask_UnregisterCallback(DisplayDemoOverlay_RemoteProcCallback);

  if (displayDemoOverlayState.refresh_timer != NULL)
  {
    (void)osTimerStop(displayDemoOverlayState.refresh_timer);
    (void)osTimerDelete(displayDemoOverlayState.refresh_timer);
    displayDemoOverlayState.refresh_timer = NULL;
  }

  displayDemoOverlayState.visible = false;
}

/**
  * @brief  Show the overlay and start periodic refresh.
  * @retval None
  */
void DisplayDemoOverlay_Show(void)
{
  displayDemoOverlayState.visible = true;
  DisplayDemoOverlay_Render();

  if (displayDemoOverlayState.refresh_timer != NULL)
  {
    (void)osTimerStart(displayDemoOverlayState.refresh_timer,
                       DISPLAY_DEMO_OVERLAY_REFRESH_PERIOD_MS);
  }
}

/**
  * @brief  Refresh the overlay if it is currently visible.
  * @retval None
  */
void DisplayDemoOverlay_Update(void)
{
  if (!displayDemoOverlayState.visible)
  {
    return;
  }

  DisplayDemoOverlay_Render();
}

/**
  * @brief  Hide the overlay and clear the OLED framebuffer.
  * @retval None
  */
void DisplayDemoOverlay_Hide(void)
{
  if (displayDemoOverlayState.refresh_timer != NULL)
  {
    (void)osTimerStop(displayDemoOverlayState.refresh_timer);
  }

  displayDemoOverlayState.visible = false;
  ssd1306_Fill(Black);
  ssd1306_UpdateScreen();
}

#endif /* OVERLAY_FEATURE_ENABLED */

/**
  * @}
  */
