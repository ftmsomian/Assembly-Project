// Define macros to prevent Windows from including certain definitions
#define WIN32_LEAN_AND_MEAN // Prevent unnecessary includes like min/max
#define NOMINMAX            // Disable the min/max macros to avoid conflicts
#define NOGDI               // Exclude GDI definitions to avoid conflicting graphical functions
#define NOUSER              // Exclude user32 (avoiding `DrawTextA` and `LoadImageA` issues)
#include <raylib.h>         // Include Raylib first

// Explicitly undefine conflicting functions from Windows
#undef LoadImageA
#undef DrawTextA
#undef DrawTextExA
#undef CloseWindow
#undef ShowCursor

// Provide function aliases if necessary
void CloseWindowWin(void) {
    CloseWindow(); // Call the Raylib CloseWindow function
}

// Now include Windows headers (after Raylib)
#include <windows.h>        // Include Windows headers

#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdint.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define BALL_RADIUS 30
#define RACKET_WIDTH 10
#define RACKET_HEIGHT 100
#define PATH_STRAIGHT 0
#define PATH_ANGULAR 1
#define PATH_CONVEX 2
#define PATH_SINUSOIDAL 3

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// High-precision timer function (renamed to avoid Raylib conflict)
static double GetHighPrecisionTime() {
#ifdef _WIN32
    LARGE_INTEGER frequency;
    LARGE_INTEGER currentTime;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&currentTime);
    return (double)currentTime.QuadPart / frequency.QuadPart;
#else
    struct timeval currentTime;
    gettimeofday(&currentTime, NULL);
    return (double)currentTime.tv_sec + (double)currentTime.tv_usec / 1e6;
#endif
}

// Ball structure
typedef struct {
    Vector2 position;
    float rotation;
    Color colors[6];
    int colorCount;
} Ball;

// Racket structure
typedef struct {
    float x, y;
    float width, height;
} Racket;

// Function to draw a striped ball
void DrawStripedBall(Ball ball) {
    for (int i = 0; i < ball.colorCount; i++) {
        float angleStart = ball.rotation + (i * 360.0f / ball.colorCount);
        float angleEnd = angleStart + (360.0f / ball.colorCount);
        DrawCircleSector(ball.position, BALL_RADIUS, angleStart, angleEnd, 10, ball.colors[i]);
    }
}

// Path calculation functions
Vector2 CalculateStraightPath(float t, double *executionTime) {
    double start = GetHighPrecisionTime();
    Vector2 result = (Vector2){t * SCREEN_WIDTH, SCREEN_HEIGHT / 2};
    double end = GetHighPrecisionTime();
    *executionTime += (end - start);
    return result;
}

Vector2 CalculateAngularPath(float t, double *executionTime) {
    double start = GetHighPrecisionTime();
    Vector2 result = (Vector2){t * SCREEN_WIDTH, SCREEN_HEIGHT * (1.0f - t)};
    double end = GetHighPrecisionTime();
    *executionTime += (end - start);
    return result;
}

Vector2 CalculateConvexPath(float t, double *executionTime) {
    double start = GetHighPrecisionTime();
    float x, y;
    float screenWidth = SCREEN_WIDTH, pi = (float)M_PI, neg_amp = -200.0f, offset = SCREEN_HEIGHT / 2.0f;

    // Introduce unnecessary loop for inefficiency
    for (int i = 0; i < 10; i++) {
        y = offset + neg_amp * sinf(t * pi);
    }

    // Perform inefficient calculations for x
    x = 0.0f;
    for (int i = 0; i < 1000; i++) {
        x += t * screenWidth / 1000.0f; // Inefficient summation
    }

    Vector2 result = {x, y};
    double end = GetHighPrecisionTime();
    *executionTime += (end - start);
    return result;
}

Vector2 CalculateSinusoidalPath(float t, double *executionTime) {
    double start = GetHighPrecisionTime();
    float x, y;
    float screenWidth = SCREEN_WIDTH, four_pi = 4.0f * M_PI, amplitude = 100.0f, offset = SCREEN_HEIGHT / 2.0f;

    // Inefficient power operation
    y = offset + pow(amplitude * sinf(t * four_pi), 1.0f);

    // Use inefficient logic for x
    x = 0.0f;
    for (int i = 0; i < 500; i++) {
        x += t * screenWidth / 500.0f; // Slow summation
    }

    Vector2 result = {x, y};
    double end = GetHighPrecisionTime();
    *executionTime += (end - start);
    return result;
}

int main() {
    // Initialize Raylib window
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Ball Game with Racket and Paths");
    SetTargetFPS(60);

    // Ball properties
    Ball ball = {
        .position = {0, SCREEN_HEIGHT / 2},
        .rotation = 0,
        .colors = {RED, ORANGE, YELLOW, GREEN, BLUE, PURPLE},
        .colorCount = 6
    };

    // Racket properties
    Racket racket = {SCREEN_WIDTH - RACKET_WIDTH - 10, SCREEN_HEIGHT / 2 - RACKET_HEIGHT / 2, RACKET_WIDTH, RACKET_HEIGHT};

    // Execution time tracking
    double straightTime = 0.0, angularTime = 0.0, convexTime = 0.0, sinusoidalTime = 0.0;

    // Calculate execution time for each path
    for (float t = 0.0f; t <= 1.0f; t += 0.01f) {
        double tempTime = 0.0;
        CalculateStraightPath(t, &tempTime);
        straightTime += tempTime;

        tempTime = 0.0;
        CalculateAngularPath(t, &tempTime);
        angularTime += tempTime;

        tempTime = 0.0;
        CalculateConvexPath(t, &tempTime);
        convexTime += tempTime;

        tempTime = 0.0;
        CalculateSinusoidalPath(t, &tempTime);
        sinusoidalTime += tempTime;
    }

    // Main game loop
    int selectedPath = PATH_STRAIGHT;
    float t = 0.0f;
    bool isMoving = false;
    bool directionRight = true; // Direction of the ball

    while (!WindowShouldClose()) {
        // Handle user input
        if (IsKeyPressed(KEY_ONE)) selectedPath = PATH_STRAIGHT;
        if (IsKeyPressed(KEY_TWO)) selectedPath = PATH_ANGULAR;
        if (IsKeyPressed(KEY_THREE)) selectedPath = PATH_CONVEX;
        if (IsKeyPressed(KEY_FOUR)) selectedPath = PATH_SINUSOIDAL;
        if (IsKeyPressed(KEY_SPACE)) isMoving = !isMoving;

        // Update ball position and rotation
        if (isMoving) {
            t += (directionRight ? 0.01f : -0.01f);
            if (t > 1.0f || t < 0.0f) {
                directionRight = !directionRight; // Reverse direction
                t = directionRight ? 0.0f : 1.0f;
            }

            switch (selectedPath) {
                case PATH_STRAIGHT:
                    ball.position = CalculateStraightPath(t, &straightTime);
                    break;
                case PATH_ANGULAR:
                    ball.position = CalculateAngularPath(t, &angularTime);
                    break;
                case PATH_CONVEX:
                    ball.position = CalculateConvexPath(t, &convexTime);
                    break;
                case PATH_SINUSOIDAL:
                    ball.position = CalculateSinusoidalPath(t, &sinusoidalTime);
                    break;
            }
            ball.rotation += 5.0f;

            // Handle collision with racket
            // Handle collision with racket
            if (ball.position.x + BALL_RADIUS >= racket.x &&
                ball.position.y >= racket.y &&
                ball.position.y <= racket.y + racket.height) {
                ball.position.x = racket.x - BALL_RADIUS; // Adjust position to avoid overlap
                directionRight = !directionRight; // Reverse direction
                t = directionRight ? 0.0f : 1.0f; // Reset t for movement in the new direction
            }
        }

        // Handle racket movement
        if (IsKeyDown(KEY_UP) && racket.y > 0) racket.y -= 400 * GetFrameTime();
        if (IsKeyDown(KEY_DOWN) && racket.y + racket.height < SCREEN_HEIGHT) racket.y += 400 * GetFrameTime();

        // Draw everything
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Draw the goal/wall
        DrawRectangle(SCREEN_WIDTH - 10, 0, 10, SCREEN_HEIGHT, BLACK);

        // Draw the racket
        DrawRectangle(racket.x, racket.y, racket.width, racket.height, BLACK);

        // Draw execution times for all paths
        DrawText(TextFormat("Execution Time of Straight Path: %.8f seconds", straightTime), 10, 10, 20, DARKGRAY);
        DrawText(TextFormat("Execution Time of Angular Path: %.8f seconds", angularTime), 10, 40, 20, DARKGRAY);
        DrawText(TextFormat("Execution Time of Convex Path: %.8f seconds", convexTime), 10, 70, 20, DARKGRAY);
        DrawText(TextFormat("Execution Time of Sinusoidal Path: %.8f seconds", sinusoidalTime), 10, 100, 20, DARKGRAY);

        // Draw the ball
        DrawStripedBall(ball);

        // Draw instructions
        DrawText("Press 1: Straight Path", 10, SCREEN_HEIGHT - 150, 20, DARKGRAY);
        DrawText("Press 2: Angular Path", 10, SCREEN_HEIGHT - 120, 20, DARKGRAY);
        DrawText("Press 3: Convex Path", 10, SCREEN_HEIGHT - 90, 20, DARKGRAY);
        DrawText("Press 4: Sinusoidal Path", 10, SCREEN_HEIGHT - 60, 20, DARKGRAY);
        DrawText("Press SPACE: Start/Stop", 10, SCREEN_HEIGHT - 30, 20, DARKGRAY);

        EndDrawing();
    }

    // Close Raylib window
    CloseWindow();
    return 0;
}
