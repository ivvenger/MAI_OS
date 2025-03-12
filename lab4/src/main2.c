#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <string.h>
#include "func.h"


int main() {
    void *libHandle;
    float (*Derivative)(float, float);
    float (*E)(int);
    char command[256];
    //char currentLib[256] = "./lib/libfunc1.so";
    char currentLib[256] = "./lib/libfunc1.dylib";


    printf("Введите 'выход' для выхода.\n");
    printf("Введите '0', чтобы переключиться между библиотеками.\n");

    // Загрузка первой библиотеки
    libHandle = dlopen(currentLib, RTLD_LAZY);
    if (!libHandle) {
        fprintf(stderr, "Ошибка загрузки библиотеки: %s\n", dlerror());
        return 1;
    }

    while (1) {
        printf("Введите команду: ");
        scanf("%s", command);

        if (strcmp(command, "выход") == 0) {
            break;
        }

        if (strcmp(command, "0") == 0) {
            dlclose(libHandle);
            //if (strcmp(currentLib, "./lib/libfunc1.so") == 0) {
            //    strcpy(currentLib, "./lib/libfunc2.so");
            //} else {
            //    strcpy(currentLib, "./lib/libfunc1.so");
            //}

            if (strcmp(currentLib, "./lib/libfunc1.dylib") == 0) {
                strcpy(currentLib, "./lib/libfunc2.dylib");
            } else {
                strcpy(currentLib, "./lib/libfunc1.dylib");
            }

            libHandle = dlopen(currentLib, RTLD_LAZY);
            if (!libHandle) {
                fprintf(stderr, "Ошибка загрузки библиотеки: %s\n", dlerror());
                return 1;
            }

            printf("переключиться в библиотеку: %s\n", currentLib);
            continue;
        }

        // Загрузка функций из библиотеки
        Derivative = dlsym(libHandle, "Derivative");
        E = dlsym(libHandle, "E");

        if (!Derivative || !E) {
            fprintf(stderr, "Функции не найдены: %s\n", dlerror());
            dlclose(libHandle);
            return 1;
        }

        if (strcmp(command, "1") == 0) {
            float A, deltaX;
            printf("Введите параметры (A deltaX): ");
            scanf("%f %f", &A, &deltaX);
            printf("Результат производной: %f\n", Derivative(A, deltaX));
        } else if (strcmp(command, "2") == 0) {
            int x;
            printf("Введите параметр (x): ");
            scanf("%d", &x);
            printf("Результат E: %f\n", E(x));
        } else {
            printf("Неизвестная команда.\n");
        }
    }

    // Закрытие библиотеки перед выходом
    dlclose(libHandle);
    return 0;
}