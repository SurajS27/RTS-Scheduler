/**
 * power_manager.h - Power management functions and definitions
 */

 #ifndef POWER_MANAGER_H
 #define POWER_MANAGER_H
 
 #include "FreeRTOS.h"
 #include "task.h"
 
 // Forward declaration for cyclic dependency
 typedef struct Task Task;
 
 // Power state
 typedef struct {
     float frequencyLevel;  // Current frequency level (0.4, 0.6, 0.8, or 1.0)
     bool isDPMActive;      // Whether system is in power-down mode
 } PowerState;
 
 // Power decision types
 typedef enum {
     POWER_NO_CHANGE,
     POWER_DVFS_CHANGE,
     POWER_DPM_ON,
     POWER_DPM_OFF
 } PowerDecisionType;
 
 // Power management decision
 typedef struct {
     PowerDecisionType type;
     float newFrequencyLevel;  // Only used for DVFS decisions
 } PowerDecision;
 
 // Constants
 #define DVFS_LEVELS 4
 #define DPM_THRESHOLD 20  // Minimum slack for DPM activation
 
 // Function prototypes
 void initializePowerManager(void);
 PowerDecision makeDVFSDecision(Task *task, TickType_t slack, PowerState *currentState);
 PowerDecision makeDPMDecision(TickType_t slack, PowerState *currentState);
 void applyPowerDecision(PowerDecision *decision, PowerState *state);
 
 #endif /* POWER_MANAGER_H */