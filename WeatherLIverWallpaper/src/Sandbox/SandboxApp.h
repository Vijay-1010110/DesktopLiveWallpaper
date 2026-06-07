#pragma once

#include "../Core/Application.h"
#include "AnimeTree.h"
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

        AnimeTree m_mainTree;
        AnimeTree m_smallTree;
        
        float m_windSpeed;
    };

}
