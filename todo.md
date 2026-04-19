# TODO

- [x] Thread creation
    - Able to create a thread or encapsulate into a new thread
    - Need a thread trampoline
    - Able to switch between threads
- [x] Thread Destruction
    - a thread can be destroyed (if it is not running)
- [x] Thread ID handling
    - Can search for threads by id
- [ ] Thread queueing
    - A system to queue up threads
    - Simple priority system
    - global queue
- [ ] Thread termination
    - Threads can destroy themselves
- [ ] Thread waiting
    - A thread can wait to be awoken
- [ ] Event
    - Events that can wake up threads
- [ ] Thread timer interrupts
- [ ] Work Queues
    - Queues of functions to be run by worker threads
    - Different priorities of queue
- [ ] Timer system overhaul
    - Not the apic but main timer system that is for interrupting at specific times
    - Use existing framework to utilize work queues

