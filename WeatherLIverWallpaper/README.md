# Desktop Live Wallpaper

A robust Windows C++ application that seamlessly injects live, animated wallpapers directly behind your desktop icons. Built with a modern Sandbox architecture, it allows developers to easily create and swap custom rendering logic without touching the underlying Windows injection engine.

## Features

- **True Desktop Integration**: Renders perfectly behind desktop icons while allowing full interactivity (e.g., desktop context menus and selection boxes pass through perfectly).
- **Windows 11 24H2+ Support**: Includes custom dual-fallback injection logic. It supports the classic `WorkerW` splits (Windows 10 / early Win 11) as well as the new `LayeredShellView` "Raised Desktop" mode introduced in recent Windows 11 updates.
- **Sandbox Architecture**: The application logic is completely decoupled from the engine. 
  - **Engine (`Core::Application`)**: Handles the complex `wWinMain` loop, DWM edge cases, Z-order injection, and `HWND` lifecycles.
  - **Sandbox (`Sandbox::SandboxApp`)**: The customizable playground where you write your drawing logic.
- **Direct2D Rendering**: Hardware-accelerated 2D graphics out of the box (with plans to expand to advanced rendering APIs).

## Architecture Overview

The repository is structured to separate concerns:
- `src/Core/`: Contains the engine (`Application.cpp`, `WallpaperInjector.cpp`).
- `src/Sandbox/`: Contains the user implementation (`SandboxApp.cpp`).
- `src/Graphics/`: Contains API wrappers (`Renderer.cpp`).
- `docs/`: Technical documentation regarding the undocumented Windows desktop injection APIs.

## Building

This project is built using Visual Studio 2022 and C++20.
1. Open the `.sln` or `.vcxproj` in Visual Studio.
2. Ensure you have the Windows SDK installed.
3. Build for `x64` in `Debug` or `Release`.

## Getting Started

To create your own wallpaper, modify `src/Sandbox/SandboxApp.cpp`. The Sandbox exposes easy-to-use lifecycle methods like `OnInit()`, `OnUpdate(float deltaTime)`, and `OnRender(Graphics::Renderer& renderer)`.

## License
MIT License
