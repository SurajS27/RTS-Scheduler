/**
 * power_manager.c - Power management implementation
 */

 #include <stdio.h>
 #include "power_manager.h"
 #include "task_set.h"
 
 // Available frequency levels
 static const float availableFrequencyLevels[DVFS_LEVELS] = {0.4, 0.6, 0.8, 1.0};
 
 void initializePowerManager(void) {
     // Initialize power management hardware or simulation
     printf("Power manager initialized with DVFS levels: 0.4, 0.6, 0.8, 1.0\n");
     printf("DPM threshold set to: %d ticks\n", DPM_THRESHOLD);
 }
 
 PowerDecision makeDVFSDecision(Task *task, TickType_t slack, PowerState *currentState) {
     PowerDecision decision;
     decision.type = POWER_NO_CHANGE;
     decision.newFrequencyLevel = currentState->frequencyLevel;
     
     // If DPM is active, we need to wake up first
     if (currentState->isDPMActive) {
         decision.type = POWER_DPM_OFF;
         return decision;
     }
     
     // CCEDF algorithm:
     // 1. Calculate required frequency based on utilization and slack
     float requiredFrequency = 0.0;
     
     if (task != NULL) {
         // Calculate minimum required frequency to meet the deadline
         TickType_t timeToDeadline = task->absoluteDeadline - task->arrivalTime;
         requiredFrequency = (float)task->actualExecutionTime[task->currentInstance] / timeToDeadline;
         
         // Adjust for remaining work
         if (task->remainingExecutionTime > 0) {
             TickType_t currentTimeToDeadline = task->absoluteDeadline - 
                                               (task->arrivalTime + task->actualExecutionTime[task->currentInstance] - 
                                                task->remainingExecutionTime);
             
             if (currentTimeToDeadline > 0) {
                 requiredFrequency = (float)task->remainingExecutionTime / currentTimeToDeadline;
             } else {
                 // Past deadline, use maximum frequency
                 requiredFrequency = 1.0;
             }
         }
     }
     
     // Find the minimum frequency level that meets the requirement
     int selectedFrequencyIndex = 0;  // Default to lowest frequency
     
     for (int i = 0; i < DVFS_LEVELS; i++) {
         if (availableFrequencyLevels[i] >= requiredFrequency) {
             selectedFrequencyIndex = i;
             break;
         }
     }
     
     float newFrequency = availableFrequencyLevels[selectedFrequencyIndex];
     
     // Only make a change if the frequency is different
     if (newFrequency != currentState->frequencyLevel) {
         decision.type = POWER_DVFS_CHANGE;
         decision.newFrequencyLevel = newFrequency;
     }
     
     return decision;
 }
 
 PowerDecision makeDPMDecision(TickType_t slack, PowerState *currentState) {
     PowerDecision decision;
     decision.type = POWER_NO_CHANGE;
     
     // DPM logic: If slack is greater than threshold, power down
     if (slack > DPM_THRESHOLD && !currentState->isDPMActive) {
         decision.type = POWER_DPM_ON;
     } 
     // If already in DPM mode and slack is below threshold, wake up
     else if (currentState->isDPMActive && slack <= DPM_THRESHOLD) {
         decision.type = POWER_DPM_OFF;
     }
     
     return decision;
 }
 
 void applyPowerDecision(PowerDecision *decision, PowerState *state) {
     switch (decision->type) {
         case POWER_DVFS_CHANGE:
             state->frequencyLevel = decision->newFrequencyLevel;
             break;
             
         case POWER_DPM_ON:
             state->isDPMActive = true;
             break;
             
         case POWER_DPM_OFF:
             state->isDPMActive = false;
             break;
             
         case POWER_NO_CHANGE:
         default:
             // No change
             break;
     }
 }