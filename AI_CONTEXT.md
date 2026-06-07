# WeatherLiveWallpaper

## Goal
A Windows Desktop Application using C++ and Direct2D that functions as a dynamic live wallpaper.
Target frame rate is ~30 FPS for optimization.

## Features (Planned)
- Draw behind desktop icons using the `Progman` -> `WorkerW` injection method.
- **Sun/Moon/Stars**: Mathematically calculate positions based on location and time (no external API calls). Moon phases pre-loaded/stored. Night sky stars mapped mathematically and cached based on location.
- **Clouds**: Move on time intervals (not every frame) for optimization. Density and color depend on live weather data.
- **Weather Effects**: Pre-baked animations for lightning and rain based on live weather data.
- **Wind**: Simulated with cartoon-style wind lines and flowing leaves, based on live wind speed data.

## Current State
Phase 2: Developed an interactive procedural environment. 
- Implemented `AnimeTree` for procedurally generating branching trees that sway with simulated wind and drop seasonal particles (leaves, snow, petals).
- Implemented a `Terrain` system that uses generated and cropped Studio Ghibli style textures on the bottom half of the screen.
- Implemented a dynamic weather and puddle system that fills with rain.
- Implemented a procedural Cloud and Sky system where the sky changes color dynamically based on the season and clouds drift horizontally across the screen with varying seasonal densities.
- Implemented a system where users can dynamically change the season using the 1, 2, 3, 4 number keys, and toggle rain using the 5 key.
