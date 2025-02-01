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

// Optimized path calculations using inline assembly
Vector2 CalculateStraightPath(float t) {
    float x;
    float screenWidth = SCREEN_WIDTH;  // Load SCREEN_WIDTH into a local variable
    __asm__ volatile (
        "mulss %[screenWidth], %[t]\n"  // t * SCREEN_WIDTH
        "movss %[t], %[x]\n"            // Store result in x
        : [x] "=m" (x)
        : [t] "x" (t), [screenWidth] "x" (screenWidth)
        : 
    );
    return (Vector2){x, SCREEN_HEIGHT / 2.0f};
}

Vector2 CalculateAngularPath(float t) {
    float x, y_offset;
    float screenWidth = SCREEN_WIDTH;  // Load SCREEN_WIDTH into a local variable
    float half = 0.5f, plus_offset = 100.0f, minus_offset = -100.0f;

    __asm__ volatile (
        // Compute x = t * SCREEN_WIDTH
        "mulss %[screenWidth], %[t]\n"
        "movss %[t], %[x]\n"

        // Compute y_offset based on t < 0.5
        "movss %[t], %%xmm0\n"
        "ucomiss %[half], %%xmm0\n"
        "jb 1f\n"
        "movss %[plus_offset], %[y_offset]\n"
        "jmp 2f\n"
        "1: movss %[minus_offset], %[y_offset]\n"
        "2:\n"

        : [x] "=m" (x), [y_offset] "=m" (y_offset)
        : [t] "x" (t), [screenWidth] "x" (screenWidth),
          [half] "x" (half), [plus_offset] "x" (plus_offset), [minus_offset] "x" (minus_offset)
        : "xmm0"
    );

    return (Vector2){x, (SCREEN_HEIGHT / 2.0f) + y_offset};
}

Vector2 CalculateConvexPath(float t) {
    float x, y;
    float screenWidth = SCREEN_WIDTH;  // Load SCREEN_WIDTH into a local variable
    float pi = (float)M_PI, neg_amp = -200.0f, offset = SCREEN_HEIGHT / 2.0f;

    // Compute y using C's sinf function
    y = offset + neg_amp * sinf(t * pi);

    __asm__ volatile (
        // Compute x = t * SCREEN_WIDTH
        "mulss %[screenWidth], %[t]\n"
        "movss %[t], %[x]\n"

        : [x] "=m" (x)
        : [t] "x" (t), [screenWidth] "x" (screenWidth)
        : 
    );

    return (Vector2){x, y};
}

Vector2 CalculateSinusoidalPath(float t) {
    float x, y;
    float screenWidth = SCREEN_WIDTH;  // Load SCREEN_WIDTH into a local variable
    float four_pi = 4.0f * M_PI, amplitude = 100.0f, offset = SCREEN_HEIGHT / 2.0f;

    // Compute y using C's sinf function
    y = offset + amplitude * sinf(t * four_pi);

    __asm__ volatile (
        // Compute x = t * SCREEN_WIDTH
        "mulss %[screenWidth], %[t]\n"
        "movss %[t], %[x]\n"

        : [x] "=m" (x)
        : [t] "x" (t), [screenWidth] "x" (screenWidth)
        : 
    );

    return (Vector2){x, y};
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
