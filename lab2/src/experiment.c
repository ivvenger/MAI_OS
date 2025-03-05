#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>
#include <sys/random.h>

#include "experiment.h"

typedef struct {
    int occupied;
    int *results;
    long *threadTimes;
    pthread_mutex_t mtx;
} expRuntime;

expConfig cfg;
expRuntime vars = {0, NULL, 0};

int throw_dice() {
    int output[2];
    getrandom(output, sizeof(output), 0);
    return (output[0] % 6) + (output[1] % 6) + 2;
}

void* workerCode(void *arg) {
    int turns = cfg.rounds - cfg.currentRound;

    struct timespec start, stop;
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start);

    while (true) {
        pthread_mutex_lock(&vars.mtx);
        int cell = vars.occupied++;
        pthread_mutex_unlock(&vars.mtx);

        if (cell >= cfg.testRuns) {
            break;
        }

        int score_1 = cfg.p1Score;
        int score_2 = cfg.p2Score;
        for (int i = 0; i < turns; ++i) {
            score_1 += throw_dice();
            score_2 += throw_dice();
        }

        if (score_1 > score_2) {
            vars.results[cell] = 1;
        } else if (score_1 < score_2) {
            vars.results[cell] = 2;
        } else {
            vars.results[cell] = 0;
        }
    }

    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &stop);

    if (cfg.enableTiming) {
        long *time = (long*)arg;
        *time = 1000 * (stop.tv_sec - start.tv_sec) 
                + (stop.tv_nsec - start.tv_nsec) / 1e6;
    }

    pthread_exit(0);
}

void expFreeEnv() {
    pthread_mutex_destroy(&vars.mtx);
    if (vars.results != NULL) {
        free(vars.results);
    }
    if (vars.threadTimes != NULL) {
        free(vars.threadTimes);
    }
    vars.results = NULL;
    vars.threadTimes = NULL;
    vars.occupied = 0;
}

void expPrepareEnv(expConfig configuration) {
    cfg = configuration;
    expFreeEnv();
    if (cfg.enableTiming) {
        vars.threadTimes = malloc(sizeof(long) * cfg.threads);
    }
    vars.occupied = 0;
    vars.results = malloc(sizeof(int) * cfg.testRuns);
    pthread_mutex_init(&vars.mtx, NULL);
}

void expRun() {
    pthread_t threads[cfg.threads];
    for (int i = 0; i < cfg.threads; ++i) {
        pthread_create(&threads[i], NULL, workerCode, &vars.threadTimes[i]);
    }
    for (int i = 0; i < cfg.threads; ++i) {
        pthread_join(threads[i], NULL);
    }
}

double getPlayerProbability(int playerNum) {
    int wins = 0;
    for (int i = 0; i < cfg.testRuns; ++i) {
        wins += (vars.results[i] == playerNum);
    }
    return (double)wins / cfg.testRuns;
}

double expPlayerOneProb() {
    return getPlayerProbability(1);
}

double expPlayerTwoProb() {
    return getPlayerProbability(2);
}

long *expGetThreadTimes() {
    if (cfg.enableTiming == false)
        return NULL;
    return vars.threadTimes;
}