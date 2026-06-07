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

        GenerateClouds(20); // Generate initial batch of clouds
    }

    void Terrain::GenerateClouds(int count) {
        m_clouds.clear();
        for (int i = 0; i < count; ++i) {
            Cloud c;
            c.x = static_cast<float>(rand() % static_cast<int>(m_screenWidth * 1.5f)) - (m_screenWidth * 0.25f);
            c.y = static_cast<float>(rand() % static_cast<int>(m_screenHeight * 0.5f)); // Top half of screen
            c.width = 200.0f + static_cast<float>(rand() % 400);
            c.height = c.width * (0.3f + 0.2f * (rand() % 100) / 100.0f);
            c.speed = 5.0f + static_cast<float>(rand() % 15);
            c.opacity = 0.4f + (rand() % 40) / 100.0f;
            m_clouds.push_back(c);
        }
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

        // Update clouds
        for (auto& c : m_clouds) {
            c.x += c.speed * deltaTime;
            if (c.x > m_screenWidth + c.width) {
                // Wrap around
                c.x = -c.width;
                c.y = static_cast<float>(rand() % static_cast<int>(m_screenHeight * 0.5f));
            }
        }
    }

    void Terrain::RenderSky(Graphics::Renderer& renderer) {
        if (!m_skin.cloudTexture) return;

        // Draw each cloud
        for (const auto& c : m_clouds) {
            float opacity = c.opacity;
            
            // Adjust cloud appearance based on season
            if (m_currentSeason == Season::WINTER || m_currentSeason == Season::AUTUMN) {
                // More dense/overcast looking clouds
                opacity *= 1.5f; 
            } else if (m_currentSeason == Season::SPRING) {
                // Light wispy clouds
                opacity *= 0.7f;
            }

            if (opacity > 1.0f) opacity = 1.0f;

            // Draw the cloud texture (snow_flake stretched horizontally)
            // It acts as a soft white blob. We draw a few overlapping to make it fluffier.
            renderer.DrawTexture(m_skin.cloudTexture, c.x, c.y, c.width, c.height, opacity);
            renderer.DrawTexture(m_skin.cloudTexture, c.x + c.width*0.2f, c.y - c.height*0.2f, c.width*0.8f, c.height, opacity*0.8f);
            renderer.DrawTexture(m_skin.cloudTexture, c.x - c.width*0.1f, c.y + c.height*0.1f, c.width*0.6f, c.height*0.9f, opacity*0.9f);
        }
    }

    void Terrain::Render(Graphics::Renderer& renderer)
    {
        // 1. Draw Full-Screen Scenic Background
        ID2D1Bitmap* currentBgTex = nullptr;
        switch (m_currentSeason) {
            case Season::SPRING: currentBgTex = m_skin.grassSpring; break;
            case Season::SUMMER: currentBgTex = m_skin.grassSummer; break;
            case Season::AUTUMN: currentBgTex = m_skin.grassAutumn; break;
            case Season::WINTER: currentBgTex = m_skin.grassWinter; break;
        }

        if (currentBgTex) {
            // Stretch the scenic background to fill the entire screen
            renderer.DrawTexture(currentBgTex, 0.0f, 0.0f, m_screenWidth, m_screenHeight);
        }

        // 2. Draw Puddle with 3D Perspective Squash
        if (m_puddleFillLevel > 0.01f && m_skin.puddleTexture) {
            // Scale puddle based on fill level
            float currentWidth = m_puddleMaxWidth * m_puddleFillLevel;
            
            // Apply extreme squash to give 3D depth perspective (lying flat on the ground)
            float perspectiveSquash = 0.2f; 
            float currentHeight = (m_puddleMaxHeight * m_puddleFillLevel) * perspectiveSquash;
            
            float opacity = m_puddleFillLevel * 0.8f; // Max 80% opacity

            // Adjust puddle position to match the new tree depth (height - 350)
            float targetPuddleY = m_screenHeight - 330.0f; 
            float px = m_puddleX - (currentWidth / 2.0f);
            float py = targetPuddleY - (currentHeight / 2.0f);

            renderer.DrawTexture(m_skin.puddleTexture, px, py, currentWidth, currentHeight, opacity);
        }
    }

}
