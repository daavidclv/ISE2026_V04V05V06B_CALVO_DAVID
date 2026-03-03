/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2019 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    HTTP_Server.c
 * Purpose: HTTP Server example
 *----------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdbool.h>

#include "main.h"
//#include "led.h"
#include "LCD.h"
#include "rl_net.h"                     // Keil.MDK-Pro::Network:CORE
#include "LEDs.h"
#include "adc.h"
#include "stm32f4xx_hal.h"              // Keil::Device:STM32Cube HAL:Common
//#include "Board_LED.h"                  // ::Board Support:LED
//#include "Board_Buttons.h"              // ::Board Support:Buttons
//#include "Board_ADC.h"                  // ::Board Support:A/D Converter
//#include "Board_GLCD.h"                 // ::Board Support:Graphic LCD
//#include "GLCD_Config.h"                // Keil.MCBSTM32F400::Board Support:Graphic LCD

// Main stack size must be multiple of 8 Bytes
#define APP_MAIN_STK_SZ (1024U)
uint64_t app_main_stk[APP_MAIN_STK_SZ / 8];
const osThreadAttr_t app_main_attr = {
  .stack_mem  = &app_main_stk[0],
  .stack_size = sizeof(app_main_stk)
};

extern uint16_t AD_in          (uint32_t ch);
extern uint8_t  get_button     (void);
extern void     netDHCP_Notify (uint32_t if_num, uint8_t option, const uint8_t *val, uint32_t len);

extern bool LEDrun;
extern char lcd_text[2][20+1];


extern osThreadId_t TID_Display;
extern osThreadId_t TID_Led;




ADC_HandleTypeDef adchandle;

bool LEDrun;
char lcd_text[2][20+1] = { "LCD line 1",
                           "LCD line 2" };

/* Thread IDs */
osThreadId_t TID_Display;
osThreadId_t TID_Led;


/* Thread declarations */
static void BlinkLed (void *arg);
static void Display  (void *arg);

__NO_RETURN void app_main (void *arg);


  uint32_t voltage = 0;

/* Read analog inputs 
uint16_t AD_in (uint32_t ch) {
  int32_t val = 0;

  if (ch == 0) {
    ADC_StartConversion();
    while (ADC_ConversionDone () < 0);
    val = ADC_GetValue();
  }
  return ((uint16_t)val);
}
*/

uint16_t AD_in (uint32_t ch) {

  if (ch == 10) {	
		voltage=ADC_getVoltage(&adchandle , ch ); //gets raw values from channel 10->ADC123_IN10
  }
  return (voltage);
}



/* Read digital inputs */
uint8_t get_button (void) {
  //return ((uint8_t)Buttons_GetState ());
	return 0;
}

/* IP address change notification */
void netDHCP_Notify (uint32_t if_num, uint8_t option, const uint8_t *val, uint32_t len) {

  (void)if_num;
  (void)val;
  (void)len;

  if (option == NET_DHCP_OPTION_IP_ADDRESS) {
    /* IP address change, trigger LCD update */
    osThreadFlagsSet (TID_Display, 0x01);
  }
}

/*----------------------------------------------------------------------------
  Thread 'Display': LCD display handler
 *---------------------------------------------------------------------------*/

static __NO_RETURN void Display(void *arg) {

	LCD_reset();
	Iniciacion_LCD();
	reset_buffer();
  (void)arg;

  while(1) {
		osThreadFlagsWait(0x02,osFlagsWaitAny, osWaitForever);
		reset_buffer();
    escribe(lcd_text[0],lcd_text[1]);
		LCD_update();
  }

}

/*----------------------------------------------------------------------------
  Thread 'BlinkLed': Blink the LEDs on an eval board
 *---------------------------------------------------------------------------*/
static __NO_RETURN void BlinkLed (void *arg) {
  const uint8_t led_val[16] = { 0x48,0x88,0x84,0x44,0x42,0x22,0x21,0x11,
                                0x12,0x0A,0x0C,0x14,0x18,0x28,0x30,0x50 };
  uint32_t cnt = 0U;

  (void)arg;

  LEDrun = true;
  while(1) {
    // Every 100 ms 
    if (LEDrun == true) {
      LED_Expo (led_val[cnt]);
      if (++cnt >= sizeof(led_val)) {
        cnt = 0U;
      }
    }
    osDelay (100);
  }
}

/*
static __NO_RETURN void BlinkLed (void *arg) {
  uint32_t cnt = 0U;
  const uint8_t carrusel[] = {0x01, 0x02, 0x04};
  
  while(1) {
    if (LEDrun == true) {
        if (modo_especial == 2) {
            LED_Modo_Alarma(); // Modo Alarma
        } else if (modo_especial == 1) {
            LED_Write(carrusel[cnt]); // Modo Automático
            if (++cnt >= 3) cnt = 0;
        }
    } else {
        // MODO BROWSER: Aquí es donde aplicamos lo que viene de la web
        LED_Write(P2); 
    }
    osDelay(200); 
  }
}
*/
/*----------------------------------------------------------------------------
  Main Thread 'main': Run Network
 *---------------------------------------------------------------------------*/
__NO_RETURN void app_main (void *arg) {
  (void)arg;
    colaLCD_L1  = osMessageQueueNew(5,20*sizeof(char),NULL);        //Cola Linea 1
    colaLCD_L2  = osMessageQueueNew(5,20*sizeof(char),NULL);        //Cola Linea 2

  
	 LED_Init();
	 ADC1_pins_F429ZI_config();
	 ADC_Init_Single_Conversion(&adchandle , ADC1);

  netInitialize ();

  TID_Led     = osThreadNew (BlinkLed, NULL, NULL);
  TID_Display = osThreadNew (Display,  NULL, NULL);


  osThreadExit();
}
