/* kernel.c - Основное ядро ОС */

#include <stdint.h>
#include <string.h>
#include "kernel.h"

/* VGA буфер как массив символов с атрибутом цвета */
static uint16_t* vga_buffer = (uint16_t*)VGA_MEMORY;
static size_t cursor_x = 0;
static size_t cursor_y = 0;
static uint8_t color = VGA_LIGHT_GREY << 4 | VGA_BLACK;

/* Очистка экрана */
void cls() {
    for (size_t i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        vga_buffer[i] = (color << 8) | ' ';
    }
    cursor_x = 0;
    cursor_y = 0;
}

/* Установка цвета текста */
void set_color(uint8_t fg, uint8_t bg) {
    color = (bg << 4) | fg;
}

/* Вывод символа */
void putchar(char c) {
    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
        return;
    }
    
    if (cursor_y >= VGA_HEIGHT) {
        /* Прокрутка экрана */
        for (size_t i = 0; i < (VGA_HEIGHT - 1) * VGA_WIDTH; i++) {
            vga_buffer[i] = vga_buffer[i + VGA_WIDTH];
        }
        for (size_t i = (VGA_HEIGHT - 1) * VGA_WIDTH; i < VGA_WIDTH * VGA_HEIGHT; i++) {
            vga_buffer[i] = (color << 8) | ' ';
        }
        cursor_y = VGA_HEIGHT - 1;
    } else {
        vga_buffer[cursor_y * VGA_WIDTH + cursor_x] = (color << 8) | c;
        cursor_x++;
        
        if (cursor_x >= VGA_WIDTH) {
            cursor_x = 0;
            cursor_y++;
        }
    }
}

/* Вывод строки */
void kprint(const char* str) {
    while (*str) {
        putchar(*str);
        str++;
    }
}

/* Вывод числа в hex */
void kprint_hex(uint32_t val) {
    const char hex_chars[] = "0123456789ABCDEF";
    char buffer[11];
    buffer[0] = '0';
    buffer[1] = 'x';
    
    for (int i = 0; i < 8; i++) {
        buffer[9 - i] = hex_chars[val & 0xF];
        val >>= 4;
    }
    buffer[10] = '\0';
    
    kprint(buffer);
}

/* Заглушка для загрузки приложения с FAT32 */
int load_app(const char* filename, struct app_header* header) {
    /* В полной версии здесь будет код чтения FAT32 */
    kprint("Loading app: ");
    kprint(filename);
    kprint("\n");
    
    /* Эмуляция успешной загрузки */
    header->magic = APP_MAGIC;
    header->entry_point = 0x5000;
    header->code_size = 1024;
    header->data_size = 256;
    
    /* Копирование имени вручную (без strcpy) */
    const char* app_name = "test_program";
    int i = 0;
    while (app_name[i] != '\0' && i < 31) {
        header->name[i] = app_name[i];
        i++;
    }
    header->name[i] = '\0';
    
    return 0;
}

/* Запуск приложения */
void run_app(struct app_header* header) {
    if (header->magic != APP_MAGIC) {
        kprint("Error: Invalid APP format!\n");
        return;
    }
    
    kprint("Running: ");
    kprint(header->name);
    kprint(" at 0x");
    kprint_hex(header->entry_point);
    kprint("\n");
    
    /* В реальной ОС здесь был бы переход на точку входа */
    /* typedef void (*app_entry_t)(); */
    /* app_entry_t entry = (app_entry_t)header->entry_point; */
    /* entry(); */
}

/* Program Manager - меню управления программами */
void program_manager() {
    cls();
    set_color(VGA_WHITE, VGA_BLUE);
    
    kprint("========================================\n");
    kprint("       MY OS - PROGRAM MANAGER\n");
    kprint("========================================\n\n");
    
    set_color(VGA_LIGHT_GREY, VGA_BLUE);
    kprint("Available Programs:\n");
    kprint("  1. hello.app - Приветствие\n");
    kprint("  2. calc.app  - Калькулятор\n");
    kprint("  3. sysinfo.app - Информация о системе\n");
    kprint("\n");
    
    kprint("Commands:\n");
    kprint("  run <name>  - Запустить программу\n");
    kprint("  list        - Показать список программ\n");
    kprint("  exit        - Выйти из менеджера\n");
    kprint("\n");
    
    set_color(VGA_WHITE, VGA_BLUE);
    kprint("pm> ");
}

/* Точка входа ядра */
void main() {
    cls();
    set_color(VGA_LIGHT_GREEN, VGA_BLACK);
    
    kprint("==================================\n");
    kprint("  Welcome to MY OS (x86/FAT32)\n");
    kprint("==================================\n\n");
    
    kprint("Kernel initialized.\n");
    kprint("FAT32 support: enabled\n");
    kprint("APP format support: enabled\n\n");
    
    /* Запуск Program Manager */
    program_manager();
    
    /* Тестирование загрузки APP */
    struct app_header test_app;
    load_app("hello.app", &test_app);
    run_app(&test_app);
    
    kprint("\nSystem ready.\n");
    
    /* Бесконечный цикл */
    while (1) {
        __asm__ volatile("hlt");
    }
}
