/* 
 * calc.c - Пример калькулятора в формате .app
 * Демонстрация создания программ на собственном языке
 */

#include <stdint.h>

/* Системные вызовы ядра (адреса будут определены в документации API) */
#define SYS_PRINT     0x10000
#define SYS_PRINT_HEX 0x10001
#define SYS_EXIT      0x10002

/* Макрос для вызова системных функций */
#define SYSCALL(addr, arg1, arg2) __asm__ volatile("call *%0" :: "r"(addr) : : "eax", "ebx", "ecx")

void app_main() {
    /* Точка входа программы */
    
    uint16_t* vga = (uint16_t*)0xB8000;
    const char* title = "=== CALCULATOR APP ===";
    const char* result = "Result: 42";
    
    /* Вывод заголовка */
    int offset = 0;
    while (title[offset] != '\0') {
        vga[offset] = (0x0F << 8) | title[offset]; /* Белый текст */
        offset++;
    }
    
    offset = 80 + 10; /* Вторая строка */
    while (result[offset - 80] != '\0') {
        vga[offset] = (0x0A << 8) | result[offset - 80]; /* Зеленый текст */
        offset++;
    }
    
    /* Пример вычислений */
    int a = 10;
    int b = 32;
    int sum = a + b;
    
    /* В полной версии здесь был бы вывод результата через syscall */
    /* SYSCALL(SYS_PRINT_HEX, sum, 0); */
}

/* Экспорт информации о приложении для Program Manager */
struct {
    uint32_t magic;          // 0x41505058
    uint32_t version;        // Версия формата
    void (*entry)();         // Точка входа
    char name[32];           // Имя программы
    char description[64];    // Описание
} __attribute__((section(".app_header"))) app_info = {
    .magic = 0x41505058,
    .version = 1,
    .entry = app_main,
    .name = "calculator",
    .description = "Simple calculator demo application"
};
