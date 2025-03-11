#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <unistd.h>
#include "include/lab2.h"


int main() {
   int k, current_round, player1_score, player2_score, experiments, max_threads;

   printf("Введите K, ROUND, P1SCORE, P2SCORE, EXPERIMENTS, MAXTHREADS (separated by spaces):\n");
   int scanf_result = scanf("%d %d %d %d %d %d", &k, &current_round, &player1_score, &player2_score, &experiments, &max_threads);
   if (scanf_result != 6) {
      fprintf(stderr, "Пожалуйста, введите 6 чисел, разделенных пробелами.\n");
      return 1; 
   }

   if (k <= 0 || current_round < 0 || experiments < 0 || max_threads <= 0 || player1_score < 0 || player2_score < 0) {
      fprintf(stderr, "Пожалуйста, введите положительные числа для K, ROUND, EXPERIMENTS, MAXTHREADS и неотрицательные для scores.\n");
      return 1;
   }
   if (k > INT_MAX / 2 || current_round > INT_MAX / 2 || experiments > INT_MAX / 2 || max_threads > INT_MAX / 2 || player1_score > INT_MAX / 2 || player2_score > INT_MAX / 2) {
      fprintf(stderr, "Введены слишком большие значения.\n");
      return 1;
   }

   run_simulation(k, current_round, player1_score, player2_score, experiments, max_threads);
   return 0;
}
