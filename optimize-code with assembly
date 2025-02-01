#include "raylib.h"
#include <math.h>
#include <stdio.h>
#include <time.h>  // For benchmarking
#include <x86intrin.h>  // For SIMD intrinsics

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define BALL_RADIUS 30

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Optimized path calculations using SIMD intrinsics
Vector2 CalculateStraightPath(float t) {
    __m128 t_vec = _mm_set1_ps(t);  // Broadcast t to all elements of the SIMD register
    __m128 width_vec = _mm_set1_ps(SCREEN_WIDTH);  // Broadcast SCREEN_WIDTH
    __m128 result = _mm_mul_ps(t_vec, width_vec);  // Multiply t * SCREEN_WIDTH
    float x = _mm_cvtss_f32(result);  // Extract the result
    return (Vector2){x, SCREEN_HEIGHT / 2.0f};
}

Vector2 CalculateAngularPath(float t) {
    float x, y_offset;
    float screenWidth = SCREEN_WIDTH;
    float half = 0.5f, plus_offset = 100.0f, minus_offset = -100.0f;

    // Compute x using SIMD
    __m128 t_vec = _mm_set1_ps(t);
    __m128 width_vec = _mm_set1_ps(screenWidth);
    __m128 x_result = _mm_mul_ps(t_vec, width_vec);
    x = _mm_cvtss_f32(x_result);

    // Compute y_offset using conditional logic without branches
    __m128 t_cmp = _mm_set1_ps(t);
    __m128 half_vec = _mm_set1_ps(half);
    __m128 offset_mask = _mm_cmplt_ps(t_cmp, half_vec);  // Compare t < 0.5

    // Fallback for systems without SSE4.1
    __m128 plus_vec = _mm_set1_ps(plus_offset);
    __m128 minus_vec = _mm_set1_ps(minus_offset);
    __m128 offset_result = _mm_or_ps(
        _mm_and_ps(offset_mask, minus_vec),
        _mm_andnot_ps(offset_mask, plus_vec)
    );
    y_offset = _mm_cvtss_f32(offset_result);

    return (Vector2){x, (SCREEN_HEIGHT / 2.0f) + y_offset};
}

Vector2 CalculateConvexPath(float t) {
    float x, y;
    float screenWidth = SCREEN_WIDTH, pi = (float)M_PI, neg_amp = -200.0f, offset = SCREEN_HEIGHT / 2.0f;

    // Compute y using C's sinf function
    y = offset + neg_amp * sinf(t * pi);

    // Compute x using SIMD
    __m128 t_vec = _mm_set1_ps(t);
    __m128 width_vec = _mm_set1_ps(screenWidth);
    __m128 x_result = _mm_mul_ps(t_vec, width_vec);
    x = _mm_cvtss_f32(x_result);

    return (Vector2){x, y};
}

Vector2 CalculateSinusoidalPath(float t) {
    float x, y;
    float screenWidth = SCREEN_WIDTH, four_pi = 4.0f * M_PI, amplitude = 100.0f, offset = SCREEN_HEIGHT / 2.0f;

    // Compute y using C's sinf function
    y = offset + amplitude * sinf(t * four_pi);

    // Compute x using SIMD
    __m128 t_vec = _mm_set1_ps(t);
    __m128 width_vec = _mm_set1_ps(screenWidth);
    __m128 x_result = _mm_mul_ps(t_vec, width_vec);
    x = _mm_cvtss_f32(x_result);

    return (Vector2){x, y};
}

// Benchmark Function
void BenchmarkPathFunctions() {
    clock_t start, end;
    const int iterations = 100000;  // Increase iterations for more accurate timing
    
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
