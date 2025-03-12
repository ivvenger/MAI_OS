#include <math.h>
#include "func.h"

// Вторая реализация производной
float Derivative(float A, float deltaX) {
    return (cos(A + deltaX) - cos(A - deltaX)) / (2 * deltaX);
}

// Вторая реализация e
float E(int x) {
    float result = 1.0, term = 1.0;
    for (int n = 1; n <= x; n++) {
        term /= n;
        result += term;
    }
    return result;
}