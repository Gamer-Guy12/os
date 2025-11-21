# Structures

- Thread
    - Basic unit of execution
    - Contains information like the context, and scheduling info
- Schedulable
    - Information about scheduling
    - Has priority and stuff and is held in a thread queue
- Thread Queue
    - holds threads that can run
- Resource Queue
    - Holds threads that are waiting for a given resource
- Context
    - Basic required information to do a context switch

## Handling Page Tables
> [!NOTE]
> Page tables are contained within the context but are allowed to be NULL
> A NULL means do not swap

# Thread types

- Kernel Thread 
    - Only needs a thread
- User Thread
    - Thread
    - File Descriptors
    - Memory Info
    - Signal Handling
- Daemon
    - Thread
    - Signal Handling
    - Does other stuff in kernel space (network, memory, files)

