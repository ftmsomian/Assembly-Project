#include "raylib.h"
#include <math.h>
#include <stdio.h>
#include <time.h>  // For benchmarking

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define BALL_RADIUS 30

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Vector2 CalculateStraightPath(float t) {
    return (Vector2){t * SCREEN_WIDTH, SCREEN_HEIGHT / 2};
}

Vector2 CalculateAngularPath(float t) {
    if (t < 0.5f) {
        return (Vector2){t * SCREEN_WIDTH, SCREEN_HEIGHT / 2 - 100};
    } else {
        return (Vector2){t * SCREEN_WIDTH, SCREEN_HEIGHT / 2 + 100};
    }
}

Vector2 CalculateConvexPath(float t) {
    float y = SCREEN_HEIGHT / 2 - 200 * sinf(t * PI);
    return (Vector2){t * SCREEN_WIDTH, y};
}

Vector2 CalculateSinusoidalPath(float t) {
    float y = SCREEN_HEIGHT / 2 + 100 * sinf(t * 4 * PI);
    return (Vector2){t * SCREEN_WIDTH, y};
}

// Benchmark Function
void BenchmarkPathFunctions() {
    clock_t start, end;
    const int iterations = 10000000;  // Increase iterations for more accurate timing
    
    start = clock();
    for (int i = 0; i < iterations; i++) {
        for (float t = 0.0f; t < 1.0f; t += 0.001f) {
            CalculateStraightPath(t);
        }
    }
    end = clock();
    printf("Straight Path Execution Time: %lf seconds\n", ((double)(end - start)) / CLOCKS_PER_SEC);

    start = clock();
    for (int i = 0; i < iterations; i++) {
        for (float t = 0.0f; t < 1.0f; t += 0.001f) {
            CalculateAngularPath(t);
        }
    }
    end = clock();
    printf("Angular Path Execution Time: %lf seconds\n", ((double)(end - start)) / CLOCKS_PER_SEC);

    start = clock();
    for (int i = 0; i < iterations; i++) {
        for (float t = 0.0f; t < 1.0f; t += 0.001f) {
            CalculateConvexPath(t);
        }
    }
    end = clock();
    printf("Convex Path Execution Time: %lf seconds\n", ((double)(end - start)) / CLOCKS_PER_SEC);

    start = clock();
    for (int i = 0; i < iterations; i++) {
        for (float t = 0.0f; t < 1.0f; t += 0.001f) {
            CalculateSinusoidalPath(t);
        }
    }
    end = clock();
    printf("Sinusoidal Path Execution Time: %lf seconds\n", ((double)(end - start)) / CLOCKS_PER_SEC);
}

int main() {
    BenchmarkPathFunctions();
    return 0;
}
