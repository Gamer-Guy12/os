# Threading

## Structs

thread struct:
    - contains a context
    - thread id
    - scheduling info
    - page tables
    - state
        - Running: Currently running
        - Ready: Ready to run
        - Terminated: Should be cleaned up
        - Waiting: Waiting on something
    - stack

## Switching

A thread switch first switches the page tables if necessary
here is some code for the check

```c
if (!cur_tables == new_tables && !__pt_null(new_tables))
    __switch_tables(new_tables);
```

After that it does a context switch
it then calles switch_tail which finishes the switch

switch_tail does this:
    - Put the old thread where it belongs (e.g. into the wait queue, thread queue, kills it)
    - Changes the states of the threads (if needed)

functions:
    - switch_threads
    - switch_tail
        - Called after the thread switch to finish it off
        - Also is called by the trampoline
    - thread_trampoline
        - What all new threads switch to upon creation

## Creation

A new thread is created by settings a variable in the thread struct called entry point
Then having ip being set to a trampoline, it finishes off a thread switch and then calls the entry point

## Init

At init, the os is running using a stack within bootloader reclaimable memory
there should be 2 init functions:
- init_general_threading()
    - called once, sets up things like gheap stuff and more
- init_threading()
    - called on every core, puts the core into a thread and sets up things like the stack
    - it will also set up thread queues so that once ticking starts it will be able to get new threads

