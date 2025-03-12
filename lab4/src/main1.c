#include <stdio.h>
#include <string.h>
#include "func.h"


int main() {
    char command[256];
    printf("Введите 'выход' для выхода.\n");

    while (1) {
        printf("Введите команду: ");
        scanf("%s", command);

        if (strcmp(command, "выход") == 0) {
            break;
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

    return 0;
}