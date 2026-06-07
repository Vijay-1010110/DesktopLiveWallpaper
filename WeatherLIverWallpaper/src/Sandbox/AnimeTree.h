#pragma once

#include "../Graphics/Renderer.h"
#include <vector>
#include <string>
#include <random>

namespace Sandbox {

    enum class Season {
        SPRING,
        SUMMER,
        AUTUMN,
        WINTER
    };

    struct LeafData {
        float offsetX;
        float offsetY;
        float baseRotation;
    };

    struct Branch {
        float x0, y0; // Start point
        float length;
        float angle;  // Base angle
        float thickness;
        int depth;
        std::vector<Branch> children;
        std::vector<LeafData> leaves; // Store leaf data so they don't jitter every frame
    };

    struct Particle {
        float x, y;
        float vx, vy;
        float rotation;
        float rotationSpeed;
        float scale;
        float life;
    };

    struct TreeSkin {
        ID2D1Bitmap* leafTextureSpring = nullptr;
        ID2D1Bitmap* leafTextureSummer = nullptr;
        ID2D1Bitmap* leafTextureAutumn = nullptr;
        ID2D1Bitmap* leafTextureWinter = nullptr;
        
        float trunkColorR = 0.25f;
        float trunkColorG = 0.15f;
        float trunkColorB = 0.1f;
        
        float winterTrunkColorR = 0.15f;
        float winterTrunkColorG = 0.15f;
        float winterTrunkColorB = 0.2f;

        float branchLengthMultiplier = 1.0f;
        float branchThicknessMultiplier = 1.0f;
    };

    class AnimeTree
    {
    public:
        AnimeTree();
        ~AnimeTree();

        void Initialize(const TreeSkin& skin);
        void SetTransform(float x, float y, float scale = 1.0f);
        void SetSeason(Season season);
        Season GetSeason() const { return m_currentSeason; }

        void Update(float deltaTime, float windSpeed);
        void Render(Graphics::Renderer& renderer);

    private:
        void GenerateTree(Branch& branch, int depth, std::mt19937& rng);
        void DrawBranch(Graphics::Renderer& renderer, const Branch& branch, float startX, float startY, float currentAngle, float windOffset, bool isLeafPass);
        void SpawnParticles(float deltaTime, float windSpeed);

        Branch m_root;
        Season m_currentSeason;
        TreeSkin m_skin;

        // Transform
        float m_x;
        float m_y;
        float m_scale;

        // Particles
        std::vector<Particle> m_particles;
        float m_time;
    };

}
