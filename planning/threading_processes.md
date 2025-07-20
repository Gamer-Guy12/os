
# Queueing

There is one big thread queue and one big process list

the threads need to be doubly linked with the other threads in the process and singly linked in a queue together

The thread queue is a circular singly linked list

the process are doubly linked to eachother

# Thread Deletion
threads are deleted by popping them off the queue and then removing them from the process's list and then finally deallocating them



