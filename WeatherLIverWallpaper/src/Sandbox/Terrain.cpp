#include "Terrain.h"
#include "AnimeTree.h" // For Season enum
#include <algorithm>

namespace Sandbox {

    Terrain::Terrain()
        : m_currentSeason(Season::SPRING),
          m_currentWeather(Weather::SUNNY),
          m_screenWidth(1920.0f),
          m_screenHeight(1080.0f),
          m_terrainHeight(150.0f),
          m_puddleFillLevel(0.0f)
    {
    }

    void Terrain::Init(float screenWidth, float screenHeight)
    {
        m_screenWidth = screenWidth;
        m_screenHeight = screenHeight;

        m_terrainHeight = 150.0f; // Grass strip at bottom

        // Position puddle near the center bottom
        m_puddleX = m_screenWidth / 2.0f;
        m_puddleY = m_screenHeight - (m_terrainHeight / 2.0f);
        m_puddleMaxWidth = 400.0f;
        m_puddleMaxHeight = 80.0f;
    }

    void Terrain::SetSkin(const TerrainSkin& skin)
    {
        m_skin = skin;
    }

    void Terrain::SetSeason(Season season)
    {
        m_currentSeason = season;
    }

    void Terrain::SetWeather(Weather weather)
    {
        m_currentWeather = weather;
    }

    void Terrain::Update(float deltaTime)
    {
        // Puddle drying/filling logic
        float fillRate = 0.5f; // Reaches full in 2 seconds
        float dryRate = 0.2f;  // Dries out in 5 seconds

        if (m_currentWeather == Weather::RAINING) {
            m_puddleFillLevel += fillRate * deltaTime;
            if (m_puddleFillLevel > 1.0f) m_puddleFillLevel = 1.0f;
        } else {
            // Sunny weather dries it out
            m_puddleFillLevel -= dryRate * deltaTime;
            if (m_puddleFillLevel < 0.0f) m_puddleFillLevel = 0.0f;
        }

        // Summer heat dries it out even faster
        if (m_currentSeason == Season::SUMMER && m_currentWeather != Weather::RAINING) {
            m_puddleFillLevel -= (dryRate * 2.0f) * deltaTime;
            if (m_puddleFillLevel < 0.0f) m_puddleFillLevel = 0.0f;
        }
    }

    void Terrain::Render(Graphics::Renderer& renderer)
    {
        // 1. Draw Grass
        ID2D1Bitmap* currentGrassTex = nullptr;
        switch (m_currentSeason) {
            case Season::SPRING: currentGrassTex = m_skin.grassSpring; break;
            case Season::SUMMER: currentGrassTex = m_skin.grassSummer; break;
            case Season::AUTUMN: currentGrassTex = m_skin.grassAutumn; break;
            case Season::WINTER: currentGrassTex = m_skin.grassWinter; break;
        }

        if (currentGrassTex) {
            // Tile the grass across the bottom
            // Since we are doing a simple 2D renderer, we can draw a few stretched rectangles or tiles
            // We'll just stretch one big texture across the bottom for now
            float grassY = m_screenHeight - m_terrainHeight;
            renderer.DrawTexture(currentGrassTex, 0.0f, grassY, m_screenWidth, m_terrainHeight);
        }

        // 2. Draw Puddle
        if (m_puddleFillLevel > 0.01f && m_skin.puddleTexture) {
            // Scale and fade puddle based on fill level
            float currentWidth = m_puddleMaxWidth * m_puddleFillLevel;
            float currentHeight = m_puddleMaxHeight * m_puddleFillLevel;
            float opacity = m_puddleFillLevel * 0.8f; // Max 80% opacity

            float px = m_puddleX - (currentWidth / 2.0f);
            float py = m_puddleY - (currentHeight / 2.0f);

            renderer.DrawTexture(m_skin.puddleTexture, px, py, currentWidth, currentHeight, opacity);
        }
    }

}
