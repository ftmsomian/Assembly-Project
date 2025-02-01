#include "raylib.h"
#include <math.h>
#include <stdio.h>     // For snprintf

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define BALL_RADIUS 30
#define PATH_STRAIGHT 0
#define PATH_ANGULAR 1
#define PATH_CONVEX 2
#define PATH_SINUSOIDAL 3

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

// Sinusoidal path calculation using x86-64 inline assembly
Vector2 CalculateSinusoidalPath(float t) {
    float y;
    float four_pi = 4.0f * PI;
    float amplitude = 100.0f;
    float offset = SCREEN_HEIGHT / 2.0f;

    // Inline assembly to compute y = offset + amplitude * sin(t * four_pi)
    __asm__ volatile (
        "movss %[t], %%xmm0\n"          // Load t into xmm0
        "mulss %[four_pi], %%xmm0\n"    // Multiply xmm0 by four_pi (t * 4 * PI)
        "call sinf\n"                   // Call sinf function (result in xmm0)
        "mulss %[amplitude], %%xmm0\n"  // Multiply result by amplitude
        "addss %[offset], %%xmm0\n"     // Add offset to the result
        "movss %%xmm0, %[y]\n"          // Store the result in y
        : [y] "=m" (y)                  // Output
        : [t] "m" (t), [four_pi] "m" (four_pi), [amplitude] "m" (amplitude), [offset] "m" (offset) // Inputs
        : "xmm0", "memory"              // Clobbered registers and memory
    );

    return (Vector2){t * SCREEN_WIDTH, y};
}

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Colorful Striped Ball Paths");

    Ball ball = {
        .position = {0, SCREEN_HEIGHT / 2},
        .rotation = 0,
        .colors = {RED, ORANGE, YELLOW, GREEN, BLUE, PURPLE},
        .colorCount = 6
    };

    int selectedPath = PATH_STRAIGHT;
    float t = 0.0f;
    bool isMoving = false;
    double startTime = 0.0;
    double endTime = 0.0;
    bool hasStopped = false;

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        // Handle user input
        if (IsKeyPressed(KEY_ONE)) selectedPath = PATH_STRAIGHT;
        if (IsKeyPressed(KEY_TWO)) selectedPath = PATH_ANGULAR;
        if (IsKeyPressed(KEY_THREE)) selectedPath = PATH_CONVEX;
        if (IsKeyPressed(KEY_FOUR)) selectedPath = PATH_SINUSOIDAL;
        if (IsKeyPressed(KEY_SPACE)) {
            isMoving = !isMoving;
            if (isMoving) {
                startTime = GetTime(); // Record start time when movement begins
                hasStopped = false;
            } else {
                endTime = GetTime(); // Record end time when movement stops
                hasStopped = true;
            }
        }

        // Update ball position and rotation
        if (isMoving) {
            t += 0.01f;
            if (t >= 1.0f) {
                t = 0.0f; // Reset t to 0 to repeat the movement
            }
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

        // Draw everything
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Draw the goal/wall
        DrawRectangle(SCREEN_WIDTH - 10, 0, 10, SCREEN_HEIGHT, BLACK);

        // Draw the ball
        DrawStripedBall(ball);

        // Draw instructions
        DrawText("Press 1: Straight Path", 10, 10, 20, DARKGRAY);
        DrawText("Press 2: Angular Path", 10, 40, 20, DARKGRAY);
        DrawText("Press 3: Convex Path", 10, 70, 20, DARKGRAY);
        DrawText("Press 4: Sinusoidal Path", 10, 100, 20, DARKGRAY);
        DrawText("Press SPACE: Start/Stop", 10, 130, 20, DARKGRAY);

        // Display execution time if the ball has stopped
        if (hasStopped) {
            double elapsedTime = endTime - startTime;
            char timeText[50];
            snprintf(timeText, sizeof(timeText), "Execution Time: %.2f seconds", elapsedTime);
            DrawText(timeText, 10, SCREEN_HEIGHT - 30, 20, DARKGRAY);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
