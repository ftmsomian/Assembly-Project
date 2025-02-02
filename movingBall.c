#include "raylib.h"
#include <math.h>
#include <stdio.h>
#include <time.h>  // For benchmarking

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define BALL_RADIUS 30
#define PATH_STRAIGHT 0
#define PATH_ANGULAR 1
#define PATH_CONVEX 2
#define PATH_SINUSOIDAL 3

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

// Path calculation functions
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
    float y = SCREEN_HEIGHT / 2 - 200 * sinf(t * M_PI);
    return (Vector2){t * SCREEN_WIDTH, y};
}

Vector2 CalculateSinusoidalPath(float t) {
    float y = SCREEN_HEIGHT / 2 + 100 * sinf(t * 4 * M_PI);
    return (Vector2){t * SCREEN_WIDTH, y};
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
