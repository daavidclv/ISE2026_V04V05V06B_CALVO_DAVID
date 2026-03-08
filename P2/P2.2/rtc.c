
#include "stm32f4xx_hal.h"
#include "rtc.h"
#include "main.h"
#include "stm32f4xx_hal_rtc_ex.h"

RTC_HandleTypeDef hrtc;
char aShowDate[80];
char aShowTime[80];
extern osThreadId_t TID_Alarma;		

void RTC_Init(void){
	
	RCC_OscInitTypeDef        RCC_OscInitStruct;
  RCC_PeriphCLKInitTypeDef  PeriphClkInitStruct;

  /*##-1- Enables the PWR Clock and Enables access to the backup domain ###################################*/
  /* To change the source clock of the RTC feature (LSE, LSI), You have to:
     - Enable the power clock using __HAL_RCC_PWR_CLK_ENABLE()
     - Enable write access using HAL_PWR_EnableBkUpAccess() function before to 
       configure the RTC clock source (to be done once after reset).
     - Reset the Back up Domain using __HAL_RCC_BACKUPRESET_FORCE() and 
       __HAL_RCC_BACKUPRESET_RELEASE().
     - Configure the needed RTc clock source */
  __HAL_RCC_PWR_CLK_ENABLE();
  HAL_PWR_EnableBkUpAccess();

  
  /*##-2- Configure LSE as RTC clock source ###################################*/
  RCC_OscInitStruct.OscillatorType =  RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
	
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  { 
    while(1);
  }
  
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  if(HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  { 
    while(1);
  }
  
  /*##-3- Enable RTC peripheral Clocks #######################################*/
  /* Enable RTC Clock */
  __HAL_RCC_RTC_ENABLE();
  /*##-1- Configure the RTC peripheral #######################################*/
  /* Configure RTC prescaler and RTC data registers */
  /* RTC configured as follows:
      - Hour Format    = Format 24
      - Asynch Prediv  = Value according to source clock
      - Synch Prediv   = Value according to source clock
      - OutPut         = Output Disable
      - OutPutPolarity = High Polarity
      - OutPutType     = Open Drain */ 
  hrtc.Instance = RTC; 
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = RTC_ASYNCH_PREDIV;
  hrtc.Init.SynchPrediv = RTC_SYNCH_PREDIV;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  __HAL_RTC_RESET_HANDLE_STATE(&hrtc);
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    /* Initialization Error */
    while(1);
  }

	// HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, 0x32F2);
	
  /*##-2- Check if Data stored in BackUp register1: No Need to reconfigure RTC#*/
  /* Read the Back Up Register 1 Data */
//  if (HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1) != 0x32F2)
//  {
//    /* Configure RTC Calendar */
//    RTC_CalenConfig();
//  }
//	
	
	//  /*##-3- Writes a data in a RTC Backup data Register1 #######################*/
//  HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, 0x32F2);
	if (HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1) != 0x32F2)
  {
    /* Configure RTC Calendar */
    RTC_CalenConfig();
  } 
		// --- CONFIGURACIÓN DE LA ALARMA CADA MINUTO ---
  RTC_AlarmTypeDef sAlarm = {0};
  
  // Condición: Que los segundos sean exactamente 00
  sAlarm.AlarmTime.Seconds = 0x00; 
  
  // Enmascaramos (ignoramos) el día, la hora y los minutos para que suene todos los minutos
  sAlarm.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY | RTC_ALARMMASK_HOURS | RTC_ALARMMASK_MINUTES;
  sAlarm.Alarm = RTC_ALARM_A;
  HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BIN);

  // Activamos la interrupción en el procesador (NVIC)
  HAL_NVIC_SetPriority(RTC_Alarm_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);
}

// 1. El procesador salta FÍSICAMENTE aquí cuando el RTC llega al segundo 00
void RTC_Alarm_IRQHandler(void) {
    HAL_RTC_AlarmIRQHandler(&hrtc);
}

// 2. Esta callback es llamada automáticamente por la función anterior
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc) {
    // Le pegamos un toque al hilo para que despierte y parpadee el LED
        osThreadFlagsSet(TID_Alarma, 0x04); 
    }



RTC_DateTypeDef date;
RTC_TimeTypeDef time;

 void RTC_CalenConfig(void){
	
//  RTC_DateTypeDef date;
//  RTC_TimeTypeDef time;

  /*##-1- Configure the Date #################################################*/
  /* Set Date: Tuesday February 18th 2014 */
  date.Year = 23;
  date.Month = RTC_MONTH_FEBRUARY;
  date.Date = 11;
  date.WeekDay = RTC_WEEKDAY_TUESDAY;
  
  if(HAL_RTC_SetDate(&hrtc,&date,RTC_FORMAT_BIN) != HAL_OK)
  {
    /* Initialization Error */
    while(1);
  }

  /*##-2- Configure the Time #################################################*/
  /* Set Time: 02:00:00 */
  time.Hours = 9;
  time.Minutes = 10;
  time.Seconds = 8;
  time.TimeFormat = RTC_HOURFORMAT12_AM;
  time.DayLightSaving = RTC_DAYLIGHTSAVING_NONE ;
  time.StoreOperation = RTC_STOREOPERATION_RESET;

  if (HAL_RTC_SetTime(&hrtc, &time, RTC_FORMAT_BIN) != HAL_OK)
  {
    /* Initialization Error */
    while(1);
  }

  /*##-3- Writes a data in a RTC Backup data Register1 #######################*/
  HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, 0x32F2);
}


/**
  * @brief  Display the current time and date.
  * @param  showtime : pointer to buffer
  * @param  showdate : pointer to buffer
  * @retval None
  */

 void RTC_WEB(char *aShowTime, char *aShowDate)
{
//  RTC_DateTypeDef date;
//  RTC_TimeTypeDef time;

  /* Get the RTC current Time */
  HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);
  /* Get the RTC current Date */
  HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN);
  /* Display time Format : hh:mm:ss */
   sprintf(aShowTime, "%02d:%02d:%02d", time.Hours, time.Minutes, time.Seconds);
  /* Display date Format : mm-dd-yy */
   sprintf(aShowDate, "%02d/%02d/%02d", date.Date, date.Month, 2000 + date.Year);
}

