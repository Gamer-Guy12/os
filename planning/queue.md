
# Queue

Basic enqueue:
- Need a reference to the tail
- update the tails, next ptr to point to the new element
- change the tail to the new element

What if tail is null, then head is null and they both can be updated

Get a reference to the tail and store it
change the tail ptr to be the new element (cas)
- ptr will the the tail ptr
- old value will the stored value
- new value will the new element
change the stored tails next to be the new element

Basic Dequeue:
- Need a reference to the head
- update the head to be the saved head.next
- return the saved head

updating the head:
cas(queue.head, saved head, new head)

