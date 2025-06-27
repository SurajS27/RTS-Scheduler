/**
 * Minimal FreeRTOS timers.h implementation for simulation
 */

 #ifndef TIMERS_H
 #define TIMERS_H
 
 #include "FreeRTOS.h"
 
 #ifdef __cplusplus
 extern "C" {
 #endif
 
 // Timer callback function type
 typedef void (*TimerCallbackFunction_t)(TimerHandle_t xTimer);
 
 // Timer modes
 #define tmrONE_SHOT         0
 #define tmrAUTO_RELOAD      1
 
 // Function prototypes
 TimerHandle_t xTimerCreate(const char * const pcTimerName,
                           const TickType_t xTimerPeriod,
                           const BaseType_t xAutoReload,
                           void * const pvTimerID,
                           TimerCallbackFunction_t pxCallbackFunction);
                           
 BaseType_t xTimerStart(TimerHandle_t xTimer, TickType_t xTicksToWait);
 BaseType_t xTimerStop(TimerHandle_t xTimer, TickType_t xTicksToWait);
 
 #ifdef __cplusplus
 }
 #endif
 
 #endif /* TIMERS_H */