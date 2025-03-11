#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>


#define MAX_COMMAND_LEN 256
//область разделяемой памяти
#define SHM_NAME "/shared_memory"
//семафоры для синхронизации между процессами
#define SEM_PARENT "/sem_parent"
#define SEM_CHILD "/sem_child"


typedef struct {
    char filename[MAX_COMMAND_LEN];
    char command[MAX_COMMAND_LEN];
} shared_data_t;


int main() {
    int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("Ошибка shm_open");
        exit(1);
    }
    //устанавливаем размер разделяемой памяти равный размеру созданной структуры
    ftruncate(shm_fd, sizeof(shared_data_t));
    //mmap отображает эту область памяти в адресное пространство процесса, shared_mem указывает на область памяти, которую видят и родительский и дочерний процессы
    shared_data_t *shared_mem = (shared_data_t *)mmap(0, sizeof(shared_data_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shared_mem == MAP_FAILED) {
        perror("Ошибка mmap"); // ошибка при отображении памяти
        exit(1);
    }
    //создаём 2 семафора для уведомления дочернего процесса, что команда введенаб а второй семафор, что результат готов
    sem_t *sem_parent = sem_open(SEM_PARENT, 0);
    sem_t *sem_child = sem_open(SEM_CHILD, 0);


    while (1) {
        sem_wait(sem_parent); // ожидаем уведомления от родительского процесса


        if (strcmp(shared_mem->command, "выход") == 0) {
            printf("Выход из child...\n");
            break;
        }
        //открываем файл, имя которого было передано через shared mem
        FILE *file = fopen(shared_mem->filename, "a");
        if (file == NULL) {
            perror("Ошибка открытия файла");
            exit(1);
        }


        int num1, num2, result;
        char *token = strtok(shared_mem->command, " ");
        num1 = atoi(token);
        result = num1;


        int division_by_zero = 0; // флаг для отслеживания деления на ноль
        while ((token = strtok(NULL, " ")) != NULL) {
            num2 = atoi(token);
            if (num2 == 0) {
                strcpy(shared_mem->command, "Ошибка деления на 0");
                fprintf(file, "Ошибка деления на 0\n");
                division_by_zero = 1;
                break;
            }
            result /= num2;
        }


        if (!division_by_zero) {
            sprintf(shared_mem->command, "%d", result);
            fprintf(file, "Результат деления: %d\n", result);
        }


        fflush(file); // принудительная запись данных в файл
        fclose(file);
        //говорим родительскому процессу, что ребёнок завершил работу
        sem_post(sem_child);
    }


    munmap(shared_mem, sizeof(shared_data_t)); // отключение отображения памяти
    close(shm_fd);
    sem_close(sem_parent);
    sem_close(sem_child);
    return 0;
}