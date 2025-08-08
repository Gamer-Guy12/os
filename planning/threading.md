
# Threading

## Thread Switching

The TCB contains:
- User RSP and RIP
- Kernel RSP
- General Purpose Registers
- Rflags
- Kernel RIP
- CR3 (in the PCB)
- XSAVE

The TCB will directly contain RSP, RSP0, CR3, XSAVE and RIP
The Stack contains the rest

When swap threads

### Saving

RIP is automagically pushed onto the stack by call
Then Push all the GPRs and RFLAGS
RSP is now saved and put into the rsp0 field of the tcb
This is also put into the registers field because it has the registers
Then use xsave to save the avx and other state

### Loading

Load the new rsp
Save the tcb into fs_msr
Use xrstor for loading the new avx and other state
Load cr3
Pop the GPRS and RFLAGS
Call ret which pops rip

### Seperating Save and Load

Returning means setting rax to a value
Store will set rax to 0 and load will set it to 1. 
This means that u can check the return value to see if you should load or not

```c
// If RAX is 0
if (save_thread() == 0) {
    // Do Things
    
    // Sets RAX to 1 when this thread is rejoined
    load_thread()
} else {
    // Continue on execution
}
```

