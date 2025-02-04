#include "raylib.h"
#include <math.h>
#include <stdio.h>
#include <time.h>  // For benchmarking
#include <stdint.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define BALL_RADIUS 30
#define PATH_STRAIGHT 0
#define PATH_ANGULAR 1
#define PATH_CONVEX 2
#define PATH_SINUSOIDAL 3

// Precompute constants
static const float HALF_SCREEN_HEIGHT = SCREEN_HEIGHT / 2.0f;
static const float HALF_SCREEN_HEIGHT_MINUS_100 = HALF_SCREEN_HEIGHT - 100.0f;
static const float HALF_SCREEN_HEIGHT_PLUS_100 = HALF_SCREEN_HEIGHT + 100.0f;

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Ball structure
typedef struct {
    Vector2 position;
    float rotation;
    Color colors[6];
    int colorCount;
} Ball;

// Function to draw a striped ball
void DrawStripedBall(Ball ball) {
    for (int i = 0; i < ball.colorCount; i++) {
        float angleStart = ball.rotation + (i * 360.0f / ball.colorCount);
        float angleEnd = angleStart + (360.0f / ball.colorCount);
        DrawCircleSector(ball.position, BALL_RADIUS, angleStart, angleEnd, 0, ball.colors[i]);
    }
}

// Path calculation functions with inline assembly
Vector2 CalculateStraightPath(float t) {
    Vector2 result;
    float screen_width = SCREEN_WIDTH;  // Store in local variable

    __asm__ (
        "mulss %[screen_width], %[t]\n\t"
        : [t] "+x"(t)
        : [screen_width] "x"(screen_width)  // Use register constraint
        : "cc"
    );

    result.x = t;
    result.y = HALF_SCREEN_HEIGHT;
    return result;
}


#include <immintrin.h> // Include for SIMD intrinsics

Vector2 CalculateAngularPath(float t) {
    // Load the scalar 't' into a SIMD register
    __m128 t_vec = _mm_set1_ps(t);

    // Load constants into SIMD registers
    __m128 screen_width_vec = _mm_set1_ps(SCREEN_WIDTH);
    __m128 screen_height_vec = _mm_set1_ps(SCREEN_HEIGHT);
    __m128 one_vec = _mm_set1_ps(1.0f);

    // Compute t * SCREEN_WIDTH
    __m128 x_result = _mm_mul_ps(t_vec, screen_width_vec);

    // Compute (1.0f - t) * SCREEN_HEIGHT
    __m128 y_result = _mm_mul_ps(_mm_sub_ps(one_vec, t_vec), screen_height_vec);

    // Extract the results from the SIMD registers
    Vector2 result;
    _mm_store_ss(&result.x, x_result);
    _mm_store_ss(&result.y, y_result);

    return result;
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
void BenchmarkPathFunctions(double *straightTime, double *angularTime, double *convexTime, double *sinusoidalTime) {
    clock_t start, end;
    const int iterations = 10000000;  // Increase iterations for more accurate timing
    
    start = clock();
    for (int i = 0; i < iterations; i++) {
        for (float t = 0.0f; t < 1.0f; t += 0.001f) {
            CalculateStraightPath(t);
        }
    }
    end = clock();
    *straightTime = ((double)(end - start)) / CLOCKS_PER_SEC;

    start = clock();
    for (int i = 0; i < iterations; i++) {
        for (float t = 0.0f; t < 1.0f; t += 0.001f) {
            CalculateAngularPath(t);
        }
    }
    end = clock();
    *angularTime = ((double)(end - start)) / CLOCKS_PER_SEC;

    start = clock();
    for (int i = 0; i < iterations; i++) {
        for (float t = 0.0f; t < 1.0f; t += 0.001f) {
            CalculateConvexPath(t);
        }
    }
    end = clock();
    *convexTime = ((double)(end - start)) / CLOCKS_PER_SEC;

    start = clock();
    for (int i = 0; i < iterations; i++) {
        for (float t = 0.0f; t < 1.0f; t += 0.001f) {
            CalculateSinusoidalPath(t);
        }
    }
    end = clock();
    *sinusoidalTime = ((double)(end - start)) / CLOCKS_PER_SEC;
}

int main() {
    // Initialize Raylib window
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Colorful Striped Ball Paths with Benchmark");
    SetTargetFPS(60);

    // Ball properties
    Ball ball = {
        .position = {0, SCREEN_HEIGHT / 2},
        .rotation = 0,
        .colors = {RED, ORANGE, YELLOW, GREEN, BLUE, PURPLE},
        .colorCount = 6
    };

    // Path selection and movement
    int selectedPath = PATH_STRAIGHT;
    float t = 0.0f;
    bool isMoving = false;

    // Benchmark results
    double straightTime = 0.0, angularTime = 0.0, convexTime = 0.0, sinusoidalTime = 0.0;
    bool benchmarkDone = false;

    // Main game loop
    while (!WindowShouldClose()) {
        // Handle user input
        if (IsKeyPressed(KEY_ONE)) selectedPath = PATH_STRAIGHT;
        if (IsKeyPressed(KEY_TWO)) selectedPath = PATH_ANGULAR;
        if (IsKeyPressed(KEY_THREE)) selectedPath = PATH_CONVEX;
        if (IsKeyPressed(KEY_FOUR)) selectedPath = PATH_SINUSOIDAL;
        if (IsKeyPressed(KEY_SPACE)) isMoving = !isMoving;

        // Update ball position and rotation
        if (isMoving) {
            t += 0.01f;  // Increment t for continuous movement
            if (t > 1.0f) t = 0.0f;  // Reset t to 0 when it exceeds 1.0 for looping

            switch (selectedPath) {
                case PATH_STRAIGHT:
                    ball.position = CalculateStraightPath(t);
                    break;
                case PATH_ANGULAR:
                    ball.position = CalculateAngularPath(t);
                    break;
                case PATH_CONVEX:
                    ball.position = CalculateConvexPath(t);
                    break;
                case PATH_SINUSOIDAL:
                    ball.position = CalculateSinusoidalPath(t);
                    break;
            }
            ball.rotation += 5.0f; // Rotate the ball
        }

        // Run benchmark if not already done
        if (!benchmarkDone) {
            BenchmarkPathFunctions(&straightTime, &angularTime, &convexTime, &sinusoidalTime);
            benchmarkDone = true;
        }

        // Draw everything
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Draw the goal/wall
        DrawRectangle(SCREEN_WIDTH - 10, 0, 10, SCREEN_HEIGHT, BLACK);

        // Draw the ball
        DrawStripedBall(ball);

        // Draw benchmark results
        DrawText(TextFormat("Straight Path: %.6f seconds", straightTime), 10, 10, 20, DARKGRAY);
        DrawText(TextFormat("Angular Path: %.6f seconds", angularTime), 10, 40, 20, DARKGRAY);
        DrawText(TextFormat("Convex Path: %.6f seconds", convexTime), 10, 70, 20, DARKGRAY);
        DrawText(TextFormat("Sinusoidal Path: %.6f seconds", sinusoidalTime), 10, 100, 20, DARKGRAY);

        // Draw instructions
        DrawText("Press 1: Straight Path", 10, SCREEN_HEIGHT - 120, 20, DARKGRAY);
        DrawText("Press 2: Angular Path", 10, SCREEN_HEIGHT - 90, 20, DARKGRAY);
        DrawText("Press 3: Convex Path", 10, SCREEN_HEIGHT - 60, 20, DARKGRAY);
        DrawText("Press 4: Sinusoidal Path", 10, SCREEN_HEIGHT - 30, 20, DARKGRAY);
        DrawText("Press SPACE: Start/Stop", 10, SCREEN_HEIGHT - 150, 20, DARKGRAY);

        EndDrawing();
    }

    // Close Raylib window
    CloseWindow();
    return 0;
}
