#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
    int K;
    int experiments_per_thread;
    int start_points_first;
    int start_points_second;
    int *draws;
    int *first_wins;
    int *second_wins;
    pthread_mutex_t *mutex;
} ThreadData;


void *simulate_game(void *arg) {
    ThreadData *input_data = (ThreadData *)arg;
    int first_wins = 0;
    int second_wins = 0;
    int draws = 0;

    for (int i = 0; i < input_data->experiments_per_thread; ++i) {
        int sum1 = input_data->start_points_first;
        int sum2 = input_data->start_points_second;
        for (int j = 0; j < input_data->K; ++j) {
            sum1 += rand() % 6 + 1; // Случайное число от 1 до 6
            sum2 += rand() % 6 + 1; // Случайное число от 1 до 6
        }

        if (sum1 > sum2) {
            ++first_wins;
        } else if (sum2 > sum1) {
            ++second_wins;
        } else {
            ++draws;
        }
    }

    pthread_mutex_lock(input_data->mutex);
    *(input_data->first_wins) += first_wins;
    *(input_data->second_wins) += second_wins;
    *(input_data->draws) += draws;
    pthread_mutex_unlock(input_data->mutex);

    return NULL;
}


int main(int argc, char *argv[]) {
    srand(time(NULL)); // Инициализация генератора случайных чисел

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start); // Начало измерения

    int max_threads = 4;
    int K = 4; // Сколько туров нужно провести
    int start_points_first = 0;
    int start_points_second = 0;
    int experiments = 4; // Кол-во экспериментов
    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL); // Инициализация мьютекса

    for (int i = 1; i < argc; ++i) {
        if (argv[i] == NULL) {
            break;
        } else if (i == 1) {
            if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
                printf("./game [K] [S1] [S2] [E]\nK - кол-во туров;\nS1, S2 - "
                       "начальные значения очков игроков;\nE - кол-во экспериментов, "
                       "которые должна программа провести;\n\nFlags:\n\t-tr - задать "
                       "максимальное кол-во потоков (по умолчанию 4);\n\t-show - "
                       "показать также результаты игр.\n");
                return 0;
            }
            K = atoi(argv[i]);
        } else if (i == 2) {
            start_points_first = atoi(argv[i]);
        } else if (i == 3) {
            start_points_second = atoi(argv[i]);
        } else if (i == 4) {
            experiments = atoi(argv[i]);
        } else if (strcmp(argv[i], "-tr") == 0 && argv[i + 1]) {
            max_threads = atoi(argv[i + 1]);
            printf("Кол-во потоков установлено на %d\n", max_threads);
            ++i;
        }
    }

    int first_wins = 0;
    int second_wins = 0;
    int draws = 0;
    pthread_t threads[max_threads];
    ThreadData threads_data[max_threads];

    int experiments_per_thread = experiments / max_threads;
    int remaining_experiments = experiments % max_threads;

    for (int i = 0; i < max_threads; ++i) {
        threads_data[i].K = K;
        threads_data[i].experiments_per_thread = experiments_per_thread + (i < remaining_experiments ? 1 : 0);
        threads_data[i].start_points_first = start_points_first;
        threads_data[i].start_points_second = start_points_second;
        threads_data[i].draws = &draws;
        threads_data[i].first_wins = &first_wins;
        threads_data[i].second_wins = &second_wins;
        threads_data[i].mutex = &mutex;

        pthread_create(&threads[i], NULL, simulate_game, &threads_data[i]);
    }

    for (int i = 0; i < max_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&mutex);

    clock_gettime(CLOCK_MONOTONIC, &end); // Конец измерения

    double time_taken = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9; // Время в секундах

    printf("Всего игр было сыграно: %d\n", experiments);
    printf("Игрок 1 выиграл %d туров, шанс выигрыша: %.2f%%\n", first_wins, (first_wins * 100.0) / experiments);
    printf("Игрок 2 выиграл %d туров, шанс выигрыша: %.2f%%\n", second_wins, (second_wins * 100.0) / experiments);
    printf("Ничья была в %d турах, шанс ничьи: %.2f%%\n", draws, (draws * 100.0) / experiments);
    printf("Время выполнения: %f секунд\n", time_taken);

    return 0;
}