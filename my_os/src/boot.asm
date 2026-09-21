; boot.asm - Загрузчик для x86 (BIOS Real Mode -> Protected Mode)
; Загружает ядро с диска и передает управление ему.

[bits 16]
[org 0x7c00]

KERNEL_OFFSET equ 0x1000 ; Адрес, куда будем загружать ядро

start:
    ; Настройка сегментов
    mov ax, 0
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7c00

    ; Сброс видеоадаптера
    mov ax, 0x0003
    int 0x10

    ; Вывод сообщения о загрузке
    mov si, MSG_LOAD
    call print_string

    ; Чтение ядра с диска (LBA)
    ; Предполагаем, что ядро находится сразу после загрузчика
    mov ah, 0x02 ; BIOS Read Sectors
    mov al, 10   ; Читать 10 секторов (достаточно для простого ядра)
    mov ch, 0    ; Цилиндр 0
    mov cl, 2    ; Сектор 2 (сектор 1 - это мы)
    mov dh, 0    ; Головка 0
    mov dl, 0x80 ; Диск 0x80 (первый жесткий диск)
    mov bx, KERNEL_OFFSET
    int 0x13

    jc disk_error ; Если ошибка переноса

    mov si, MSG_OK
    call print_string

    ; Переход в защищенный режим
    cli
    lgdt [gdt_descriptor]
    mov eax, cr0
    or al, 1
    mov cr0, eax
    jmp CODE_SEG:init_pm

disk_error:
    mov si, MSG_ERR
    call print_string
    jmp $

print_string:
    pusha
    mov ah, 0x0e
.loop:
    lodsb
    test al, al
    jz .done
    int 0x10
    jmp .loop
.done:
    popa
    ret

MSG_LOAD: db "Loading OS...", 0
MSG_OK:   db " OK", 13, 10, 0
MSG_ERR:  db " Disk Error!", 0

; GDT (Global Descriptor Table)
align 4
gdt_start:
    dq 0 ; Null descriptor
gdt_code:
    dw 0xffff    ; Limit
    dw 0         ; Base (low)
    db 0         ; Base (mid)
    db 10011010b ; Access (exec, read, non-conforming, present)
    db 11001111b ; Flags + Limit (high)
    db 0         ; Base (high)
gdt_data:
    dw 0xffff
    dw 0
    db 0
    db 10010010b ; Access (write, expand down, present)
    db 11001111b
    db 0
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

[bits 32]
init_pm:
    mov ax, DATA_SEG
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ebp, 0x90000
    mov esp, ebp

    ; Вызов ядра (адрес будет подставлен линковщиком)
    call 0x1000

    jmp $

times 510-($-$$) db 0
dw 0xaa55 ; Boot signature
