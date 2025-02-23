#ifndef GAME_H
#define GAME_H

#include <stdbool.h>

// Define DLL export/import macros for Windows
#ifdef _WIN32
    #ifdef BUILD_DLL
        #define DLL_API __declspec(dllexport)  // Export functions when building DLL
    #else
        #define DLL_API __declspec(dllimport)  // Import functions when using DLL
    #endif
#else
    #define DLL_API  // No special handling for non-Windows platforms
#endif

// Function declarations exposed by game.dll
DLL_API void UpdateAndDraw(void);  // Draws GUI (no reload flag needed)
DLL_API void InitWindowDLL(int width, int height, const char* title);  // Initializes Raylib window
DLL_API bool WindowShouldCloseDLL(void);  // Checks if window should close
DLL_API void BeginDrawingDLL(void);  // Starts drawing frame
DLL_API void EndDrawingDLL(void);  // Ends drawing frame
DLL_API void CloseWindowDLL(void);  // Closes Raylib window
DLL_API void SetTargetFPSDLL(int fps);  // Sets target frames per second

#endif