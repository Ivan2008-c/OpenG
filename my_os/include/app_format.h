/* 
 * APP Format Specification v1.0
 * Спецификация формата исполняемых файлов .app для MY OS
 */

#ifndef APP_FORMAT_H
#define APP_FORMAT_H

#include <stdint.h>

/*
 * СТРУКТУРА ФАЙЛА .app
 * ====================
 * 
 * Смещение | Размер | Описание
 * ---------|--------|------------------
 * 0x00     | 4      | Magic number (0x41505058 = "APPX")
 * 0x04     | 4      | Версия формата
 * 0x08     | 4      | Смещение точки входа (относительно начала файла)
 * 0x0C     | 4      | Размер кода (байты)
 * 0x10     | 4      | Размер данных (байты)
 * 0x14     | 32     | Имя программы (null-terminated string)
 * 0x34     | 64     | Описание программы
 * 0x74     | var    | Код программы
 * ...      | ...    | Данные программы
 */

#define APP_MAGIC 0x41505058
#define APP_VERSION 1

/* Заголовок приложения */
struct app_header {
    uint32_t magic;          // Магическое число
    uint32_t version;        // Версия формата
    uint32_t entry_offset;   // Смещение точки входа
    uint32_t code_size;      // Размер кода
    uint32_t data_size;      // Размер данных
    char name[32];           // Имя программы
    char description[64];    // Описание
};

/* Минимальный размер заголовка */
#define APP_HEADER_SIZE 0x74

/* Максимальная длина имени программы */
#define APP_MAX_NAME_LEN 31

/* Максимальная длина описания */
#define APP_MAX_DESC_LEN 63

/* Флаги приложения */
#define APP_FLAG_NONE       0x00
#define APP_FLAG_GUI        0x01  // Графическое приложение
#define APP_FLAG_CONSOLE    0x02  // Консольное приложение
#define APP_FLAG_DRIVER     0x04  // Драйвер
#define APP_FLAG_SERVICE    0x08  // Фоновая служба

/* Структура для загрузки приложения */
struct app_load_info {
    struct app_header header;
    void* code_ptr;        // Указатель на код в памяти
    void* data_ptr;        // Указатель на данные в памяти
    uint32_t load_addr;    // Адрес загрузки
    uint32_t flags;        // Флаги
};

/* 
 * ПРИМЕР СОЗДАНИЯ ПРОГРАММЫ
 * =========================
 * 
 * 1. Создайте файл с расширением .c в папке apps/
 * 2. Определите функцию app_main() как точку входа
 * 3. Используйте структуру app_info для экспорта метаданных
 * 
 * Пример:
 * 
 * #include <stdint.h>
 * 
 * void app_main() {
 *     // Ваш код здесь
 * }
 * 
 * struct {
 *     uint32_t magic = 0x41505058;
 *     uint32_t version = 1;
 *     void (*entry)() = app_main;
 *     char name[32] = "my_program";
 *     char description[64] = "My custom application";
 * } __attribute__((section(".app_header"))) app_info;
 * 
 * 
 * КОМПИЛЯЦИЯ
 * ==========
 * 
 * i686-elf-gcc -m32 -ffreestanding -fno-stack-protector -nostdlib \
 *     -Wl,--section-start=.app_header=0x0 \
 *     -o program.app program.c
 * 
 * 
 * СИСТЕМНЫЕ ВЫЗОВЫ
 * ================
 * 
 * Приложение может вызывать функции ядра через прерывание 0x80
 * или через таблицу системных вызовов по адресу 0x10000
 * 
 * Номер | Функция              | Параметры          | Возврат
 * ------|----------------------|--------------------|----------
 * 0x01  | sys_print            | const char* str    | int (кол-во символов)
 * 0x02  | sys_print_hex        | uint32_t value     | void
 * 0x03  | sys_read_key         | void               | char (код клавиши)
 * 0x04  | sys_alloc_memory     | uint32_t size      | void* (адрес)
 * 0x05  | sys_free_memory      | void* ptr          | int (0=ok)
 * 0x06  | sys_exit             | int code           | never returns
 * 0x07  | sys_file_open        | const char* name   | int (file descriptor)
 * 0x08  | sys_file_read        | int fd, void* buf  | int (bytes read)
 * 0x09  | sys_file_write       | int fd, void* buf  | int (bytes written)
 * 0x0A  | sys_file_close       | int fd             | int (0=ok)
 */

#endif /* APP_FORMAT_H */
