#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h> // библиотека для работы с memory-mapped files
#include <fcntl.h>
#include <semaphore.h>


#define MAX_COMMAND_LEN 256
#define SHM_NAME "/shared_memory"
#define SEM_PARENT "/sem_parent"
#define SEM_CHILD "/sem_child"

// структура для хранения данных в разделяемой памяти
typedef struct {
    char filename[MAX_COMMAND_LEN];
    char command[MAX_COMMAND_LEN];
} shared_data_t;


int main() {
    //создаём разделяемую память с правами доступа 0666
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("Ошибка shm_open");
        exit(1);
    }
    //устанавливаем размер разделяемой памяти равный размеру созданной структуры
    ftruncate(shm_fd, sizeof(shared_data_t));
    //mmap отображает эту область памяти в адресное пространство процесса, shared_mem указывает на область памяти, которую видят и родительский и дочерний процессы
    shared_data_t *shared_mem = (shared_data_t *)mmap(0, sizeof(shared_data_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shared_mem == MAP_FAILED) { // ошибка при отображении памяти
        perror("Ошибка mmap");
        exit(1);
    }
    // создаем два семафора для синхронизации процессов
    sem_t *sem_parent = sem_open(SEM_PARENT, O_CREAT, 0666, 0);
    sem_t *sem_child = sem_open(SEM_CHILD, O_CREAT, 0666, 0);
    //делаем fork и создаём дочерний процесс
    pid_t pid = fork();
    if (pid < 0) {
        perror("Ошибка fork");
        exit(1);
    }
    //находимся в дочернем 
    if (pid == 0) {
        execl("./child", "child", NULL);
        perror("Ошибка");
        exit(1);
    } else {
        //внутри родительского
        printf("Введите имя файла: ");
        fgets(shared_mem->filename, sizeof(shared_mem->filename), stdin);
        shared_mem->filename[strcspn(shared_mem->filename, "\n")] = 0;


        char command[MAX_COMMAND_LEN];
        while (1) {
            printf("Введите числа, разделенные пробелом или 'выход' для завершения: ");
            //считываем команду из stdin
            fgets(command, sizeof(command), stdin);
            command[strcspn(command, "\n")] = 0;
            //копируем команду в shared memory
            strcpy(shared_mem->command, command);
            //увеличиваем счётчик семафора и дочерний процесс может быть выполнен
            sem_post(sem_parent);
            //проверка на выход
            if (strcmp(command, "выход") == 0) {
                break;
            }
            //ждём выполнение дочернего процесса
            sem_wait(sem_child);
            printf("Результат деления: %s\n", shared_mem->command);
        }


        wait(NULL); //ждем завершения дочернего процесса
        //закрываем все семафоры и освобождаем ресурсы
        sem_close(sem_parent);
        sem_close(sem_child);
        sem_unlink(SEM_PARENT);
        sem_unlink(SEM_CHILD);
        munmap(shared_mem, sizeof(shared_data_t));
        shm_unlink(SHM_NAME);
        close(shm_fd);
    }


    return 0;
}