#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "experiment.h"

int compare(const void *a, const void *b) {
    return (*(long *)a - *(long *)b);
}

long calculateMedian(long *data, size_t size) {
    if (size % 2 == 1) {
        return (double)data[size / 2];
    } else {
        return (double)(data[(size / 2) - 1] + data[size / 2]) / 2.0;
    }
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("usage: %s max-thread-count iterations\n", argv[0]);
        return 1;
    }
    int maxThreads = atoi(argv[1]);
    int iterations = atoi(argv[2]);

    expConfig config = {25, 0, 0, 0, iterations, 0, true};

    printf("Starting the tests...\n");
    printf("Keep in mind that the times shown are in nanoseconds...\n");

    for (int i = 1; i <= maxThreads; ++i) {
        config.threads = i;
        expPrepareEnv(config);
        expRun();

        long *threadTimes = expGetThreadTimes();
        qsort(threadTimes, i, sizeof(threadTimes), compare);

        printf("\n=> config.threads = %d\n", config.threads);
        printf("\tMedian: %ld\n", calculateMedian(threadTimes, i));
        printf("\tWorst time: %ld\n", threadTimes[i - 1]);
    }

    expFreeEnv();
    return 0;
}