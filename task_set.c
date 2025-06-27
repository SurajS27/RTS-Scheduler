/**
 * task_set.c - Task set implementation
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <stdbool.h>
 #include "task_set.h"
 
 bool loadTaskSet(const char *filename, TaskSet *taskSet) {
     FILE *file = fopen(filename, "r");
     if (file == NULL) {
         printf("Error opening task set file: %s\n", filename);
         return false;
     }
     
     // Reset task set
     memset(taskSet, 0, sizeof(TaskSet));
     
     char line[256];
     int lineCount = 0;
     
     // Read header line
     if (fgets(line, sizeof(line), file) == NULL) {
         printf("Error reading header line from task set file.\n");
         fclose(file);
         return false;
     }
     
     // Process task definitions
     while (fgets(line, sizeof(line), file) != NULL) {
         lineCount++;
         
         // Skip empty lines and comments
         if (line[0] == '\n' || line[0] == '#') {
             continue;
         }
         
         // Parse task parameters: ID, Period, Deadline, WCET
         int id;
         TickType_t period, deadline, wcet;
         
         int result = sscanf(line, "%d %u %u %u", &id, &period, &deadline, &wcet);
         if (result != 4) {
             printf("Error parsing task parameters at line %d\n", lineCount);
             continue;
         }
         
         if (taskSet->count >= MAX_TASKS) {
             printf("Warning: Maximum number of tasks reached. Ignoring task %d.\n", id);
             continue;
         }
         
         // Initialize task
         Task *task = &taskSet->tasks[taskSet->count];
         task->id = id;
         task->period = period;
         task->relativeDeadline = deadline;
         task->worstCaseExecTime = wcet;
         
         // Initialize runtime variables
         task->state = TASK_IDLE;
         task->nextArrivalTime = 0;  // First instance arrives at time 0
         task->remainingExecutionTime = 0;
         task->currentInstance = 0;
         
         // Initialize statistics
         task->instancesCompleted = 0;
         task->deadlineMisses = 0;
         task->totalResponseTime = 0;
         
         // Set default actual execution times to WCET
         for (int i = 0; i < MAX_INSTANCES; i++) {
             task->actualExecutionTime[i] = wcet;
         }
         
         // Increment task count
         taskSet->count++;
     }
     
     fclose(file);
     printf("Loaded %d tasks from %s\n", taskSet->count, filename);
     return true;
 }
 
 bool loadActualExecutionTimes(const char *filename, TaskSet *taskSet) {
     FILE *file = fopen(filename, "r");
     if (file == NULL) {
         printf("Error opening execution times file: %s\n", filename);
         return false;
     }
     
     char line[256];
     int lineCount = 0;
     
     // Read header line
     if (fgets(line, sizeof(line), file) == NULL) {
         printf("Error reading header line from execution times file.\n");
         fclose(file);
         return false;
     }
     
     // Process execution time definitions
     while (fgets(line, sizeof(line), file) != NULL) {
         lineCount++;
         
         // Skip empty lines and comments
         if (line[0] == '\n' || line[0] == '#') {
             continue;
         }
         
         // Parse line: TaskID, InstanceID, ActualExecTime
         int taskId, instanceId;
         TickType_t execTime;
         
         int result = sscanf(line, "%d %d %u", &taskId, &instanceId, &execTime);
         if (result != 3) {
             printf("Error parsing execution time at line %d\n", lineCount);
             continue;
         }
         
         // Find the task
         Task *task = NULL;
         for (int i = 0; i < taskSet->count; i++) {
             if (taskSet->tasks[i].id == taskId) {
                 task = &taskSet->tasks[i];
                 break;
             }
         }
         
         if (task == NULL) {
             printf("Warning: No task with ID %d found. Ignoring execution time.\n", taskId);
             continue;
         }
         
         // Update actual execution time
         if (instanceId < MAX_INSTANCES) {
             task->actualExecutionTime[instanceId] = execTime;
         } else {
             printf("Warning: Instance ID %d exceeds maximum (%d). Ignoring.\n", 
                   instanceId, MAX_INSTANCES - 1);
         }
     }
     
     fclose(file);
     printf("Loaded execution times from %s\n", filename);
     return true;
 }