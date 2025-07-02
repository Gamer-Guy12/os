
# Hal

There will be 2 types of hal thingies, archictecture specific, and global

archictecture specific things include:
- IRQ Handling
- and more!

global things include:
- Keyboard interrupts
- and more

What i mean is that like archictecture specific things are things that may only exist on certain platforms and are not standard across all of them

global things on the other hand exist accross all platoforms, that does not mean that they are the same accross all platforms

## archictecture specific

When hal_init is called it can call the archictecture specific init,

## global

When hal_init is called, it can call the archictecture again

that was easy

The archictecture is in charge of initializing the hal (kernel_start not kernel_main) <br/>
It also should initialize the global thingies not just archictecture specific


| type          | name |
|---------------|------|
| archictecture | irq  |
| global        | keyboard |


[^1]: OOO footnotes

