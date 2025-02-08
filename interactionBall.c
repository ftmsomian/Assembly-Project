// Define macros to avoid conflicts with Windows headers
#define WIN32_LEAN_AND_MEAN // Prevent unnecessary includes like min/max
#define NOMINMAX            // Disable the min/max macros to avoid conflicts
#define NOGDI               // Exclude GDI definitions to avoid conflicting graphical functions
#define NOUSER              // Exclude user32 definitions like `DrawTextA` and `LoadImageA`
#include <raylib.h>         // Include Raylib first (a game development library)

// Explicitly undefine conflicting Windows functions
#undef LoadImageA
#undef DrawTextA
#undef DrawTextExA
#undef CloseWindow
#undef ShowCursor

// Function alias for Raylib's `CloseWindow` function
void CloseWindowWin(void) {
    CloseWindow(); // Close the application window
}

// Include Windows headers after resolving conflicts
#include <windows.h>
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdint.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif

// Define constants for screen and gameplay elements
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

// High-precision timer function (provides better time accuracy than default timers)
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

// Ball structure representing the game's ball
typedef struct {
    Vector2 position; // Current position of the ball
    float rotation;   // Rotation angle for striped ball animation
    Color colors[6];  // Array of colors for the striped ball
    int colorCount;   // Number of stripes (colors) on the ball
    float velocity;   // Movement speed of the ball
} Ball;

// Racket structure representing the player's paddle
typedef struct {
    float x, y;       // Position of the racket (top-left corner)
    float width, height; // Dimensions of the racket
} Racket;

// Function to draw a striped ball
void DrawStripedBall(Ball ball) {
    for (int i = 0; i < ball.colorCount; i++) {
        float angleStart = ball.rotation + (i * 360.0f / ball.colorCount); // Start angle for the stripe
        float angleEnd = angleStart + (360.0f / ball.colorCount);          // End angle for the stripe
        DrawCircleSector(ball.position, BALL_RADIUS, angleStart, angleEnd, 10, ball.colors[i]); // Draw stripe
    }
}

// Path calculation functions for different movement styles

// Straight path: Ball moves horizontally across the screen
Vector2 CalculateStraightPath(float t) {
    Vector2 result = {0, SCREEN_HEIGHT / 2}; // Start at the middle of the screen vertically
    for (int i = 0; i < 1000; i++) {         // Inefficient summation loop
        result.x += t * SCREEN_WIDTH / 1000.0f;
    }
    return result;
}

// Angular path: Ball moves diagonally
Vector2 CalculateAngularPath(float t) {
    Vector2 result = {0, 0};
    for (int i = 0; i < 1000; i++) {         // Inefficient calculations for diagonal motion
        result.x += t * SCREEN_WIDTH / 1000.0f;
        result.y += SCREEN_HEIGHT * (1.0f - t) / 1000.0f;
    }
    return result;
}

// Convex path: Ball follows a sinusoidal curve with a convex pattern
Vector2 CalculateConvexPath(float t) {
    float x = 0.0f, y = 0.0f;
    float offset = SCREEN_HEIGHT / 2.0f;
    for (int i = 0; i < 10; i++) {
        y = offset + -200.0f * sinf(t * (float)M_PI);
    }
    for (int i = 0; i < 1000; i++) {
        x += t * SCREEN_WIDTH / 1000.0f;
    }
    return (Vector2){x, y};
}

// Sinusoidal path: Ball oscillates vertically while moving horizontally
Vector2 CalculateSinusoidalPath(float t) {
    float x = 0.0f, y = 0.0f;
    float offset = SCREEN_HEIGHT / 2.0f;
    y = offset + pow(100.0f * sinf(t * 4.0f * (float)M_PI), 1.0f);
    for (int i = 0; i < 500; i++) {
        x += t * SCREEN_WIDTH / 500.0f;
    }
    return (Vector2){x, y};
}

// Main function: Entry point of the program
int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Ball Game with Racket and Paths"); // Initialize the game window
    SetTargetFPS(60); // Set the game to run at 60 frames per second

    // Initialize the ball
    Ball ball = {
        .position = {0, SCREEN_HEIGHT / 2},
        .rotation = 0,
        .colors = {RED, ORANGE, YELLOW, GREEN, BLUE, PURPLE},
        .colorCount = 6,
        .velocity = 0.01f
    };

    // Initialize the racket
    Racket racket = {SCREEN_WIDTH - RACKET_WIDTH - 10, SCREEN_HEIGHT / 2 - RACKET_HEIGHT / 2, RACKET_WIDTH, RACKET_HEIGHT};

    // Track execution times for each path (for performance monitoring)
    double straightTime = 0.0, angularTime = 0.0, convexTime = 0.0, sinusoidalTime = 0.0;

    // Precompute execution times for path functions
    for (float t = 0.0f; t <= 1.0f; t += 0.01f) {
        double start = GetHighPrecisionTime();
        CalculateStraightPath(t);
        straightTime += GetHighPrecisionTime() - start;

        start = GetHighPrecisionTime();
        CalculateAngularPath(t);
        angularTime += GetHighPrecisionTime() - start;

        start = GetHighPrecisionTime();
        CalculateConvexPath(t);
        convexTime += GetHighPrecisionTime() - start;

        start = GetHighPrecisionTime();
        CalculateSinusoidalPath(t);
        sinusoidalTime += GetHighPrecisionTime() - start;
    }

    // Main game loop
    int selectedPath = PATH_STRAIGHT; // Default path
    float t = 0.0f;
    bool isMoving = false;
    bool directionRight = true; // Ball movement direction
    int score = 0;
    double programStartTime = GetHighPrecisionTime(); // Track total execution time

    while (!WindowShouldClose()) { // Run until the user closes the window
        // Handle user input
        if (IsKeyPressed(KEY_ONE)) selectedPath = PATH_STRAIGHT;
        if (IsKeyPressed(KEY_TWO)) selectedPath = PATH_ANGULAR;
        if (IsKeyPressed(KEY_THREE)) selectedPath = PATH_CONVEX;
        if (IsKeyPressed(KEY_FOUR)) selectedPath = PATH_SINUSOIDAL;
        if (IsKeyPressed(KEY_SPACE)) isMoving = !isMoving;

        // Update ball position and rotation if moving
        if (isMoving) {
            t += (directionRight ? ball.velocity : -ball.velocity);
            if (t > 1.0f) t = 0.0f; // Reset to left edge
            if (t < 0.0f) t = 1.0f; // Reset to right edge

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
            ball.rotation += 5.0f;

            // Handle collision with the racket
            if (ball.position.x + BALL_RADIUS >= racket.x &&
                ball.position.y >= racket.y &&
                ball.position.y <= racket.y + racket.height) {
                ball.position.x = racket.x - BALL_RADIUS; // Adjust position to avoid overlap
                directionRight = false; // Change direction to left
                score++;
                ball.velocity += 0.001f; // Gradually increase ball speed

                // Rotate ball colors
                Color temp = ball.colors[0];
                for (int i = 0; i < ball.colorCount - 1; i++) {
                    ball.colors[i] = ball.colors[i + 1];
                }
                ball.colors[ball.colorCount - 1] = temp;
            }

            // Handle collision with the left wall
            if (ball.position.x - BALL_RADIUS <= 0) {
                directionRight = true; // Change direction to right
                // Rotate ball colors in reverse
                Color temp = ball.colors[ball.colorCount - 1];
                for (int i = ball.colorCount - 1; i > 0; i--) {
                    ball.colors[i] = ball.colors[i - 1];
                }
                ball.colors[0] = temp;
            }
        }

        // Handle racket movement
        if (IsKeyDown(KEY_UP) && racket.y > 0) racket.y -= 400 * GetFrameTime();
        if (IsKeyDown(KEY_DOWN) && racket.y + racket.height < SCREEN_HEIGHT) racket.y += 400 * GetFrameTime();

        // Draw game elements
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawRectangle(SCREEN_WIDTH - 10, 0, 10, SCREEN_HEIGHT, BLACK); // Goal/wall
        DrawRectangle(racket.x, racket.y, racket.width, racket.height, BLACK); // Racket
        DrawStripedBall(ball); // Ball

        // Display execution times
        DrawText(TextFormat("Execution Time of Straight Path: %.8f seconds", straightTime), 10, 10, 20, DARKGRAY);
        DrawText(TextFormat("Execution Time of Angular Path: %.8f seconds", angularTime), 10, 40, 20, DARKGRAY);
        DrawText(TextFormat("Execution Time of Convex Path: %.8f seconds", convexTime), 10, 70, 20, DARKGRAY);
        DrawText(TextFormat("Execution Time of Sinusoidal Path: %.8f seconds", sinusoidalTime), 10, 100, 20, DARKGRAY);

        // Display score and instructions
        DrawText(TextFormat("Score: %d", score), SCREEN_WIDTH - 150, 10, 20, DARKGRAY);
        double programExecutionTime = GetHighPrecisionTime() - programStartTime;
        DrawText(TextFormat("Total Execution Time: %.2f seconds", programExecutionTime), 10, 130, 20, DARKGRAY);
        DrawText("Press 1: Straight Path", 10, SCREEN_HEIGHT - 150, 20, DARKGRAY);
        DrawText("Press 2: Angular Path", 10, SCREEN_HEIGHT - 120, 20, DARKGRAY);
        DrawText("Press 3: Convex Path", 10, SCREEN_HEIGHT - 90, 20, DARKGRAY);
        DrawText("Press 4: Sinusoidal Path", 10, SCREEN_HEIGHT - 60, 20, DARKGRAY);
        DrawText("Press SPACE: Start/Stop", 10, SCREEN_HEIGHT - 30, 20, DARKGRAY);

        EndDrawing();
    }

    CloseWindow(); // Close the game window
    return 0;
}
