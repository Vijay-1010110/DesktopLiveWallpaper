# Windows Desktop Injection & Z-Order Logic

This document explains the core logic used to inject our custom window (the live wallpaper) behind desktop icons on Windows 10 and Windows 11. It's a reusable mechanism that accounts for recent changes in Windows 11's architecture.

## Background

Normally, the Windows desktop is managed by the `Progman` (Program Manager) window, which contains `SHELLDLL_DefView` (the window that handles the desktop background), which in turn contains `SysListView32` (the list view that draws the actual desktop icons).

To render a live wallpaper *behind* the desktop icons but *in front* of the static desktop background, we utilize a hidden feature triggered by sending a specific message (`0x052C`) to `Progman`.

## The Traditional Approach (Windows 10 & early Windows 11)

When you send the undocumented message `0x052C` to `Progman` with `SendMessageTimeout`, Windows does the following:
1. It splits the desktop into a layered structure by spawning new `WorkerW` windows.
2. It moves `SHELLDLL_DefView` (along with the desktop icons) out of `Progman` and into one of the newly spawned `WorkerW` windows.
3. It spawns a completely empty, top-level `WorkerW` window that sits directly behind the `WorkerW` containing the desktop icons.

**Injection Logic:**
1. Send `0x052C` to `Progman`.
2. Enumerate all top-level windows to find the `WorkerW` that is an immediate sibling to the one containing `SHELLDLL_DefView`. This is the "empty" `WorkerW`.
3. Create the custom live wallpaper window and set its parent to this empty `WorkerW`.
4. Call `SetWindowPos(hWnd, HWND_BOTTOM, ...)` on the custom window to ensure it renders at the bottom of the empty `WorkerW` layer.

## The Windows 11 24H2+ "Raised Desktop" Approach

In recent Windows 11 updates (24H2+), Microsoft introduced a feature called "Raised Desktop" (also known as `LayeredShellView`). In this mode, sending `0x052C` no longer splits the desktop in a way that allows us to inject a window between the static wallpaper and the desktop icons cleanly.

If we attach to the empty `WorkerW`, it might end up completely behind `Progman` (thus hidden by the static wallpaper) or in front of the desktop icons.

**How to detect Raised Desktop:**
We check the Extended Window Styles (`GWL_EXSTYLE`) of `Progman`. If it contains the `WS_EX_NOREDIRECTIONBITMAP` flag, the system is using the new Raised Desktop architecture.

**Injection Logic (Raised Desktop):**
1. Send `0x052C` to `Progman`. This still spawns a `WorkerW` window.
2. Unlike the traditional method, we DO NOT set our parent to the empty `WorkerW`. Instead, we set our custom window's parent directly to `Progman` (which now has `SHELLDLL_DefView`).
3. We explicitly set our custom window's Z-order to be immediately behind `SHELLDLL_DefView` inside `Progman` using `SetWindowPos(hWnd, defView, ...)`.
4. Finally, we find the "rogue" empty `WorkerW` that Windows spawned and explicitly push it to the absolute bottom (`HWND_BOTTOM`) of the Z-order so it doesn't obscure anything.

## Critical Window Styles

Your custom window **must** have the following styles to avoid being aggressively terminated by the `explorer.exe` process (specifically on Windows 11):

- `WS_CHILD`: To behave as a child window attached to the desktop layer, rather than a top-level popup.
- `WS_EX_LAYERED`: Essential for performance and correctly drawing over the desktop without causing tearing or blocking input. You must call `SetLayeredWindowAttributes(hWnd, 0, 255, LWA_ALPHA)` to initialize it.
- **NEVER use `WS_POPUP`**: Windows 11 DWM will often crash or forcefully close the window if it tries to inject a `WS_POPUP` window into `WorkerW` or `Progman`.

## Complete Reference Code
Check `main.cpp` and `WallpaperInjector.cpp` for the full C++ implementation of this dual-fallback logic.
