#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>


#define MAX_COMMAND_LEN 256


int main() {
    int pipe1[2], pipe2[2];
    pid_t pid; // храним тут идентификатор процесса


    // создаём  пайп1 и пайп2
    if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
        perror("Ошибка");
        exit(1);
    }

    pid = fork();
    if (pid < 0) {
        perror("Ошибка создания процесса");
        exit(1);
    }


    if (pid == 0) {
        // закрываем конец для записи у 1 пайпа, и закрываем конец чтения 2 пайпа
        close(pipe1[1]);
        close(pipe2[0]);


        // перенаправляем pipe1[0] на стандартный ввод дочернего процесса
        dup2(pipe1[0], STDIN_FILENO);
        close(pipe1[0]);


        // делаем то же самое, но для стандартного вывода дочернего процесса
        dup2(pipe2[1], STDOUT_FILENO);
        close(pipe2[1]);


        // запускаем дочерний процесс
        execl("./child", "child", NULL); // меняем текущий процесс на новый
        perror("Ошибка");
        exit(1);
    } else {
        //закрываем конец для чтения у 1 пайпа, и конец для записи у второго
        close(pipe1[0]);
        close(pipe2[1]);


        char filename[MAX_COMMAND_LEN]; // буфер для хранения имени файла
        printf("Введите имя файла: ");
        fgets(filename, sizeof(filename), stdin);
        filename[strcspn(filename, "\n")] = 0; // удаляем символ новой строки из имени файла


        //отправляем имя файла, которое мы ввели через 1 пайп
        write(pipe1[1], filename, strlen(filename) + 1);


        char command[MAX_COMMAND_LEN]; // буфер для хранения команды
        while (1) {
            printf("Введите числа через пробел или 'выход' для завершения : ");
            fgets(command, sizeof(command), stdin);


            // проверка на выход
            if (strstr(command, "выход") != NULL) {
                write(pipe1[1], "выход", strlen("выход") + 1);
                break;
            }


            //отправляем команду в дочерний проесс через первый пайп
            write(pipe1[1], command, strlen(command) + 1);


            // считываем результат из child через pipe2
            char result[MAX_COMMAND_LEN]; // буфер для хранения результата
            read(pipe2[0], result, sizeof(result)); // чтение результата из пайп
            printf("Результат деления: %s\n", result);
        }


        // ожидаем завершения процесса child
        wait(NULL);
        close(pipe1[1]);
        close(pipe2[0]);
    }


    return 0;
}
