#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#define MAX_COMMAND_LEN 256


int main() {
    char filename[MAX_COMMAND_LEN];
    char command[MAX_COMMAND_LEN];


    //считываем имя файла из 1 пайпа
    read(STDIN_FILENO, filename, sizeof(filename));


    FILE *file = fopen(filename, "a");
    if (file == NULL) {
        perror("Ошибка открытия файла");
        exit(1);
    }


    while (1) {
        // считали команду через первый пайп
        read(STDIN_FILENO, command, sizeof(command));


        // Проверка на exit
        if (strcmp(command, "выход") == 0) {
            printf("Выходим из child...\n");
            break;
        }


        // делим команду на числа, с которыми будем работать
        int num1, num2, result;
        char *token = strtok(command, " ");
        num1 = atoi(token);


        result = num1;
        while ((token = strtok(NULL, " ")) != NULL) {
            num2 = atoi(token);
            if (num2 == 0) {
                printf("Деление на 0 запрещено. Выход из child.\n");
                fprintf(file, "Деление на 0 запрещено. Выход из child.\n");
                fclose(file);
                exit(1);
            }


            result /= num2;
        }
        fprintf(file, "Результат деления: %d\n", result);
        fflush(file);


        // отправка результата в родительский процесс
        char result_str[MAX_COMMAND_LEN];
        sprintf(result_str, "%d", result);
        write(STDOUT_FILENO, result_str, strlen(result_str) + 1);
    }


    fclose(file);
    return 0;
}