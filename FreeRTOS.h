/**
 * FreeRTOS adaptation layer for the scheduler simulation
 * 
 * This file provides minimal FreeRTOS-compatible definitions
 * to allow the scheduler to be compiled and run without the actual FreeRTOS kernel.
 */

 #ifndef FREERTOS_H
 #define FREERTOS_H
 
 #include <stdint.h>
 #include <stdbool.h>
 
 // Type definitions
 typedef uint32_t TickType_t;
 typedef uint32_t BaseType_t;
 typedef void* TaskHandle_t;
 typedef void* TimerHandle_t;
 
 // Constants
 #define pdTRUE          1
 #define pdFALSE         0
 #define pdPASS          1
 #define pdFAIL          0
 #define portMAX_DELAY   0xFFFFFFFF
 
 // Task priorities
 #define tskIDLE_PRIORITY                      0
 #define configMAX_PRIORITIES                  32
 
 // Function prototypes (minimal subset required for our simulation)
 TickType_t xTaskGetTickCount(void);
 void vTaskDelay(TickType_t xTicksToDelay);
 
 #endif /* FREERTOS_H */