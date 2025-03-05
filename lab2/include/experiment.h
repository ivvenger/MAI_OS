#ifndef _EXPERIMENT_H_
#define _EXPERIMENT_H_

#include <stdbool.h>

typedef struct {
    int rounds;
    int currentRound;
    int p1Score;
    int p2Score;
    int testRuns;
    int threads;
    bool enableTiming;

} expConfig;

void expPrepareEnv(expConfig configuration);
void expRun();
double expPlayerOneProb();
double expPlayerTwoProb();
long* expGetThreadTimes();
void expFreeEnv();

#endif