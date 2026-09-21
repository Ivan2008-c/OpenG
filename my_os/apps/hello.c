/* hello.c - Пример программы на нашем языке для формата .app */

#include <stdint.h>

/* Эта программа компилируется в формат .app */

void app_main() {
    /* Точка входа программы */
    
    /* В реальной ОС здесь были бы системные вызовы к ядру */
    /* Например: syscall_print("Hello from my language!"); */
    
    uint16_t* vga = (uint16_t*)0xB8000;
    const char* msg = "Hello from MY APP Language!";
    
    int i = 0;
    while (msg[i] != '\0') {
        vga[i] = (0x0A << 8) | msg[i]; /* Зеленый текст на черном фоне */
        i++;
    }
}

/* Таблица экспорта для Program Manager */
struct {
    char name[32];
    void (*entry)();
} app_export __attribute__((section(".app_export"))) = {
    "hello",
    app_main
};
