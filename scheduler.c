/**
 * scheduler.c - Scheduler implementation
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include "scheduler.h"
 
 void checkForTaskArrivals(TaskSet *taskSet, TickType_t currentTime) {
     for (int i = 0; i < taskSet->count; i++) {
         Task *task = &taskSet->tasks[i];
         
         // Check if a new instance of the periodic task should arrive
         if (currentTime >= task->nextArrivalTime && task->state == TASK_IDLE) {
             task->state = TASK_READY;
             task->remainingExecutionTime = task->actualExecutionTime[task->currentInstance];
             task->absoluteDeadline = currentTime + task->relativeDeadline;
             task->arrivalTime = currentTime;
             
             // Schedule next arrival
             task->nextArrivalTime += task->period;
         }
     }
 }
 
 void checkForTaskCompletions(TaskSet *taskSet, TickType_t currentTime) {
     for (int i = 0; i < taskSet->count; i++) {
         Task *task = &taskSet->tasks[i];
         
         // Check if task has completed execution
         if (task->state == TASK_RUNNING && task->remainingExecutionTime <= 0) {
             task->state = TASK_IDLE;
             task->completionTime = currentTime;
             task->instancesCompleted++;
             
             // Calculate response time (completion time - arrival time)
             TickType_t responseTime = task->completionTime - task->arrivalTime;
             task->totalResponseTime += responseTime;
             
             // Check for deadline miss
             if (currentTime > task->absoluteDeadline) {
                 task->deadlineMisses++;
             }
             
             // Move to next instance
             task->currentInstance++;
             if (task->currentInstance >= MAX_INSTANCES) {
                 task->currentInstance = 0;  // Wrap around
             }
         }
     }
 }
 
 Task* getHighestPriorityTask(TaskSet *taskSet) {
     Task *highestPriorityTask = NULL;
     TickType_t earliestDeadline = portMAX_DELAY;
     
     // Earliest Deadline First (EDF) scheduling
     for (int i = 0; i < taskSet->count; i++) {
         Task *task = &taskSet->tasks[i];
         
         if (task->state == TASK_READY || task->state == TASK_RUNNING) {
             if (task->absoluteDeadline < earliestDeadline) {
                 earliestDeadline = task->absoluteDeadline;
                 highestPriorityTask = task;
             }
         }
     }
     
     // Update state if a task is selected
     if (highestPriorityTask != NULL && highestPriorityTask->state == TASK_READY) {
         highestPriorityTask->state = TASK_RUNNING;
     }
     
     return highestPriorityTask;
 }
 
 TickType_t calculateSystemSlack(TaskSet *taskSet, TickType_t currentTime) {
     TickType_t totalSlack = portMAX_DELAY;
     bool hasReadyTasks = false;
     
     for (int i = 0; i < taskSet->count; i++) {
         Task *task = &taskSet->tasks[i];
         
         if (task->state == TASK_READY || task->state == TASK_RUNNING) {
             hasReadyTasks = true;
             TickType_t taskSlack = task->absoluteDeadline - currentTime - task->remainingExecutionTime;
             
             if (taskSlack < totalSlack) {
                 totalSlack = taskSlack;
             }
         }
     }
     
     // If no tasks are ready, set a maximum slack value
     if (!hasReadyTasks) {
         // Find minimum time until next task arrival
         totalSlack = portMAX_DELAY;
         for (int i = 0; i < taskSet->count; i++) {
             if (taskSet->tasks[i].nextArrivalTime > currentTime) {
                 TickType_t timeUntilNextArrival = taskSet->tasks[i].nextArrivalTime - currentTime;
                 if (timeUntilNextArrival < totalSlack) {
                     totalSlack = timeUntilNextArrival;
                 }
             }
         }
     }
     
     return totalSlack;
 }
 
 TickType_t calculateSimulationEndTime(TaskSet *taskSet) {
     // Calculate hyperperiod (LCM of all task periods)
     // For simplicity, we'll just use a fixed number of periods for simulation
     const int SIMULATION_PERIODS = 3;
     TickType_t maxPeriod = 0;
     
     for (int i = 0; i < taskSet->count; i++) {
         if (taskSet->tasks[i].period > maxPeriod) {
             maxPeriod = taskSet->tasks[i].period;
         }
     }
     
     return maxPeriod * SIMULATION_PERIODS;
 }
 
 void executeTask(Task *task, PowerState *powerState, TickType_t *currentTime) {
     // Scale execution by current frequency level
     float progress = powerState->frequencyLevel;
     
     // Update remaining execution time
     if (task->remainingExecutionTime >= progress) {
         task->remainingExecutionTime -= progress;
     } else {
         task->remainingExecutionTime = 0;
     }
     
     // Advance simulation time
     (*currentTime)++;
 }
 
 void updateStats(SchedulerStats *stats, PowerState *powerState, Task *currentTask, TickType_t currentTime) {
     // Update total execution time
     stats->totalExecutionTime = currentTime + 1;  // +1 because time is 0-indexed
     
     // Update time spent at each frequency level
     if (powerState->frequencyLevel == 1.0) {
         stats->timeAtFrequency[3]++;
     } else if (powerState->frequencyLevel == 0.8) {
         stats->timeAtFrequency[2]++;
     } else if (powerState->frequencyLevel == 0.6) {
         stats->timeAtFrequency[1]++;
     } else if (powerState->frequencyLevel == 0.4) {
         stats->timeAtFrequency[0]++;
     }
     
     // Update time in power-down mode
     if (powerState->isDPMActive) {
         stats->timeInPowerDown++;
     }
     
     // Update energy consumption (simple model)
     // Energy ∝ Voltage² × Frequency
     float voltage = powerState->frequencyLevel;  // Assuming voltage scales with frequency
     float energyThisTick = voltage * voltage * powerState->frequencyLevel;
     
     // If DPM is active, use static leakage energy only (much lower)
     if (powerState->isDPMActive) {
         energyThisTick = 0.05;  // Example static leakage energy value
     }
     
     stats->energyConsumption += energyThisTick;
 }
 
 void logSchedulerState(FILE *file, TickType_t time, Task *task, PowerState powerState, 
                       TickType_t slack, PowerDecision decision) {
     fprintf(file, "%5u | ", (unsigned int)time);
     
     // Log running task
     if (task != NULL) {
         fprintf(file, "Task %2d (%2u/%2u) | ", 
                 task->id, 
                 (unsigned int)task->remainingExecutionTime, 
                 (unsigned int)task->absoluteDeadline);
     } else {
         fprintf(file, "     None      | ");
     }
     
     // Log frequency level
     fprintf(file, "  %.1f  | ", powerState.frequencyLevel);
     
     // Log power mode
     fprintf(file, "%s | ", powerState.isDPMActive ? "Power-down" : "Active    ");
     
     // Log slack
     if (slack == portMAX_DELAY) {
         fprintf(file, "  MAX  | ");
     } else {
         fprintf(file, "%6u | ", (unsigned int)slack);
     }
     
     // Log decision
     switch (decision.type) {
         case POWER_NO_CHANGE:
             fprintf(file, "No change\n");
             break;
         case POWER_DVFS_CHANGE:
             fprintf(file, "DVFS -> %.1f\n", decision.newFrequencyLevel);
             break;
         case POWER_DPM_ON:
             fprintf(file, "DPM -> ON\n");
             break;
         case POWER_DPM_OFF:
             fprintf(file, "DPM -> OFF\n");
             break;
     }
 }