#include "raylib.h"
#include <math.h>

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

Vector2 CalculateSinusoidalPath(float t) {
    float y = SCREEN_HEIGHT / 2 + 100 * sinf(t * 4 * PI);
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

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        // Handle user input
        if (IsKeyPressed(KEY_ONE)) selectedPath = PATH_STRAIGHT;
        if (IsKeyPressed(KEY_TWO)) selectedPath = PATH_ANGULAR;
        if (IsKeyPressed(KEY_THREE)) selectedPath = PATH_CONVEX;
        if (IsKeyPressed(KEY_FOUR)) selectedPath = PATH_SINUSOIDAL;
        if (IsKeyPressed(KEY_SPACE)) isMoving = !isMoving;

        // Update ball position and rotation
        if (isMoving && t < 1.0f) {
            t += 0.01f;
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

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
