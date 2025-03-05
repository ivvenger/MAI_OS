#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../include/experiment.h"

int main(int argc, char *argv[]) {
    expConfig config;
    config.enableTiming = false;

    if (argc > 1) {
        config.threads = atoi(argv[1]);
    } else {
        config.threads = 1;
    }

    printf("Using %d threads\n", config.threads);
    printf("Enter duration, current round, the two scores and the run count\n");
    scanf("%d %d %d %d %d", &config.rounds, &config.currentRound, 
        &config.p1Score, &config.p2Score, &config.testRuns);

    expPrepareEnv(config);
    expRun();

    printf("Probability of player one: %lf\n", expPlayerOneProb());
    printf("\t\tplayer two: %lf\n", expPlayerTwoProb());

    expFreeEnv();
    return 0;
} 