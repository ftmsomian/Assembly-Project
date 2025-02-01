#include "raylib.h"
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <string.h>  // Include for strlen

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define BALL_RADIUS 30
#define PATH_STRAIGHT 0
#define PATH_ANGULAR 1
#define PATH_CONVEX 2
#define PATH_SINUSOIDAL 3

#define PI 3.14159265358979323846f  // Define PI if it's not already available

typedef struct {
    Vector2 position;
    float rotation;
    Color colors[6];
    int colorCount;
} Ball;

void DrawStripedBall(Ball ball) {
    for (int i = 0; i < ball.colorCount; i++) {
        float angleStart = ball.rotation + (i * 360.0f / ball.colorCount);
        float angleEnd = angleStart + (360.0f / ball.colorCount);
        DrawCircleSector(ball.position, BALL_RADIUS, angleStart, angleEnd, 0, ball.colors[i]);
    }
}

Vector2 CalculateStraightPath(float t) {
    Vector2 result;
    float width = SCREEN_WIDTH;
    float mid = SCREEN_HEIGHT / 2.0f;
    __asm__ volatile (
        "movss %[t], %%xmm0\n"       // Load t into xmm0
        "mulss %[width], %%xmm0\n"   // Multiply t by width
        "movss %%xmm0, %[x]\n"       // Store result into result.x
        "movss %[mid], %%xmm1\n"     // Load mid into xmm1
        "movss %%xmm1, %[y]\n"       // Store result into result.y
        : [x] "=m" (result.x), [y] "=m" (result.y)
        : [t] "m" (t), [width] "m" (width), [mid] "m" (mid)
        : "xmm0", "xmm1", "memory"
    );
    return result;
}

Vector2 CalculateAngularPath(float t) {
    Vector2 result;
    float width = SCREEN_WIDTH;
    float mid = SCREEN_HEIGHT / 2.0f;
    float offset = (t < 0.5f) ? -100.0f : 100.0f;
    __asm__ volatile (
        "movss %[t], %%xmm0\n"
        "mulss %[width], %%xmm0\n"
        "movss %[mid], %%xmm1\n"
        "addss %[offset], %%xmm1\n"
        "movss %%xmm0, %[x]\n"
        "movss %%xmm1, %[y]\n"
        : [x] "=m" (result.x), [y] "=m" (result.y)
        : [t] "m" (t), [width] "m" (width), [mid] "m" (mid), [offset] "m" (offset)
        : "xmm0", "xmm1", "memory"
    );
    return result;
}

Vector2 CalculateConvexPath(float t) {
    Vector2 result;
    float y;
    float pi = PI;
    float amp = -200.0f;
    float mid = SCREEN_HEIGHT / 2.0f;
    __asm__ volatile (
        "movss %[t], %%xmm0\n"          // Load t into xmm0
        "mulss %[pi], %%xmm0\n"         // Multiply t by PI
        "call sinf\n"                   // Call sinf function (result in xmm0)
        "mulss %[amp], %%xmm0\n"        // Multiply result by amplitude
        "addss %[mid], %%xmm0\n"        // Add offset to the result
        "movss %%xmm0, %[y]\n"          // Store the result in y
        : [y] "=m" (y)
        : [t] "m" (t), [pi] "m" (pi), [amp] "m" (amp), [mid] "m" (mid)
        : "xmm0", "memory"
    );
    result.x = t * SCREEN_WIDTH;
    result.y = y;
    return result;
}

Vector2 CalculateSinusoidalPath(float t) {
    Vector2 result;
    float y;
    float four_pi = 4.0f * PI;
    float amplitude = 100.0f;
    float offset = SCREEN_HEIGHT / 2.0f;
    __asm__ volatile (
        "movss %[t], %%xmm0\n"          // Load t into xmm0
        "mulss %[four_pi], %%xmm0\n"    // Multiply t by 4 * PI
        "call sinf\n"                   // Call sinf function (result in xmm0)
        "mulss %[amplitude], %%xmm0\n"  // Multiply result by amplitude
        "addss %[offset], %%xmm0\n"     // Add offset to the result
        "movss %%xmm0, %[y]\n"          // Store the result in y
        : [y] "=m" (y)
        : [t] "m" (t), [four_pi] "m" (four_pi), [amplitude] "m" (amplitude), [offset] "m" (offset)
        : "xmm0", "memory"
    );
    result.x = t * SCREEN_WIDTH;
    result.y = y;
    return result;
}

void BenchmarkPathFunctions(char *output) {
    clock_t start, end;
    
    // Benchmark for Straight Path
    start = clock();
    for (int i = 0; i < 2; i++) {
        for (float t = 0.0f; t < 1.0f; t += 0.001f) {
            CalculateStraightPath(t);
        }
    }
    end = clock();
    sprintf(output, "Straight Path: %lf seconds", ((double)(end - start)) / CLOCKS_PER_SEC);
    
    // Benchmark for Angular Path
    start = clock();
    for (int i = 0; i < 2; i++) {
        for (float t = 0.0f; t < 1.0f; t += 0.001f) {
            CalculateAngularPath(t);
        }
    }
    end = clock();
    sprintf(output + strlen(output), "\nAngular Path: %lf seconds", ((double)(end - start)) / CLOCKS_PER_SEC);
    
    // Benchmark for Convex Path
    start = clock();
    for (int i = 0; i < 2; i++) {
        for (float t = 0.0f; t < 1.0f; t += 0.001f) {
            CalculateConvexPath(t);
        }
    }
    end = clock();
    sprintf(output + strlen(output), "\nConvex Path: %lf seconds", ((double)(end - start)) / CLOCKS_PER_SEC);
    
    // Benchmark for Sinusoidal Path
    start = clock();
    for (int i = 0; i < 2; i++) {
        for (float t = 0.0f; t < 1.0f; t += 0.001f) {
            CalculateSinusoidalPath(t);
        }
    }
    end = clock();
    sprintf(output + strlen(output), "\nSinusoidal Path: %lf seconds", ((double)(end - start)) / CLOCKS_PER_SEC);
}

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Benchmark and Graphics");

    Ball ball = {
        .position = {0, SCREEN_HEIGHT / 2},
        .rotation = 0,
        .colors = {RED, ORANGE, YELLOW, GREEN, BLUE, PURPLE},
        .colorCount = 6
    };
    
    int selectedPath = PATH_STRAIGHT;
    float t = 0.0f;
    bool isMoving = true;  // Set this to true to move the ball
    char benchmarkOutput[256] = "";  // To store benchmark results
    
    SetTargetFPS(60);
    
    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_ONE)) selectedPath = PATH_STRAIGHT;
        if (IsKeyPressed(KEY_TWO)) selectedPath = PATH_ANGULAR;
        if (IsKeyPressed(KEY_THREE)) selectedPath = PATH_CONVEX;
        if (IsKeyPressed(KEY_FOUR)) selectedPath = PATH_SINUSOIDAL;  // Add option for Sinusoidal path
        if (IsKeyPressed(KEY_B)) {
            benchmarkOutput[0] = '\0';  // Clear the previous output
            BenchmarkPathFunctions(benchmarkOutput);
        }
        
        if (isMoving) {
            t += 0.01f;
            if (t >= 1.0f) t = 0.0f;
            switch (selectedPath) {
                case PATH_STRAIGHT: ball.position = CalculateStraightPath(t); break;
                case PATH_ANGULAR: ball.position = CalculateAngularPath(t); break;
                case PATH_CONVEX: ball.position = CalculateConvexPath(t); break;
                case PATH_SINUSOIDAL: ball.position = CalculateSinusoidalPath(t); break;  // Sinusoidal path
            }
            ball.rotation += 5.0f;
        }
        
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawStripedBall(ball);
        DrawText("Press 1: Straight, 2: Angular, 3: Convex, 4: Sinusoidal", 10, 10, 20, DARKGRAY);
        DrawText("Press B: Run Benchmark", 10, 40, 20, DARKGRAY);
        
        if (benchmarkOutput[0] != '\0') {
            DrawText(benchmarkOutput, 10, 70, 20, DARKGRAY);  // Display benchmark results
        }
        
        EndDrawing();
    }
    
    CloseWindow();
    return 0;
}
