/**
 * Minimal FreeRTOS task.h implementation for simulation
 */

 #ifndef TASK_H
 #define TASK_H
 
 #include "FreeRTOS.h"
 
 #ifdef __cplusplus
 extern "C" {
 #endif
 
 // Function prototypes for FreeRTOS task API
 TickType_t xTaskGetTickCount(void);
 void vTaskDelay(const TickType_t xTicksToDelay);
 
 #ifdef __cplusplus
 }
 #endif
 
 #endif /* TASK_H */