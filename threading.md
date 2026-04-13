# Methods of Work

## Early Boot

Set up enough systems on the main thread to enable these other systems.
All cores wait until main core finishes early boot then continue together.
    (event system)

Early Boot Tasks:
- Logging
- Event System
- Memory Management
- ACPI
- Interrupts
    - Hardware and Software
- Thread Setup
    - When APs are created after setup they become idle threads
    - BSP has more setup then is idle
    - The threads wait for the BSP to send out the completion message before they start calling schedule
- Scheduler
    - Thread switching
    - Wait queues
    - Thread Queues
- Time Keeping


