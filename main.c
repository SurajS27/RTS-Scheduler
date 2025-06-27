/**
 * main.c - Real-time Scheduler with DVFS+DPM for FreeRTOS
 * 
 * This implementation combines Dynamic Voltage and Frequency Scaling (DVFS)
 * and Dynamic Power Management (DPM) techniques for energy-efficient real-time
 * task scheduling using the CCEDF algorithm.
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <stdbool.h>
 #include <string.h>
 #include "FreeRTOS.h"
 #include "task.h"
 #include "timers.h"
 
 #include "scheduler.h"
 #include "power_manager.h"
 #include "task_set.h"
 
 // Global variables
 TaskSet taskSet;
 PowerState currentPowerState;
 SchedulerStats stats;
 FILE *outputFile;
 
 // Function prototypes
 void initializeSystem(void);
 void mainSchedulerLoop(void);
 void analyzeResults(void);
 
 int main(int argc, char *argv[]) {
     printf("Starting Real-time Scheduler with DVFS+DPM...\n");
     
     // Initialize system components
     initializeSystem();
     
     // Load task set from input file
     if (!loadTaskSet("input_task_set.txt", &taskSet)) {
         printf("Error loading task set. Exiting.\n");
         return 1;
     }
     
     // Load actual execution times (for simulation purposes)
     if (!loadActualExecutionTimes("input_execution_times.txt", &taskSet)) {
         printf("Error loading execution times. Exiting.\n");
         return 1;
     }
     
     // Open output file
     outputFile = fopen("output.txt", "w");
     if (outputFile == NULL) {
         printf("Error opening output file. Exiting.\n");
         return 1;
     }
     
     // Run the scheduler
     mainSchedulerLoop();
     
     // Close output file
     fclose(outputFile);
     
     // Analyze results
     analyzeResults();
     
     printf("Scheduling completed successfully.\n");
     return 0;
 }
 
 void initializeSystem(void) {
     // Initialize power state
     currentPowerState.frequencyLevel = 1.0; // Start at max frequency
     currentPowerState.isDPMActive = false;
     
     // Initialize stats
     memset(&stats, 0, sizeof(SchedulerStats));
     
     // Initialize power management
     initializePowerManager();
 }
 
 void mainSchedulerLoop(void) {
     TickType_t currentTime = 0;
     TickType_t simulationEndTime = calculateSimulationEndTime(&taskSet);
     
     fprintf(outputFile, "Time | Running Task | Frequency | Power Mode | Slack | Decision\n");
     fprintf(outputFile, "----------------------------------------------------------\n");
     
     while (currentTime <= simulationEndTime) {
         // Check for new task arrivals
         checkForTaskArrivals(&taskSet, currentTime);
         
         // Get the highest priority ready task (EDF)
         Task *currentTask = getHighestPriorityTask(&taskSet);
         
         // Calculate slack
         TickType_t slack = calculateSystemSlack(&taskSet, currentTime);
         
         // Determine appropriate power management strategy
         PowerDecision decision;
         if (currentTask == NULL) {
             // No running job - use DPM logic based on slack
             decision = makeDPMDecision(slack, &currentPowerState);
         } else {
             // Has running job - use DVFS (CCEDF) logic
             decision = makeDVFSDecision(currentTask, slack, &currentPowerState);
         }
         
         // Apply power management decision
         applyPowerDecision(&decision, &currentPowerState);
         
         // Update statistics
         updateStats(&stats, &currentPowerState, currentTask, currentTime);
         
         // Write to output file
         logSchedulerState(outputFile, currentTime, currentTask, 
                          currentPowerState, slack, decision);
         
         // Execute current task (simulation)
         if (currentTask != NULL) {
             executeTask(currentTask, &currentPowerState, &currentTime);
         } else {
             // No task to execute, move time forward
             currentTime++;
         }
         
         // Check for completed tasks
         checkForTaskCompletions(&taskSet, currentTime);
     }
 }
 
 void analyzeResults(void) {
     FILE *analysisFile = fopen("analysis.txt", "w");
     if (analysisFile == NULL) {
         printf("Error opening analysis file.\n");
         return;
     }
     
     // Write analysis to file
     fprintf(analysisFile, "---- Scheduler Analysis ----\n\n");
     
     // Overall statistics
     fprintf(analysisFile, "Total execution time: %u ticks\n", stats.totalExecutionTime);
     fprintf(analysisFile, "Energy consumption estimate: %.2f units\n", stats.energyConsumption);
     
     // DVFS statistics
     fprintf(analysisFile, "DVFS transitions: %u\n", stats.dvfsTransitions);
     fprintf(analysisFile, "Time spent at different frequency levels:\n");
     fprintf(analysisFile, "  - 1.0: %.2f%%\n", (float)stats.timeAtFrequency[3] / stats.totalExecutionTime * 100);
     fprintf(analysisFile, "  - 0.8: %.2f%%\n", (float)stats.timeAtFrequency[2] / stats.totalExecutionTime * 100);
     fprintf(analysisFile, "  - 0.6: %.2f%%\n", (float)stats.timeAtFrequency[1] / stats.totalExecutionTime * 100);
     fprintf(analysisFile, "  - 0.4: %.2f%%\n", (float)stats.timeAtFrequency[0] / stats.totalExecutionTime * 100);
     
     // DPM statistics
     fprintf(analysisFile, "DPM transitions: %u\n", stats.dpmTransitions);
     fprintf(analysisFile, "Time spent in power-down mode: %.2f%%\n", 
            (float)stats.timeInPowerDown / stats.totalExecutionTime * 100);
     
     // Task statistics
     fprintf(analysisFile, "\nTask Statistics:\n");
     for (int i = 0; i < taskSet.count; i++) {
         fprintf(analysisFile, "Task %d:\n", taskSet.tasks[i].id);
         fprintf(analysisFile, "  - Instances completed: %u\n", taskSet.tasks[i].instancesCompleted);
         fprintf(analysisFile, "  - Deadline misses: %u\n", taskSet.tasks[i].deadlineMisses);
         fprintf(analysisFile, "  - Average response time: %.2f ticks\n", 
                taskSet.tasks[i].instancesCompleted > 0 ? 
                (float)taskSet.tasks[i].totalResponseTime / taskSet.tasks[i].instancesCompleted : 0);
     }
     
     fclose(analysisFile);
 }