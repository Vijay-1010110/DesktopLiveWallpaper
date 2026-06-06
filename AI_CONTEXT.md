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
Phase 1: Initializing boilerplate. Setting up the WorkerW injection and Direct2D to render a basic animated scene at 30 FPS to prove the wallpaper concept works.
