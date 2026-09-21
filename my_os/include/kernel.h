/* kernel.h - Заголовочный файл ядра */

#ifndef KERNEL_H
#define KERNEL_H

#include <stdint.h>

/* VGA буфер */
#define VGA_MEMORY 0xB8000
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

/* Цвета VGA */
enum vga_color {
    VGA_BLACK = 0,
    VGA_BLUE = 1,
    VGA_GREEN = 2,
    VGA_CYAN = 3,
    VGA_RED = 4,
    VGA_MAGENTA = 5,
    VGA_BROWN = 6,
    VGA_LIGHT_GREY = 7,
    VGA_DARK_GREY = 8,
    VGA_LIGHT_BLUE = 9,
    VGA_LIGHT_GREEN = 10,
    VGA_LIGHT_CYAN = 11,
    VGA_LIGHT_RED = 12,
    VGA_LIGHT_MAGENTA = 13,
    VGA_LIGHT_BROWN = 14,
    VGA_WHITE = 15,
};

/* Структура заголовка приложения .app */
struct app_header {
    uint32_t magic;          // Магическое число: 0x41505058 ("APPX")
    uint32_t entry_point;    // Точка входа
    uint32_t code_size;      // Размер кода
    uint32_t data_size;      // Размер данных
    char name[32];           // Имя программы
};

#define APP_MAGIC 0x41505058

/* Функции ядра */
void kprint(const char* str);
void kprint_hex(uint32_t val);
void cls();
int load_app(const char* filename, struct app_header* header);
void run_app(struct app_header* header);

#endif /* KERNEL_H */
