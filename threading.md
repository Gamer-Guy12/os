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

## Thread Queues

Threads are put into one global queue for now.

get_thread:
- currently does nothing but will be used later so just call it from the tick

schedule_thread:
- Put a thread into the scheduler (Say after creation or sleeping)

requeue_thread:
- Put a thread back into the scheduler (after being run)

pop_thread:
- Pop the next thread out of the thread queue (for usage)

schedule:
- Switch to the next thread
- Guarentees to switch away

terminate:
- Kill current thread

wait_thread:
- Wait for a thread to finish

sleep:
- Sleep for some ms

## Waiting

A thread can only wait if it is currently running.
Other methods must be used if you want to put it to sleep while its not running.
Can be updated later.

Wait queue:
- A structure to store waiting threads
- threads can be awakened from it 
- A thread can add itself to a waitqueue to sleep

waitqueue_create:
- Creates a waitqueue

waitqueue_wait:
- Puts a thread into a waitqueue

waitqueue_awaken (tid):
- wakes a thread up

waitqueue_destroy (bool force):
- Destroys a waitqueue
- It will return false if force is false and there are still threads waiting
- It will return true otherwise
- If there are waiting threads and force is true it will awaken them

## Events

An object that can be triggered that has threads wait before waking them up when fired.
If the function is already awake when it triggers, it will not do anything to it.
Can be used to wait_thread (Wait for it do die and uses the tid as the filter)
Can take in a number to only trigger some at a time:

event_create:
- Initializes the event
- Specifies whether it takes a filter or not

event_trigger(size_t filter):
- If the event doesn't take a filter it will trigger all that wait if not it will only trigger those within the filter

event_destroy:
- Destroys that event

## Deferred Work

A work queue takes in functions and executes them. It has a worker thread executing everything that sleeps until an event triggers to wake it up.

## Sleep

Uses an event that gets triggered by the timer to awaken a thread. Uses a sleep id.
