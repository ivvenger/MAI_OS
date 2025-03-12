#include <math.h>
#include "func.h"

// Первая реализация производной
float Derivative(float A, float deltaX) {
    return (cos(A + deltaX) - cos(A)) / deltaX;
}

// Первая реализация e
float E(int x) {
    return pow(1 + 1.0 / x, x);
}
