#define BUILD_DLL  // Indicates this file builds the DLL
#include "game.h"

#define WIN32_LEAN_AND_MEAN  // Reduces windows.h include size
#define NOGDI  // Excludes GDI functions from windows.h
#define NOUSER  // Excludes USER functions from windows.h
#include <windows.h>
#include <stdbool.h>

#include "raylib.h"  // Raylib 5.5 header for core functionality
#define RAYGUI_IMPLEMENTATION  // Defines RayGUI implementation (must be before include)
#include "raygui.h"  // RayGUI 4.0 header for GUI elements

static bool isRed = false;  // State for toggling background color

// Initializes the Raylib window with given dimensions and title
void InitWindowDLL(int width, int height, const char* title) {
    InitWindow(width, height, title);
}

// Returns true if the window should close (e.g., close button clicked)
bool WindowShouldCloseDLL(void) {
    return WindowShouldClose();
}

// Begins a new drawing frame
void BeginDrawingDLL(void) {
    BeginDrawing();
}

// Ends the current drawing frame
void EndDrawingDLL(void) {
    EndDrawing();
}

// Closes the Raylib window
void CloseWindowDLL(void) {
    CloseWindow();
}

// Sets the target FPS for the window
void SetTargetFPSDLL(int fps) {
    SetTargetFPS(fps);
}

// Draws the GUI with a toggle button and reload instruction
void UpdateAndDraw() {
    ClearBackground(isRed ? RED : RAYWHITE);  // Toggle background color based on isRed
    if (GuiButton((Rectangle){ 300, 200, 200, 40 }, "Toggle Color Q")) {  // Button to toggle color
        isRed = !isRed;
    }
    DrawText("Edit me and reload! (Auto-reloads on change)", 150, 150, 20, DARKGRAY);  // Instruction text
    //DrawText("Edit me and reload! (Auto-reloads on change)", 150, 170, 20, DARKGRAY);  // Instruction text
}