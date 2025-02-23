
# grok raylib raygui auto reload c

# LICENSE: MIT

# required:
 * CMake
 * toolchain / compiler (vs2022 tested windows 10 64bit)

# features:
 * RayLib 5.5
 * Hot Reload
 * RayGUI
 * C language

# Innformation:
  With the help of Grok Beta 3 limited time. Able to learn how to proper set up cmake build to run raylib correct setup.

  With the AI. I work on trying to see to make sure hot reload work correctly. There are different way to reload depend on how code structure correctly.

  Added notes in the files from the Grok AI.

  Please note this use c language.

  This auto reload once detect change for game.dll time stamp.

# Build:
  command line on the current project folder dir.
```
mkdir build
cd build
```

```
cmake ..
```

```
cmake --build . --config Debug
```

```
build/Debug/RaylibExample.exe
```
 * Note it will create game.dll copy to store application for next compile game.dll in case of conflict.

## rebuild game.dll:
```
cmake --build . --target game --config Debug
```

# Grok 3:

## How It Works:
 * CheckDLLUpdate monitors game.dll every 500ms, triggering a close-reload-reopen cycle on change.
 * Temp files ensure game.dll is free for rebuilds, tracked in tempDllPaths, and deleted by CleanupTempDLLs
 * RayGUI button toggles color, reflecting updates post-reload.

### Overview:
 * Purpose: Automatically hot reloads game.dll (Raylib 5.5 + RayGUI 4.0) when rebuilt, cleaning up temp DLL files on exit to avoid clutter.
 * Key Mechanism: Monitors game.dll’s timestamp every 500ms, closes and reopens the window with a new temp-loaded DLL on change, and deletes temp files on program termination.

### File Roles:
 * CMakeLists.txt: Configures the build, fetching Raylib 5.5 and RayGUI 4.0, producing RaylibExample.exe and game.dll.
 * game.h: Defines the DLL interface for Raylib and RayGUI functions.
 * game.c: Implements the DLL, managing the window and drawing a toggle button with RayGUI.
 * main.c: Loads game.dll, runs the window loop, auto-reloads on timestamp changes, and cleans up temp files.

### Auto-Reload Process:
 * Initial Load: Copies game.dll to a temp file (e.g., game_temp_12345.dll), loads it into dllHandle, stores the path in tempDllPaths, and initializes the window.
 * Main Loop: Draws the GUI via UpdateAndDrawPtr every frame.
 * Timestamp Check: Every 500ms, CheckDLLUpdate compares game.dll’s timestamp to lastWriteTime.
 * Reload Trigger: On change, closes the window, reloads game.dll into a new temp file (stored in tempDllPaths), reinitializes the window, and resumes drawing.
 * Cleanup: On exit, CleanupTempDLLs deletes all temp files tracked in tempDllPaths.
### Key Features:
 * Automatic Reload: Updates game.dll when rebuilt, triggered by timestamp changes every 500ms.
 * Temp File Management: Uses temp copies to keep game.dll free, deletes them on exit to prevent accumulation.
 * RayGUI: Button toggles background color, reloadable with new text (e.g., "Switch Color").
 * Stability: Closing/reopening avoids mid-run DLL state issues, with Sleep(200) stabilizing reloads.

### Cleanup Notes:
 * Storage: tempDllPaths holds up to 10 temp file paths (arbitrary limit—adjustable if more reloads expected).
 * Deletion: DeleteFile removes each temp DLL on exit, logging success or failure.

# Refs:
 * https://github.com/raysan5/raylib/issues/1217
