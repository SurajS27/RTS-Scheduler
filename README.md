
# Real-time Scheduler with DVFS and DPM Simulation

This project implements a real-time task scheduler that incorporates Dynamic Voltage and Frequency Scaling (DVFS) and Dynamic Power Management (DPM) techniques for enhanced energy efficiency. It is designed as a simulation environment, leveraging minimal FreeRTOS header adaptations for compatibility and potential integration pathways.

The scheduler follows an Earliest Deadline First (EDF) scheduling policy. For power management, it employs a Constant-utilization/Capacity Earliest Deadline First (CCEDF) like approach for DVFS when tasks are executing and a slack-based approach for DPM during idle periods.

## Features

*   **Real-time Scheduling:** Implements an EDF-based scheduling policy.
*   **Dynamic Voltage and Frequency Scaling (DVFS):** Adjusts the simulated CPU frequency based on task requirements and system slack to reduce energy consumption while meeting deadlines (inspired by CCEDF principles). Supports multiple frequency levels (0.4, 0.6, 0.8, 1.0).
*   **Dynamic Power Management (DPM):** Transitions the system to a low-power state (simulated) when there is sufficient system slack and no tasks are ready to run, further saving energy.
*   **Simulation Environment:** Simulates task arrivals, execution, state transitions, and power management decisions tick-by-tick.
*   **Configurable Task Sets:** Loads periodic task parameters (Period, Deadline, WCET) from an input file (`input_task_set.txt`).
*   **Actual Execution Times:** Allows loading actual execution times for specific task instances from an input file (`input_execution_times.txt`) to simulate variability. Defaults to WCET if not provided.
*   **Detailed Logging:** Generates a tick-by-tick trace of the scheduler's state (`output.txt`), showing the running task, frequency, power mode, slack, and the decision made.
*   **Performance and Energy Analysis:** Produces a summary analysis (`analysis.txt`) including overall execution time, estimated energy consumption, power state transitions, and task-specific statistics (completions, deadline misses, response times).

## How it Works

The simulation operates in discrete time steps (ticks). In each tick:

1.  The scheduler checks for task arrivals based on their periods.
2.  It identifies the highest priority task ready to run based on the Earliest Deadline First (EDF) rule.
3.  System slack is calculated, representing the minimum time margin before the earliest deadline considering remaining work.
4.  Based on whether a task is running and the available slack, the power manager makes a decision:
    *   If a task is running, it uses a DVFS strategy (CCEDF-inspired) to determine the minimum frequency required to meet the task's deadline considering its remaining execution time.
    *   If no task is running, it uses a DPM strategy, potentially entering a low-power state if the system slack exceeds a defined threshold (`DPM_THRESHOLD`).
5.  The chosen power state (frequency and DPM mode) is applied.
6.  If a task is running and the system is active, its remaining execution time is decremented, scaled by the current frequency level.
7.  Simulation time advances by one tick.
8.  The scheduler checks for task completions and updates statistics, including response times and deadline misses.
9.  The current state is logged to the output file.

The simulation continues until a predefined end time (calculated based on task hyperperiods). Finally, an analysis of the collected statistics is generated.

## Project Structure

*   `main.c`: The main program entry point. Handles initialization, loading input files, running the main simulation loop, and analyzing the results.
*   `scheduler.h`, `scheduler.c`: Contains the core scheduling logic, task management (arrivals, completions, state updates), slack calculation, task execution simulation loop, and statistics handling/logging.
*   `power_manager.h`, `power_manager.c`: Implements the DVFS and DPM decision-making algorithms. Defines the power state and decision structures.
*   `task_set.h`, `task_set.c`: Defines the `Task` and `TaskSet` data structures and provides functions to load task parameters and actual execution times from input files.
*   `FreeRTOS.h`, `task.h`, `timers.h`: Minimal header files providing necessary type definitions and function prototypes to mimic a FreeRTOS-like environment for compilation and simulation purposes. These do not include the actual FreeRTOS kernel logic.
*   `input_task_set.txt`: Default input file defining the tasks.
*   `input_execution_times.txt`: Default input file providing actual execution times for instances.
*   `output.txt`: Generated simulation trace log.
*   `analysis.txt`: Generated summary analysis file.
*   `README.md`: This file.

## Getting Started

To build and run the simulation:

1.  **Prerequisites:** You need a C compiler (like GCC or Clang) installed on your system.
2.  **Clone the repository:** (Assuming you put these files in a Git repo)
    ```bash
    git clone <repository-url>
    cd <repository-directory>
    ```
3.  **Compile the source code:**
    ```bash
    gcc *.c -o scheduler -Wall -Wextra
    ```
    *(Note: `-Wall` and `-Wextra` enable useful warnings. Consider addressing any warnings during compilation.)*
4.  **Ensure Input Files:** Make sure `input_task_set.txt` and `input_execution_times.txt` are present in the same directory as the compiled executable. You can modify these files to define your own task sets and execution profiles.
5.  **Run the simulation:**
    ```bash
    ./scheduler
    ```

The simulation will run, and the `output.txt` and `analysis.txt` files will be generated/overwritten in the current directory.

## Input File Formats

### `input_task_set.txt`

Defines the periodic tasks. Each line (excluding comments starting with `#` and empty lines) should follow the format:


TaskID Period Deadline WCET


*   `TaskID`: An integer identifier for the task.
*   `Period`: The task's period in simulation ticks (`TickType_t`).
*   `Deadline`: The task's relative deadline in simulation ticks (`TickType_t`).
*   `WCET`: The task's Worst-Case Execution Time in simulation ticks (`TickType_t`).

Example:
IGNORE_WHEN_COPYING_START
content_copy
download
Use code with caution.
IGNORE_WHEN_COPYING_END
Task Set
Format: TaskID Period Deadline WCET

1 5 5 1
2 15 15 2
3 30 30 3


### `input_execution_times.txt`

(Optional) Provides specific actual execution times for task instances, overriding the WCET defined in `input_task_set.txt`. Each line (excluding comments and empty lines) should follow the format:
IGNORE_WHEN_COPYING_START
content_copy
download
Use code with caution.
IGNORE_WHEN_COPYING_END

TaskID InstanceID ActualExecutionTime


*   `TaskID`: The ID of the task.
*   `InstanceID`: The 0-based index of the task instance (0 for the first release, 1 for the second, etc.).
*   `ActualExecutionTime`: The actual execution time for this specific instance in simulation ticks (`TickType_t`).

Example:
IGNORE_WHEN_COPYING_START
content_copy
download
Use code with caution.
IGNORE_WHEN_COPYING_END
Actual Execution Times
Format: TaskID InstanceID ActualExecutionTime

1 0 1
1 1 0.5 # Note: The sscanf in task_set.c expects integer (%u), float values like 0.5 might not be parsed correctly.
1 2 1
1 3 0.5
1 4 1
1 5 0.5
2 0 1.5 # Note: The sscanf expects integer (%u), float values like 1.5 might not be parsed correctly.
2 1 1.5 # Note: The sscanf expects integer (%u), float values like 1.5 might not be parsed correctly.
3 0 2.5 # Note: The sscanf expects integer (%u), float values like 2.5 might not be parsed correctly.


*Self-correction Note:* As observed from the code, the `sscanf` function in `loadActualExecutionTimes` is configured to read an unsigned integer (`%u`) for `ActualExecutionTime`. This means that fractional values like `0.5`, `1.5`, `2.5` present in the example `input_execution_times.txt` *will not be parsed correctly* and will likely result in `0` being read. The simulation logic internally uses floating-point for frequency scaling, but the input loading expects integers for execution times. You should either provide integer execution times in this file or modify the `sscanf` format to read floats if fractional execution times are intended as input. The energy calculation and execution simulation *do* use floating-point frequency levels, so internal execution time tracking likely needs to handle fractional values for accuracy when scaled.

## Output Files

### `output.txt`

A detailed log of the scheduler's state at each simulation tick.
IGNORE_WHEN_COPYING_START
content_copy
download
Use code with caution.
IGNORE_WHEN_COPYING_END
Time | Running Task | Frequency | Power Mode | Slack | Decision

0 | Task  1 ( 1/ 5) |   0.4  | Active     |      4 | DVFS -> 0.4
1 | Task  2 ( 1/15) |   0.4  | Active     |     13 | No change
... (continues for the full simulation duration)
IGNORE_WHEN_COPYING_START
content_copy
download
Use code with caution.
IGNORE_WHEN_COPYING_END

### `analysis.txt`

A summary of the simulation results and collected statistics.
IGNORE_WHEN_COPYING_START
content_copy
download
Use code with caution.
IGNORE_WHEN_COPYING_END

---- Scheduler Analysis ----

Total execution time: 91 ticks
Energy consumption estimate: 5.82 units
DVFS transitions: 0
Time spent at different frequency levels:

1.0: 0.00%

0.8: 0.00%

0.6: 0.00%

0.4: 0.00%
DPM transitions: 0
Time spent in power-down mode: 0.00%

Task Statistics:
Task 1:

Instances completed: 19

Deadline misses: 0

Average response time: 1.00 ticks
... (continues for all tasks)



## Acknowledgements

*   Based on concepts from Real-time Operating Systems, EDF scheduling, DVFS (CCEDF), and DPM.
*   Adapted minimal FreeRTOS headers for simulation purposes.

---
