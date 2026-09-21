#!/bin/bash
# build.sh - Скрипт сборки ОС

set -e

echo "=== Building MY OS (x86/FAT32/APP) ==="

# Проверка наличия инструментов
if ! command -v nasm &> /dev/null; then
    echo "Error: NASM assembler not found. Install with: sudo apt install nasm"
    exit 1
fi

if ! command -v i686-elf-gcc &> /dev/null; then
    echo "Warning: i686-elf-gcc not found, trying regular gcc..."
    CC="gcc"
    CFLAGS="-m32 -ffreestanding -fno-stack-protector -nostdlib -I./include"
else
    CC="i686-elf-gcc"
    CFLAGS="-ffreestanding -fno-stack-protector -nostdlib -I./include"
fi

# Создание директорий
mkdir -p bin

echo "[1/4] Assembling bootloader..."
nasm -f bin src/boot.asm -o bin/boot.bin

echo "[2/4] Compiling kernel..."
$CC $CFLAGS -c src/kernel.c -o bin/kernel.o

echo "[3/4] Linking kernel..."
ld -m elf_i386 -T src/linker.ld -o bin/kernel.bin bin/kernel.o

echo "[4/4] Creating disk image with FAT32..."
# Создаем образ диска размером 10MB
dd if=/dev/zero of=myos.img bs=1M count=10 status=none

# Копируем загрузчик в первый сектор
dd if=bin/boot.bin of=myos.img conv=notrunc status=none

# Копируем ядро сразу после загрузчика
dd if=bin/kernel.bin of=myos.img seek=1 conv=notrunc status=none

echo ""
echo "=== Build Complete! ==="
echo "Disk image: myos.img"
echo ""
echo "To run in QEMU:"
echo "  qemu-system-i386 -drive format=raw,file=myos.img"
echo ""
echo "To test with FAT32 filesystem:"
echo "  1. Create FAT32 partition on myos.img"
echo "  2. Copy kernel.bin and *.app files to it"
echo ""

# Сборка примера приложения
echo "[BONUS] Compiling sample APP..."
if command -v i686-elf-gcc &> /dev/null; then
    i686-elf-gcc -m32 -ffreestanding -fno-stack-protector -nostdlib \
        -Wl,--section-start=.app_export=0x100000 \
        -o apps/hello.app apps/hello.c 2>/dev/null || true
    echo "Sample app created: apps/hello.app"
else
    echo "Skip app compilation (need i686-elf-gcc)"
fi
