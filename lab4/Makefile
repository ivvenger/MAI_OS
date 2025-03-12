# Компилятор
CC = gcc

# Флаги для создания динамических библиотек
CFLAGS = -fPIC -shared
LDFLAGS = -lm  # Подключение math.h

# Правило по умолчанию
all: lib/libfunc1.dylib lib/libfunc2.dylib main1 main2

#all: lib/libfunc1.so lib/libfunc2.so main1 main2

# Создание динамической библиотеки libfunc1.dylib
lib/libfunc1.dylib: src/func1.c src/func.h
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

# Создание динамической библиотеки libfunc2.dylib
lib/libfunc2.dylib: src/func2.c src/func.h
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)


# Создание динамической библиотеки libfunc1.so
#lib/libfunc1.so: src/func1.c src/func.h
#	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)  # <- Добавлен -lm

# Создание динамической библиотеки libfunc2.so
#lib/libfunc2.so: src/func2.c src/func.h
#	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)  # <- Добавлен -lm


# Компиляция main1 с использованием libfunc1.dylib
main1: src/main1.c
	$(CC) -o $@ $< -L./lib -lfunc1 $(LDFLAGS) -Wl,-rpath,./lib

# Компиляция main2 (если требуется динамическая загрузка)
main2: src/main2.c
	$(CC) -o $@ $< $(LDFLAGS)

# Компиляция main1 с использованием libfunc1.so
#main1: src/main1.c
#	$(CC) -o $@ $< -L./lib -lfunc1 $(LDFLAGS) -Wl,-rpath=./lib  # <- Добавлен -lm

# Компиляция main2 (если требуется динамическая загрузка)
#main2: src/main2.c
#	$(CC) -o $@ $< $(LDFLAGS)

# Очистка
clean:
	rm -f main1 main2 lib/*.dylib

# Очистка
#clean:
#	rm -f main1 main2 lib/*.so