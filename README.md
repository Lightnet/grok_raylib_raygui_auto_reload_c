
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

  Please note this use c language.

  Added notes in the files from the Grok AI.

  This has simple counter if game.dll has change to update counter.

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

## Run and Reload:
 * Edit game.c multiple times (e.g., "Toggle Color" to "Switch Color", then "Change Color").

# Grok 3:

## How It Works:
 * CheckDLLUpdate monitors game.dll every 500ms, triggering a close-reload-reopen cycle on change.
 * Temp files ensure game.dll is free for rebuilds, tracked in tempDllPaths, and deleted by CleanupTempDLLs
 * RayGUI button toggles color, reflecting updates post-reload.

### Overview:
 * Purpose: Automatically hot reloads game.dll (Raylib 5.5 + RayGUI 4.0) when rebuilt, displays a reload count in the window, and cleans up unlimited temp DLL files on exit using a dynamic array.
 * Key Mechanism: Monitors game.dll’s timestamp every 500ms, closes and reopens the window with a new temp-loaded DLL on change, increments a reload counter, and deletes all temp files on exit.

### File Roles:
 * CMakeLists.txt: Configures the build, fetching Raylib 5.5 and RayGUI 4.0, producing RaylibExample.exe and game.dll.
 * game.h: Defines the DLL interface, updated UpdateAndDraw to take reloadCount.
 * game.c: Implements the DLL, drawing a toggle button and reload count with RayGUI.
 * main.c: Loads game.dll, runs the window loop, auto-reloads with a reload counter, and cleans up temp files.

### Auto-Reload Process:
 * Initial Load: Copies game.dll to a temp file (e.g., game_temp_12345.dll), loads it into dllHandle, stores the path in tempDllPaths, and initializes the window.
 * Main Loop: Draws the GUI via UpdateAndDrawPtr every frame, passing reloadCount.
 * Timestamp Check: Every 500ms, CheckDLLUpdate compares game.dll’s timestamp to lastWriteTime.
 * Reload Trigger: On change, closes the window, reloads game.dll into a new temp file (added to tempDllPaths), increments reloadCount, reinitializes the window, and resumes drawing.
 * Cleanup: On exit, CleanupTempDLLs deletes all temp files tracked in tempDllPaths and frees the dynamic array.
### Key Features:
 * Automatic Reload: Updates game.dll when rebuilt, triggered by timestamp changes every 500ms.
 * Reload Counter: reloadCount tracks reloads, displayed in the window via UpdateAndDraw.
 * Dynamic Temp Files: tempDllPaths grows dynamically (doubling from 2), cleaned up on exit.
 * RayGUI: Button toggles color, reloads with new text (e.g., "Switch Color").
 * Stability: Closing/reopening with Sleep(200) keeps Raylib’s state intact.

### Reload Counter Notes:
 * Tracking: reloadCount increments each reload, starting at 0 (initial load), passed to UpdateAndDraw.
 * Display: Rendered as "Reloads: X" in the top-left corner, updated live.

# Refs:
 * https://github.com/raysan5/raylib/issues/1217
