# **ARBOS - A Custom Operating System**  
**Documentation**  

## **1. Introduction**  
ARBOS is a **32-bit x86 operating system** built from scratch, designed to run on real hardware or emulators like QEMU. It includes essential OS components such as:  
- **Bootloader (Multiboot-compliant)**  
- **Memory Management (PMM & Paging)**  
- **Interrupt Handling (IDT, PIC, Exceptions, IRQs)**  
- **Process Scheduling (Basic Round-Robin Scheduler)**  
- **System Calls (Syscall Interface)**  
- **Device Drivers (Keyboard, VGA, Timer)**  

This document explains the **key components** and their functionality step-by-step.  

---

## **2. Boot Process**  
### **2.1. Bootloader (`boot/boot.asm`)**  
- The OS boots using a **Multiboot-compliant** bootloader.  
- The bootloader:  
  1. Sets up a **stack** for the kernel.  
  2. Calls `idt_setup()` to initialize interrupts.  
  3. Jumps to the kernel’s `kmain()` function.  

### **2.2. Kernel Entry (`kernel/kernel.c` - `kmain()`)**  
- Initializes core OS components:  
  - **Physical Memory Manager (PMM)**  
  - **Paging (Virtual Memory)**  
  - **Timer (PIT)**  
  - **Task Scheduler**  
- Creates **two test tasks** (`task_a()` and `task_b()`) and starts the scheduler.  

---

## **3. Memory Management**  
### **3.1. Physical Memory Manager (`kernel/pmm.c`)**  
- Tracks **free and used memory pages** (4KB each).  
- Uses a **bitmap** to manage allocations.  
- Key Functions:  
  - `pmm_init()`: Marks low memory (0-1MB) and kernel space as used.  
  - `pmm_alloc_page()`: Allocates a free page.  
  - `pmm_free_page()`: Releases a page back to the free pool.  

### **3.2. Paging (`kernel/paging.c`)**  
- Implements **identity paging** (maps physical addresses 1:1 to virtual).  
- Sets up a **Page Directory (PD) and Page Tables (PT)**.  
- Enables paging by setting the **CR0 and CR3 registers**.  

---

## **4. Interrupts & Exceptions**  
### **4.1. Interrupt Descriptor Table (`kernel/interrupts.c`)**  
- The **IDT** defines how the CPU handles interrupts.  
- `idt_setup()`:  
  - Configures **exception handlers** (ISRs 0-31).  
  - Remaps the **PIC (Programmable Interrupt Controller)** to avoid conflicts.  
  - Sets up **IRQ handlers** (Timer, Keyboard).  
  - Configures **Syscall handler (int 0x80)**.  

### **4.2. Exception Handling (`kernel/exceptions.c`)**  
- Handles CPU exceptions (e.g., Divide Error, Page Fault).  
- Prints debug info (exception number, error code).  

### **4.3. Hardware Interrupts**  
1. **Timer (`kernel/timer.c`)**  
   - Uses the **PIT (Programmable Interval Timer)**.  
   - `init_timer()`: Sets the timer frequency (e.g., 100Hz).  
   - `timer_handler()`: Increments a tick counter.  

2. **Keyboard (`drivers/keyboard.c`)**  
   - Reads **scancodes** from PS/2 keyboard.  
   - Converts scancodes to ASCII and prints them via VGA.  

---

## **5. Process Management & Scheduling**  
### **5.1. Task Structure (`include/process.h`)**  
- Each task has:  
  - **PID (Process ID)**  
  - **State (Running, Ready, Blocked, etc.)**  
  - **Stack Pointer (Saved ESP)**  
  - **Stack Page (4KB allocated memory)**  

### **5.2. Scheduler (`kernel/process.c`)**  
- **Round-Robin Scheduling**: Tasks yield CPU voluntarily.  
- Key Functions:  
  - `task_create()`: Allocates a stack and initializes a task.  
  - `task_yield()`: Switches to the next ready task.  
  - `scheduler_start()`: Begins task execution.  

### **5.3. Context Switching (`kernel/context_switch.S`)**  
- **Saves CPU registers** of the current task.  
- **Loads registers** of the next task.  
- Uses `pushad`/`popad` for register preservation.  

---

## **6. System Calls (Syscalls)**  
### **6.1. Syscall Mechanism (`kernel/syscall.c`)**  
- **`int 0x80`** triggers a syscall.  
- **Syscall Numbers**:  
  - `SYS_WRITE` (1): Writes data to the screen.  
  - `SYS_GET_TICKS` (2): Returns timer ticks.  

### **6.2. Syscall Handling (`kernel/isr_syscall.S`)**  
- The **syscall stub** forwards arguments to `syscall_handler_regparm()`.  
- Return values are stored in `syscall_ret`.  

---

## **7. Device Drivers**  
### **7.1. VGA Text Mode (`drivers/vga.c`)**  
- Writes text to **0xB8000** (VGA buffer).  
- Functions:  
  - `vga_putc()`: Prints a character.  
  - `vga_puts()`: Prints a string.  
  - `vga_clear()`: Clears the screen.  

### **7.2. Keyboard (`drivers/keyboard.asm` & `keyboard.c`)**  
- **IRQ1 handler** reads scancodes from **port 0x60**.  
- Converts scancodes to ASCII (simplified mapping).  

---

## **8. Build & Execution**  
### **8.1. Compilation (`build.sh`)**  
- Uses **NASM** for assembly files.  
- Uses **Clang** for C files (cross-compiled for `i686-elf`).  
- Links objects into `kernel.bin`.  

### **8.2. Running in QEMU**  
```sh
qemu-system-i386 -kernel dist/kernel.bin
```
- Boots the OS in **QEMU** with keyboard support.  

---

## **9. Future Improvements**  
1. **User-mode separation** (Ring 3).  
2. **File system support**.  
3. **Dynamic memory allocation (Heap management)**.  
4. **Multi-core CPU support**.  
5. **Better shell/command interface**.  

---

## **Conclusion**  
ARBOS is a **simple but functional** 32-bit OS with:  
✔ **Bootloader**  
✔ **Memory Management**  
✔ **Interrupts & Syscalls**  
✔ **Task Scheduling**  
✔ **Basic Drivers**  