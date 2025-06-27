/**
 * scheduler.h - Scheduler definitions and functions
 */

 #ifndef SCHEDULER_H
 #define SCHEDULER_H
 
 #include <stdio.h>
 #include "FreeRTOS.h"
 #include "task.h"
 #include "power_manager.h"
 #include "task_set.h"
 
 // Constants for scheduler
 #define MAX_TASKS 50
 #define SLACK_THRESHOLD 10  // Threshold for DPM decisions
 
 // Scheduler statistics structure
 typedef struct {
     TickType_t totalExecutionTime;
     float energyConsumption;
     unsigned int dvfsTransitions;
     unsigned int dpmTransitions;
     TickType_t timeAtFrequency[4];  // Time spent at each frequency level
     TickType_t timeInPowerDown;     // Time spent in power-down mode
 } SchedulerStats;
 
 // Function prototypes
 void checkForTaskArrivals(TaskSet *taskSet, TickType_t currentTime);
 void checkForTaskCompletions(TaskSet *taskSet, TickType_t currentTime);
 Task* getHighestPriorityTask(TaskSet *taskSet);
 TickType_t calculateSystemSlack(TaskSet *taskSet, TickType_t currentTime);
 TickType_t calculateSimulationEndTime(TaskSet *taskSet);
 void executeTask(Task *task, PowerState *powerState, TickType_t *currentTime);
 void updateStats(SchedulerStats *stats, PowerState *powerState, Task *currentTask, TickType_t currentTime);
 void logSchedulerState(FILE *file, TickType_t time, Task *task, PowerState powerState, 
                       TickType_t slack, PowerDecision decision);
 
 #endif /* SCHEDULER_H */