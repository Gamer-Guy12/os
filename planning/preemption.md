
# Preemption

API:
- Enable Preemption
    - Enables interrupts and allows all preemption to continue
- Disable Preemption
    - Disables interrupts and makes it so that if any preemption does happen it just instantly returns
- Run Preemption
    - Essentially just starts the preemption process
    - This one should be called in begin thread
    - Basically after the thread switch happens it will call so once it gets swapped back it will kind like continue ig
    - Wait no cuz like early swaps and stuff.

On every thread swap the current preemption should be canceled and a new one should be created

