#define WIN32_LEAN_AND_MEAN  // Reduces windows.h include size
#define NOGDI  // Excludes GDI functions
#define NOUSER  // Excludes USER functions
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>  // For malloc, realloc, free
#include <stdbool.h>
#include "game.h"  // Header for game.dll functions

// Function pointer typedefs for dynamic loading from game.dll
typedef void (*InitWindowFunc)(int, int, const char*);
typedef bool (*WindowShouldCloseFunc)(void);
typedef void (*BeginDrawingFunc)(void);
typedef void (*EndDrawingFunc)(void);
typedef void (*CloseWindowFunc)(void);
typedef void (*SetTargetFPSFunc)(int);
typedef void (*UpdateAndDrawFunc)(int);  // Updated to take reload count

static InitWindowFunc InitWindowPtr = NULL;
static WindowShouldCloseFunc WindowShouldClosePtr = NULL;
static BeginDrawingFunc BeginDrawingPtr = NULL;
static EndDrawingFunc EndDrawingPtr = NULL;
static CloseWindowFunc CloseWindowPtr = NULL;
static SetTargetFPSFunc SetTargetFPSPtr = NULL;
static UpdateAndDrawFunc UpdateAndDrawPtr = NULL;

static HMODULE dllHandle = NULL;  // Handle to the loaded game.dll instance
static FILETIME lastWriteTime = {0};  // Tracks last modification time of game.dll
static char (*tempDllPaths)[MAX_PATH] = NULL;  // Dynamic array for temp DLL paths
static int tempDllCount = 0;  // Number of temp DLLs created
static int tempDllCapacity = 0;  // Current capacity of tempDllPaths array

// Loads or reloads game.dll from a temp file, keeping original free for rebuild
bool LoadDLL(const char* dllPath) {
    if (dllHandle) {
        FreeLibrary(dllHandle);  // Unload previous DLL instance if it exists
    }

    char tempPath[MAX_PATH];
    // Create a unique temp filename to avoid locking game.dll
    snprintf(tempPath, MAX_PATH, "game_temp_%lu.dll", GetTickCount());
    CopyFile(dllPath, tempPath, FALSE);  // Copy game.dll to temp file
    dllHandle = LoadLibrary(tempPath);   // Load the temp copy
    if (!dllHandle) {
        printf("Failed to load DLL: %lu\n", GetLastError());
        return false;
    }

    // Assign function pointers from the loaded DLL
    InitWindowPtr = (InitWindowFunc)GetProcAddress(dllHandle, "InitWindowDLL");
    WindowShouldClosePtr = (WindowShouldCloseFunc)GetProcAddress(dllHandle, "WindowShouldCloseDLL");
    BeginDrawingPtr = (BeginDrawingFunc)GetProcAddress(dllHandle, "BeginDrawingDLL");
    EndDrawingPtr = (EndDrawingFunc)GetProcAddress(dllHandle, "EndDrawingDLL");
    CloseWindowPtr = (CloseWindowFunc)GetProcAddress(dllHandle, "CloseWindowDLL");
    SetTargetFPSPtr = (SetTargetFPSFunc)GetProcAddress(dllHandle, "SetTargetFPSDLL");
    UpdateAndDrawPtr = (UpdateAndDrawFunc)GetProcAddress(dllHandle, "UpdateAndDraw");

    // Check if all functions loaded successfully
    if (!InitWindowPtr || !WindowShouldClosePtr || !BeginDrawingPtr || 
        !EndDrawingPtr || !CloseWindowPtr || !SetTargetFPSPtr || !UpdateAndDrawPtr) {
        printf("Failed to load DLL functions: %lu\n", GetLastError());
        FreeLibrary(dllHandle);
        dllHandle = NULL;
        return false;
    }

    // Dynamically grow tempDllPaths array as needed
    if (tempDllCount >= tempDllCapacity) {
        tempDllCapacity = (tempDllCapacity == 0) ? 2 : tempDllCapacity * 2;  // Start with 2, then double
        char (*newPaths)[MAX_PATH] = realloc(tempDllPaths, tempDllCapacity * sizeof(*tempDllPaths));
        if (!newPaths) {
            printf("Failed to allocate memory for temp DLL paths\n");
            FreeLibrary(dllHandle);
            dllHandle = NULL;
            return false;
        }
        tempDllPaths = newPaths;
    }

    // Store temp DLL path for cleanup
    strncpy(tempDllPaths[tempDllCount], tempPath, MAX_PATH);
    tempDllCount++;

    printf("DLL loaded successfully!\n");
    return true;
}

// Checks if game.dll’s timestamp has changed, indicating a rebuild
bool CheckDLLUpdate(const char* dllPath) {
    HANDLE file = CreateFile(dllPath, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, 
                            OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) {
        printf("CheckDLLUpdate: Failed to open DLL file\n");
        return false;
    }

    FILETIME currentWriteTime;
    if (!GetFileTime(file, NULL, NULL, &currentWriteTime)) {
        printf("CheckDLLUpdate: GetFileTime failed with error %lu\n", GetLastError());
        CloseHandle(file);
        return false;
    }
    CloseHandle(file);

    if (CompareFileTime(&lastWriteTime, &currentWriteTime) != 0) {
        lastWriteTime = currentWriteTime;  // Update last known timestamp
        printf("CheckDLLUpdate: DLL timestamp changed\n");
        return true;
    }
    return false;
}

// Cleans up temporary DLL files created during load/reload
void CleanupTempDLLs() {
    for (int i = 0; i < tempDllCount; i++) {
        if (DeleteFile(tempDllPaths[i])) {
            printf("Deleted temp DLL: %s\n", tempDllPaths[i]);
        } else {
            printf("Failed to delete temp DLL: %s, error %lu\n", tempDllPaths[i], GetLastError());
        }
    }
    free(tempDllPaths);  // Free the dynamic array
    tempDllPaths = NULL;
    tempDllCount = 0;
    tempDllCapacity = 0;  // Reset counters
}

int main(void) {
    const char* dllPath = "game.dll";  // Path to the original game.dll

    // Initialize lastWriteTime to avoid false reloads on startup
    HANDLE file = CreateFile(dllPath, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, 
                            OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file != INVALID_HANDLE_VALUE) {
        GetFileTime(file, NULL, NULL, &lastWriteTime);
        CloseHandle(file);
    }

    // Load the DLL initially
    if (!LoadDLL(dllPath)) {
        printf("Initial DLL load failed\n");
        return 1;
    }

    // Initialize the Raylib window via the DLL
    InitWindowPtr(800, 450, "Raylib Hot Reload Test");
    SetTargetFPSPtr(60);

    printf("Window initialized, entering main loop...\n");
    int frameCount = 0;
    int reloadCount = 0;  // Tracks number of reloads
    DWORD lastCheckTime = GetTickCount();
    const DWORD checkInterval = 500;  // Check every 500ms to avoid overloading

    // Main loop: runs until window closes
    while (!WindowShouldClosePtr()) {
        frameCount++;
        printf("Frame %d: Window still open\n", frameCount);

        BeginDrawingPtr();          // Start drawing frame
        UpdateAndDrawPtr(reloadCount);  // Draw GUI with reload count
        EndDrawingPtr();            // End drawing frame

        // Check for DLL changes every 500ms
        DWORD currentTime = GetTickCount();
        if (currentTime - lastCheckTime >= checkInterval) {
            if (CheckDLLUpdate(dllPath)) {
                printf("Reload requested...\n");
                CloseWindowPtr();  // Close current window
                if (!LoadDLL(dllPath)) {
                    printf("Reload failed, exiting\n");
                    break;
                }
                InitWindowPtr(800, 450, "Raylib Hot Reload Test");  // Reopen window
                SetTargetFPSPtr(60);
                reloadCount++;  // Increment reload counter
                printf("DLL reloaded and window re-initialized\n");
                Sleep(200);  // Brief delay to stabilize reload
            }
            lastCheckTime = currentTime;
        }
    }

    printf("Window closing...\n");
    if (dllHandle) {
        CloseWindowPtr();    // Final window close
        FreeLibrary(dllHandle);  // Free the DLL handle
    }
    CleanupTempDLLs();  // Delete all temporary DLL files
    printf("Program exiting\n");
    return 0;
}