# ClipCursor Disable
A DLL that disables mouse confinement in Windows applications by overriding the `ClipCursor` API

This project provides a clean, injection-based way to prevent any Windows game or application from locking the mouse cursor to a window.
By hooking the Win32 `ClipCursor` function and turning it into a no-op, the target program loses all ability to confine or unconfine the cursor.

## Overview

Some software forcibly traps or untraps the mouse inside a window using the `ClipCursor` API.
This DLL intercepts that API call and replaces it with a stub that:

- Does nothing
- Always returns `TRUE`
- Leaves mouse confinement unchanged

As a result:

- Applications cannot lock the mouse
- Applications cannot unlock the mouse
- Mouse confinement cannot be changed by the application

This works on any Windows application: 32-bit or 64-bit.

## Features

- Overrides the Win32 `ClipCursor` function
- Prevents applications from changing mouse confinement
- Works for both x86 and x64 programs
- Simple drag-and-drop / CLI batch installer
- Fully reversible using the same script
- Uses Microsoft Detours (included as a submodule)
- No application code patching; only the import table is updated to load the DLL

## How It Works

The DLL uses Microsoft Detours to hook the process's import of:

```c
BOOL WINAPI ClipCursor(const RECT *);
```

The hooked replacement immediately returns `TRUE` and performs no cursor restriction.

Every call to `ClipCursor` becomes a no-op:

- `ClipCursor(&rect);` → ignored
- `ClipCursor(NULL);` → ignored

The program cannot alter the cursor's freedom in any way.

## Prerequisites

- Windows operating system
- Target program (any EXE)

## Usage

1. Place all files from this repository into the same directory as the target executable.

2. Either:
   - Drag the target executable onto `ClipCursorDisable.bat`, or
   - Run it from a command prompt, passing the EXE as an argument:
     ```bat
     ClipCursorDisable.bat "path\to\program.exe"
     ```

3. The script will:
   - Detect whether the EXE is 32-bit or 64-bit
   - Select the appropriate DLL (`ClipCursorDisable_x86.dll` or `ClipCursorDisable_x64.dll`)
   - Display:
     ```text
     1. Add ClipCursorDisable library to "<path-to-executable>"
     2. Remove ClipCursorDisable library from "<path-to-executable>"
     ```

4. Choose an option and press Enter.

The script uses `setdll` to add or remove the DLL from the executable's import table.

## Files Included In Build

- `ClipCursorDisable.bat`: Batch installer/remover
- `ClipCursorDisable_x86.dll`: 32-bit ClipCursor override
- `ClipCursorDisable_x64.dll`: 64-bit ClipCursor override
- `setdll_x86.exe` / `setdll_x64.exe`: DLL import patch tools
- `LICENSE.ClipCursorDisable` / `LICENSE.Detours`: License files
- `README.ClipCursorDisable.md`: Readme

## Technical Notes

- Uses Microsoft Detours for API hooking
- Does not modify the application's code; only the import table is updated to load the DLL
- Hook is lightweight and has no noticeable runtime overhead
- The target application still believes `ClipCursor` succeeded

## License

This project includes Microsoft Detours under its license.
All custom code in this repository is MIT-licensed unless otherwise stated.
