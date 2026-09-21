/* linker.ld - Скрипт компоновщика */

ENTRY(main)

SECTIONS
{
    . = 0x1000; /* Адрес загрузки ядра */

    .text : ALIGN(4K)
    {
        *(.multiboot)
        *(.text*)
        *(.rodata)
    }

    .data : ALIGN(4K)
    {
        *(.data)
    }

    .bss : ALIGN(4K)
    {
        *(COMMON)
        *(.bss)
    }
}
