#ifndef LAB2_H
#define LAB2_H

#include <pthread.h>

typedef struct {
  int k;                  // Количество бросков кубиков
  int player1_score;      // Текущий счет игрока 1
  int player2_score;      // Текущий счет игрока 2
  int experiments;        // Общее количество экспериментов
  int thread_count;       // Количество потоков
  int player1_wins;       // Количество побед игрока 1
  int player2_wins;       // Количество побед игрока 2
  int draws;              // Количество ничьих
  pthread_mutex_t mutex; // Мьютекс для синхронизации
} GameData;

void run_simulation(int k, int current_round, int player1_score, int player2_score, int experiments, int max_threads);

#endif // LAB2_H