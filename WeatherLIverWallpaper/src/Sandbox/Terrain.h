#pragma once

#include <d2d1.h>
#include "../Graphics/Renderer.h"
#include <vector>

namespace Sandbox {

    enum class Weather {
        SUNNY,
        RAINING
    };

    // Forward declaration of Season (defined in SandboxApp.h or we can define it here if needed)
    // Actually, Season is currently defined in SandboxApp.h, but AnimeTree uses it. 
    // It's defined in AnimeTree.h right now. Let's include AnimeTree.h or just forward declare.
    // Wait, let's just #include "AnimeTree.h" to get the Season enum for now to avoid circular dependencies if we restructure.
    enum class Season; // Forward declaration

    struct TerrainSkin {
        ID2D1Bitmap* grassSpring = nullptr;
        ID2D1Bitmap* grassSummer = nullptr;
        ID2D1Bitmap* grassAutumn = nullptr;
        ID2D1Bitmap* grassWinter = nullptr;

        ID2D1Bitmap* puddleTexture = nullptr;
    };

    class Terrain {
    public:
        Terrain();
        ~Terrain() = default;

        void Init(float screenWidth, float screenHeight);
        void SetSkin(const TerrainSkin& skin);
        void SetSeason(Season season);
        void SetWeather(Weather weather);

        void Update(float deltaTime);
        void Render(Graphics::Renderer& renderer);

    private:
        TerrainSkin m_skin;
        Season m_currentSeason;
        Weather m_currentWeather;

        float m_screenWidth;
        float m_screenHeight;

        // Terrain logic
        float m_terrainHeight; // How tall the grass strip is

        // Puddle logic
        float m_puddleFillLevel; // 0.0f (dry) to 1.0f (full)
        float m_puddleX;
        float m_puddleY;
        float m_puddleMaxWidth;
        float m_puddleMaxHeight;
    };

}
