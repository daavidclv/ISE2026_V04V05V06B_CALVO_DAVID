#include "stm32f4xx_hal.h"
#include "rtc.h"

 GPIO_InitTypeDef GPIO_InitStruct;
	
void BotonAzul_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // Habilitar el reloj del puerto C
    __HAL_RCC_GPIOC_CLK_ENABLE();

    // Configurar PC13 como entrada con interrupción por flanco de subida
    GPIO_InitStruct.Pin = GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    // Habilitar la interrupción en el controlador NVIC
    // Se usa la línea EXTI15_10 porque el pin es el 13
    HAL_NVIC_SetPriority(EXTI15_10_IRQn, 5, 0); 
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}

// Manejador principal de la interrupción de hardware
void EXTI15_10_IRQHandler(void) {
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
}

// Callback oficial que se ejecuta tras procesar la interrupción
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if (GPIO_Pin == GPIO_PIN_13) {
        
        // 1. Configuramos la hora a las 00:00:00
        RTC_ConfTime(0, 0, 0);
        
        // 2. Configuramos la fecha al 01-01-2000
        // Teniendo en cuenta la lógica de tu función RTC_ConfDate:
        // - El 1 de enero de 2000 fue Sábado (RTC_WEEKDAY_SATURDAY o 6)
        // - Pasamos 0 como mes porque internamente haces Month + 1
        // - Pasamos 100 como año porque internamente haces Year - 100
        RTC_ConfDate(RTC_WEEKDAY_SATURDAY, 0, 1, 100);
        
    }
}