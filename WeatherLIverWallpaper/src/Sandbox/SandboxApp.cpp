#include "SandboxApp.h"

namespace Sandbox {

    SandboxApp::~SandboxApp()
    {
        if (m_mainSkin.leafTextureSpring) m_mainSkin.leafTextureSpring->Release();
        if (m_mainSkin.leafTextureSummer) m_mainSkin.leafTextureSummer->Release();
        if (m_mainSkin.leafTextureAutumn) m_mainSkin.leafTextureAutumn->Release();
        if (m_mainSkin.leafTextureWinter) m_mainSkin.leafTextureWinter->Release();

        if (m_terrainSkin.grassSpring) m_terrainSkin.grassSpring->Release();
        if (m_terrainSkin.grassSummer) m_terrainSkin.grassSummer->Release();
        if (m_terrainSkin.grassAutumn) m_terrainSkin.grassAutumn->Release();
        if (m_terrainSkin.grassWinter) m_terrainSkin.grassWinter->Release();
        if (m_terrainSkin.puddleTexture) m_terrainSkin.puddleTexture->Release();
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
        if (GetAsyncKeyState('1') & 0x8000) { m_mainTree.SetSeason(Season::SPRING); m_smallTree.SetSeason(Season::SPRING); m_terrain.SetSeason(Season::SPRING); }
        if (GetAsyncKeyState('2') & 0x8000) { m_mainTree.SetSeason(Season::SUMMER); m_smallTree.SetSeason(Season::SUMMER); m_terrain.SetSeason(Season::SUMMER); }
        if (GetAsyncKeyState('3') & 0x8000) { m_mainTree.SetSeason(Season::AUTUMN); m_smallTree.SetSeason(Season::AUTUMN); m_terrain.SetSeason(Season::AUTUMN); }
        if (GetAsyncKeyState('4') & 0x8000) { m_mainTree.SetSeason(Season::WINTER); m_smallTree.SetSeason(Season::WINTER); m_terrain.SetSeason(Season::WINTER); }

        // Keyboard input to toggle Rain for puddle testing
        static bool was5Pressed = false;
        bool is5Pressed = (GetAsyncKeyState('5') & 0x8000);
        static Weather currentWeather = Weather::SUNNY;
        if (is5Pressed && !was5Pressed) {
            currentWeather = (currentWeather == Weather::SUNNY) ? Weather::RAINING : Weather::SUNNY;
            m_terrain.SetWeather(currentWeather);
        }
        was5Pressed = is5Pressed;

        m_mainTree.Update(deltaTime, m_windSpeed);
        m_smallTree.Update(deltaTime, m_windSpeed * 1.5f); // Smaller tree affected more by wind
        m_terrain.Update(deltaTime);
    }

    void SandboxApp::OnRender(Graphics::Renderer& renderer)
    {
        // Initialize textures and trees on first frame
        static bool initialized = false;
        float width = renderer.GetWidth();
        float height = renderer.GetHeight();
        if (!initialized) {
            // Load Textures
            ID2D1Bitmap* spring = renderer.LoadTexture(L"d:/VS 22 projects/WeatherLIverWallpaper/WeatherLIverWallpaper/assets/leaf_spring.png");
            ID2D1Bitmap* summer = renderer.LoadTexture(L"d:/VS 22 projects/WeatherLIverWallpaper/WeatherLIverWallpaper/assets/leaf_summer.png");
            ID2D1Bitmap* autumn = renderer.LoadTexture(L"d:/VS 22 projects/WeatherLIverWallpaper/WeatherLIverWallpaper/assets/leaf_autumn.png");
            ID2D1Bitmap* winter = renderer.LoadTexture(L"d:/VS 22 projects/WeatherLIverWallpaper/WeatherLIverWallpaper/assets/snow_flake.png");

            ID2D1Bitmap* gSpring = renderer.LoadTexture(L"d:/VS 22 projects/WeatherLIverWallpaper/WeatherLIverWallpaper/assets/bg_spring.png");
            ID2D1Bitmap* gSummer = renderer.LoadTexture(L"d:/VS 22 projects/WeatherLIverWallpaper/WeatherLIverWallpaper/assets/bg_summer.png");
            ID2D1Bitmap* gAutumn = renderer.LoadTexture(L"d:/VS 22 projects/WeatherLIverWallpaper/WeatherLIverWallpaper/assets/bg_autumn.png");
            ID2D1Bitmap* gWinter = renderer.LoadTexture(L"d:/VS 22 projects/WeatherLIverWallpaper/WeatherLIverWallpaper/assets/bg_winter.png");
            ID2D1Bitmap* puddleTex = renderer.LoadTexture(L"d:/VS 22 projects/WeatherLIverWallpaper/WeatherLIverWallpaper/assets/water_puddle.png");

            // Setup Terrain Skin
            m_terrainSkin.grassSpring = gSpring;
            m_terrainSkin.grassSummer = gSummer;
            m_terrainSkin.grassAutumn = gAutumn;
            m_terrainSkin.grassWinter = gWinter;
            m_terrainSkin.puddleTexture = puddleTex;
            m_terrainSkin.cloudTexture = winter; // Use snow_flake as a soft cloud texture
            m_terrain.Init(width, height);
            m_terrain.SetSkin(m_terrainSkin);

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
        // Position trees so they sit on the crest of the rolling hills in the background image
        m_mainTree.SetTransform(width * 0.45f, height - 350.0f, 1.0f);
        m_smallTree.SetTransform(width * 0.75f, height - 450.0f, 0.6f);

        Season s = m_mainTree.GetSeason();

        // 1. Draw solid sky color based on season
        if (s == Season::SPRING) renderer.Clear(0.85f, 0.95f, 1.0f); 
        else if (s == Season::SUMMER) renderer.Clear(0.6f, 0.85f, 1.0f); 
        else if (s == Season::AUTUMN) renderer.Clear(1.0f, 0.8f, 0.6f); 
        else if (s == Season::WINTER) renderer.Clear(0.7f, 0.75f, 0.8f);

        // 2. Draw procedural clouds
        m_terrain.RenderSky(renderer);

        // 3. Draw the transparent scenic background (hills)
        m_terrain.Render(renderer);

        // Render the procedural trees
        m_mainTree.Render(renderer);
        m_smallTree.Render(renderer);
    }

}
