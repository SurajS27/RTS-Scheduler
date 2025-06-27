/**
 * task_set.h - Task set structure and functions
 */

 #ifndef TASK_SET_H
 #define TASK_SET_H
 
 #include "FreeRTOS.h"
 #include "task.h"
 
 #define MAX_TASKS 50
 #define MAX_INSTANCES 100
 
 // Task states
 typedef enum {
     TASK_IDLE,    // Not released yet
     TASK_READY,   // Released but not executing
     TASK_RUNNING  // Currently executing
 } TaskState;
 
 // Task structure
 typedef struct Task {
     int id;                     // Task ID
     TickType_t period;          // Task period
     TickType_t relativeDeadline; // Relative deadline
     TickType_t worstCaseExecTime; // Worst-case execution time
     
     // Runtime information
     TaskState state;
     TickType_t nextArrivalTime;   // Absolute time of next arrival
     TickType_t absoluteDeadline;  // Absolute deadline of current instance
     TickType_t remainingExecutionTime; // Remaining execution time
     TickType_t arrivalTime;       // Arrival time of current instance
     TickType_t completionTime;    // Completion time of current instance
     
     // Actual execution times for simulation
     TickType_t actualExecutionTime[MAX_INSTANCES];
     int currentInstance;
     
     // Statistics
     unsigned int instancesCompleted;
     unsigned int deadlineMisses;
     TickType_t totalResponseTime;
 } Task;
 
 // Task set structure
 typedef struct {
     Task tasks[MAX_TASKS];
     int count;
 } TaskSet;
 
 // Function prototypes
 bool loadTaskSet(const char *filename, TaskSet *taskSet);
 bool loadActualExecutionTimes(const char *filename, TaskSet *taskSet);
 
 #endif /* TASK_SET_H */