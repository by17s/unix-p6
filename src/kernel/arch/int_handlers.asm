extern handle_irqint
extern handle_exception

global isr_table
global irq_table

%macro irqint 1
irqint_%+%1:
    push %1
    jmp irq_master
%endmacro

%macro isr_general 1
    push %1
    cli
    jmp isr_master
%endmacro

%macro isr_err 1
isr_%+%1:
    isr_general %1
%endmacro

%macro isr_noerr 1
isr_%+%1:
    push 0 ; Zero error code
    isr_general %1
%endmacro

section .text

irq_master:

    ;mov ecx, 0
    ;loop:
    ;inc ecx
    ;cmp ecx, 0xffffffff
    ;jne loop

    pusha                    ; Push general registers

    mov ax, ds
    push eax                 ; Push data segment

    mov ax, 0x10  ; Load the kernel data segment descriptor
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    call handle_irqint

    pop eax        ; Reload the original data segment descriptor
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    popa                     ; Pop general registers

    add esp, 4     ; Clean up the pushed error code and IRQ number
    iret           ; Pop 5 things at once: CS, EIP, EFLAGS, SS, and ESP

isr_master:
    pusha                    ; Push general registers

    mov ax, ds
    push eax                 ; Push data segment

    mov ax, 0x10  ; Load the kernel data segment descriptor
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    call handle_exception   

    pop eax        ; Reload the original data segment descriptor
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    popa                     ; Pop general registers

    add esp, 8     ; Clean up the pushed error code and ISR number
    sti
    iret           ; Pop 5 things at once: CS, EIP, EFLAGS, SS, and ESP

    irqint 0
    irqint 1
    irqint 2
    irqint 3
    irqint 4
    irqint 5
    irqint 6
    irqint 7
    irqint 8
    irqint 9
    irqint 10
    irqint 11
    irqint 12
    irqint 13
    irqint 14
    irqint 15

irq_table:
%assign i 0 
%rep    16
    dd irqint_%+i
%assign i i+1 
%endrep

    isr_noerr 0
    isr_noerr 1
    isr_noerr 2
    isr_noerr 3
    isr_noerr 4
    isr_noerr 5
    isr_noerr 6
    isr_noerr 7
    isr_err   8
    isr_noerr 9
    isr_err   10
    isr_err   11
    isr_err   12
    isr_err   13
    isr_err   14
    isr_noerr 15
    isr_noerr 16
    isr_err   17
    isr_noerr 18
    isr_noerr 19
    isr_noerr 20
    isr_noerr 21
    isr_noerr 22
    isr_noerr 23
    isr_noerr 24
    isr_noerr 25
    isr_noerr 26
    isr_noerr 27
    isr_noerr 28
    isr_noerr 29
    isr_err   30
    isr_noerr 31

isr_table:
%assign i 0 
%rep    32 
    dd isr_%+i
%assign i i+1 
%endrep