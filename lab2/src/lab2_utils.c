#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "../include/lab2.h"


int roll_dice() {
  unsigned int random_number = rand();
  int dice1 = (random_number % 6) + 1;
  int dice2 = ((random_number >> 4) % 6) + 1;
  return dice1 + dice2;
}


void* simulate_games(void* arg) {
  GameData* data = (GameData*)arg;
  int local_player1_wins = 0;
  int local_player2_wins = 0;
  int base_experiments = data->experiments / data->thread_count;
  int extra_experiments = data->experiments % data->thread_count;
  int experiments_per_thread = base_experiments + (pthread_self() < extra_experiments);
  for (int i = 0; i < experiments_per_thread; ++i) {
    int player1_total = data->player1_score;
    int player2_total = data->player2_score;
    for (int j = 0; j < data->k; j++) {
      player1_total += roll_dice();
      player2_total += roll_dice();
    }
    if (player1_total > player2_total) {
      local_player1_wins++;
    } else if (player2_total > player1_total) {
      local_player2_wins++;
    } else {
      data->draws++;
    }
  }
  pthread_mutex_lock(&data->mutex);
  data->player1_wins += local_player1_wins;
  data->player2_wins += local_player2_wins;
  pthread_mutex_unlock(&data->mutex);
  return NULL;
}


void run_simulation(int k, int current_round, int player1_score, int player2_score, int experiments, int max_threads) {
  if (max_threads < 1 || experiments < 1) {
    fprintf(stderr, "Неверные аргументы\n");
    return;
  }
  GameData data;
  data.k = k;
  data.player1_score = player1_score;
  data.player2_score = player2_score;
  data.experiments = experiments;
  data.thread_count = max_threads;
  data.player1_wins = 0;
  data.player2_wins = 0;
  data.draws = 0;
  pthread_mutex_init(&data.mutex, NULL);

  pthread_t threads[max_threads];
  srand(time(NULL));
  for (int i = 0; i < max_threads; i++) {
    pthread_create(&threads[i], NULL, simulate_games, &data);
  }
  for (int i = 0; i < max_threads; i++) {
    pthread_join(threads[i], NULL);
  }

  printf("Игрок 1 победил в %d эксперименте(ах)\n", data.player1_wins);
  printf("Игрок 2 победил в %d эксперименте(ах)\n", data.player2_wins);
  printf("Ничья в %d эксперименте(ах)\n", data.draws);

  double total_experiments = data.player1_wins + data.player2_wins + data.draws;
  printf("Шанс на победу игрока 1: %.2f%%\n", (data.player1_wins / total_experiments) * 100);
  printf("Шанс на победу игрока 2: %.2f%%\n", (data.player2_wins / total_experiments) * 100);
  pthread_mutex_destroy(&data.mutex);
}