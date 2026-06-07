#include "SandboxApp.h"

namespace Sandbox {

    SandboxApp::~SandboxApp()
    {
        if (m_mainSkin.leafTextureSpring) m_mainSkin.leafTextureSpring->Release();
        if (m_mainSkin.leafTextureSummer) m_mainSkin.leafTextureSummer->Release();
        if (m_mainSkin.leafTextureAutumn) m_mainSkin.leafTextureAutumn->Release();
        if (m_mainSkin.leafTextureWinter) m_mainSkin.leafTextureWinter->Release();
    }

    void SandboxApp::OnInit()
    {
    }

    void SandboxApp::OnUpdate(float deltaTime)
    {
        // Simple wind variation
        static float time = 0.0f;
        time += deltaTime;
        m_windSpeed = sin(time * 0.5f) * 0.5f + 0.5f; // Oscillates between 0 and 1

        // Keyboard inputs to switch seasons
        if (GetAsyncKeyState('1') & 0x8000) { m_mainTree.SetSeason(Season::SPRING); m_smallTree.SetSeason(Season::SPRING); }
        if (GetAsyncKeyState('2') & 0x8000) { m_mainTree.SetSeason(Season::SUMMER); m_smallTree.SetSeason(Season::SUMMER); }
        if (GetAsyncKeyState('3') & 0x8000) { m_mainTree.SetSeason(Season::AUTUMN); m_smallTree.SetSeason(Season::AUTUMN); }
        if (GetAsyncKeyState('4') & 0x8000) { m_mainTree.SetSeason(Season::WINTER); m_smallTree.SetSeason(Season::WINTER); }

        m_mainTree.Update(deltaTime, m_windSpeed);
        m_smallTree.Update(deltaTime, m_windSpeed * 1.5f); // Smaller tree affected more by wind
    }

    void SandboxApp::OnRender(Graphics::Renderer& renderer)
    {
        // Initialize textures and trees on first frame
        static bool initialized = false;
        if (!initialized) {
            // Load Textures
            ID2D1Bitmap* spring = renderer.LoadTexture(L"d:/VS 22 projects/WeatherLIverWallpaper/WeatherLIverWallpaper/assets/leaf_spring.png");
            ID2D1Bitmap* summer = renderer.LoadTexture(L"d:/VS 22 projects/WeatherLIverWallpaper/WeatherLIverWallpaper/assets/leaf_summer.png");
            ID2D1Bitmap* autumn = renderer.LoadTexture(L"d:/VS 22 projects/WeatherLIverWallpaper/WeatherLIverWallpaper/assets/leaf_autumn.png");
            ID2D1Bitmap* winter = renderer.LoadTexture(L"d:/VS 22 projects/WeatherLIverWallpaper/WeatherLIverWallpaper/assets/snow_flake.png");

            // Setup Main Tree Skin
            m_mainSkin.leafTextureSpring = spring;
            m_mainSkin.leafTextureSummer = summer;
            m_mainSkin.leafTextureAutumn = autumn;
            m_mainSkin.leafTextureWinter = winter;
            m_mainSkin.branchLengthMultiplier = 1.0f;
            m_mainSkin.branchThicknessMultiplier = 1.0f;
            
            // Setup Small Tree Skin (e.g., Birch tree style: taller/thinner branches, lighter trunk)
            m_smallSkin.leafTextureSpring = spring;
            m_smallSkin.leafTextureSummer = summer;
            m_smallSkin.leafTextureAutumn = autumn;
            m_smallSkin.leafTextureWinter = winter;
            m_smallSkin.trunkColorR = 0.4f;
            m_smallSkin.trunkColorG = 0.35f;
            m_smallSkin.trunkColorB = 0.35f;
            m_smallSkin.branchLengthMultiplier = 1.2f;
            m_smallSkin.branchThicknessMultiplier = 0.6f;

            m_mainTree.Initialize(m_mainSkin);
            m_smallTree.Initialize(m_smallSkin);

            initialized = true;
        }

        // Set transforms dynamically in case window resizes
        float width = renderer.GetWidth();
        float height = renderer.GetHeight();
        m_mainTree.SetTransform(width / 2.0f, height - 10.0f, 1.0f);
        m_smallTree.SetTransform(width * 0.75f, height - 10.0f, 0.6f);

        // Draw sky background based on season
        Season s = m_mainTree.GetSeason();
        if (s == Season::SPRING) renderer.Clear(0.8f, 0.9f, 1.0f); // Bright blue sky
        if (s == Season::SUMMER) renderer.Clear(0.5f, 0.8f, 1.0f); // Deep blue sky
        if (s == Season::AUTUMN) renderer.Clear(0.9f, 0.7f, 0.6f); // Sunset orange sky
        if (s == Season::WINTER) renderer.Clear(0.8f, 0.8f, 0.9f); // Greyish winter sky

        // Render the procedural trees
        m_mainTree.Render(renderer);
        m_smallTree.Render(renderer);

        // Draw ground
        if (s == Season::WINTER) {
            renderer.DrawLine(0.0f, height - 10.0f, width, height - 10.0f, 0.9f, 0.9f, 0.95f, 50.0f);
        } else {
            renderer.DrawLine(0.0f, height - 10.0f, width, height - 10.0f, 0.2f, 0.5f, 0.2f, 50.0f);
        }
    }

}
