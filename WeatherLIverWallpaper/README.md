# Desktop Live Wallpaper - Procedural Anime Tree Engine

This project is a Windows desktop live wallpaper engine that uses Direct2D to render procedural, interactive scenes directly behind your desktop icons. It features a complete Sandbox architecture that separates the core rendering engine from the visual scene logic.

## Architecture: Sandbox Approach

The project is split into two primary layers to make it highly modifiable and reusable:

1. **The Core Engine (`src/Core`, `src/Graphics`)**:
   - Manages the Win32 windowing, desktop wallpaper injection (using `WorkerW`), and the Direct2D/DirectWrite rendering context.
   - Handles the application lifecycle and the system tray icon (`TrayIcon.cpp`).
   - Remains completely agnostic to *what* is being drawn on the screen.

2. **The Sandbox (`src/Sandbox`)**:
   - Contains all the visual logic, user interface, and scene generation.
   - `SandboxApp`: The main controller for the scene. It holds the background logic, parses user input (e.g., number keys to change seasons), and instantiates objects.
   - `AnimeTree`: A reusable, object-oriented procedural tree generator. 

By keeping the background and UI logic in the Sandbox, you can easily rip out `SandboxApp.cpp` and `AnimeTree.cpp` and replace them with completely different logic (like a physics simulation or a clock) without ever needing to touch the complicated Windows injection code in the Core.

## Core Working Logic: Procedural Anime Tree

The `AnimeTree` class is a robust procedural generation system that uses recursive mathematics to grow a tree.

### 1. The `TreeSkin` System
The tree is highly customizable via the `TreeSkin` struct. This allows the tree's appearance to be entirely swapped out dynamically. A skin defines:
- **Trunk colors**: A gradient from the base (`trunkColorStart`) to the tips (`trunkColorEnd`).
- **Leaf Textures**: The actual `ID2D1Bitmap` images to use for the leaves and falling particles.

### 2. Recursive Generation (`GenerateTree`)
To prevent the tree from jittering or changing every frame, the entire structure is generated *once* and stored in memory.
- The tree starts with a root branch and recursively spawns child branches.
- At each step, the `angle` is modified by a random offset, and the `length` and `thickness` are reduced.
- The recursive function pushes `Branch` structs into a hierarchical tree of `std::vector<Branch> children`.

### 3. Static Leaf Placement
When the tree generation reaches the end of a branch (or a maximum depth), it spawns a cluster of leaves.
- The math calculates the exact coordinates of the branch tip using Sine and Cosine.
- **Orientation & Pivots**: Because different leaf images have stems in different locations and natural pointing angles, the engine defines a custom `pivotX`, `pivotY`, and `textureOffset` for each image asset (Spring, Summer, Autumn). 
- This guarantees that the stem of the image perfectly overlaps the branch joint, and the leaf points outwards, regardless of the raw asset's layout.
- The leaf data (offsets, base rotations) is saved permanently in the `Branch` struct so it never jitters.

### 4. Interactive Rendering (`DrawBranch`)
Every frame, the tree is drawn using a **Two-Pass Rendering** technique:
- **Pass 1 (Wood)**: The engine recursively draws all the branches using Direct2D lines. The line thickness tapers based on the branch depth. A swaying motion is added by applying a `sin()` wave tied to the total elapsed time, making the branches gently blow in the wind.
- **Pass 2 (Leaves)**: The engine traverses the tree a second time to draw all the leaves. By drawing *all* leaves after *all* branches, we guarantee the canopy properly overlaps the wood, giving the tree realistic volume instead of floating behind the branches.

### 5. Falling Particles
Each tree manages its own independent particle system (`m_particles`). Every frame, it spawns new leaves that drift downwards. The particles use the same `TreeSkin` textures, and their movement includes horizontal swaying to simulate wind.
