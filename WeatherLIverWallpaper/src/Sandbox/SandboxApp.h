#pragma once

#include "../Core/Application.h"
#include "AnimeTree.h"
#include "Terrain.h"
#include <math.h>

namespace Sandbox {

    class SandboxApp : public Core::Application
    {
    public:
        SandboxApp() : m_windSpeed(1.0f) {}
        ~SandboxApp() override;

    protected:
        void OnInit() override;
        void OnUpdate(float deltaTime) override;
        void OnRender(Graphics::Renderer& renderer) override;

    private:
        TreeSkin m_mainSkin;
        TreeSkin m_smallSkin;

        TerrainSkin m_terrainSkin;

        AnimeTree m_mainTree;
        AnimeTree m_smallTree;
        
        Terrain m_terrain;
        
        float m_windSpeed;
    };

}
