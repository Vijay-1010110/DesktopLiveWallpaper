#include "AnimeTree.h"
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace Sandbox {

    AnimeTree::AnimeTree() :
        m_currentSeason(Season::SPRING),
        m_time(0.0f),
        m_x(0.0f), m_y(0.0f), m_scale(1.0f)
    {
    }

    AnimeTree::~AnimeTree()
    {
        // Textures are now owned by the TreeSkin / SandboxApp, so we don't release them here!
    }

    void AnimeTree::Initialize(const TreeSkin& skin)
    {
        m_skin = skin;

        std::random_device rd;
        std::mt19937 rng(rd());

        // FIX: Set root angle to 0. We will inject -PI/2 as the base angle during rendering.
        m_root.length = 200.0f * m_skin.branchLengthMultiplier;
        m_root.angle = 0.0f; 
        m_root.thickness = 25.0f * m_skin.branchThicknessMultiplier;
        m_root.depth = 0;
        
        GenerateTree(m_root, 6, rng); // Depth of 6
    }

    void AnimeTree::SetTransform(float x, float y, float scale)
    {
        m_x = x;
        m_y = y;
        m_scale = scale;
    }

    void AnimeTree::GenerateTree(Branch& branch, int maxDepth, std::mt19937& rng)
    {
        if (branch.depth >= maxDepth) return;

        std::uniform_real_distribution<float> angleDist(0.2f, 0.6f);
        std::uniform_real_distribution<float> lengthDist(0.6f, 0.8f);

        int numBranches = (branch.depth == 0) ? 3 : 2;
        if (branch.depth > 3 && std::uniform_real_distribution<float>(0.0f, 1.0f)(rng) > 0.5f) {
            numBranches = 1;
        }

        for (int i = 0; i < numBranches; ++i) {
            Branch child;
            child.depth = branch.depth + 1;
            child.length = branch.length * lengthDist(rng);
            child.thickness = branch.thickness * 0.7f;
            
            float sign = (i % 2 == 0) ? 1.0f : -1.0f;
            if (numBranches == 3 && i == 2) sign = 0.0f; // Middle branch
            
            child.angle = sign * angleDist(rng);
            if (sign == 0.0f) child.angle = angleDist(rng) * 0.2f - 0.1f; // Slight wobble for middle
            
            GenerateTree(child, maxDepth, rng);
            branch.children.push_back(child);
        }

        // Generate leaves for this branch if it's an end branch or high depth
        if (branch.children.empty() || branch.depth >= 4) {
            std::uniform_real_distribution<float> rotDist(-30.0f, 30.0f); // Fan out +/- 30 degrees from branch angle
            
            for (int i = 0; i < 3; ++i) {
                LeafData leaf;
                leaf.offsetX = 0.0f; // No spatial offset, attach exactly at the joint
                leaf.offsetY = 0.0f;
                leaf.baseRotation = rotDist(rng);
                branch.leaves.push_back(leaf);
            }
        }
    }

    void AnimeTree::SetSeason(Season season)
    {
        m_currentSeason = season;
    }

    void AnimeTree::Update(float deltaTime, float windSpeed)
    {
        m_time += deltaTime;
        SpawnParticles(deltaTime, windSpeed);

        // Update particles
        for (auto it = m_particles.begin(); it != m_particles.end(); ) {
            it->x += it->vx * deltaTime + windSpeed * deltaTime * 100.0f * m_scale;
            it->y += it->vy * deltaTime;
            it->rotation += it->rotationSpeed * deltaTime;
            it->life -= deltaTime;

            if (it->life <= 0.0f || it->y > m_y + 1000.0f * m_scale) {
                it = m_particles.erase(it);
            } else {
                ++it;
            }
        }
    }

    void AnimeTree::SpawnParticles(float deltaTime, float windSpeed)
    {
        float spawnChance = 0.0f;
        if (m_currentSeason == Season::SPRING) spawnChance = 0.3f; // Cherry blossoms falling
        else if (m_currentSeason == Season::AUTUMN) spawnChance = 0.8f; // Lots of leaves falling
        else if (m_currentSeason == Season::WINTER) spawnChance = 1.0f; // Snowing
        else if (m_currentSeason == Season::SUMMER) spawnChance = 0.05f; // Occasional leaf

        spawnChance *= (1.0f + abs(windSpeed)); // Wind knocks more leaves off

        std::random_device rd;
        std::mt19937 rng(rd());
        std::uniform_real_distribution<float> chance(0.0f, 1.0f);

        if (chance(rng) < spawnChance * deltaTime * 60.0f) {
            std::uniform_real_distribution<float> xDist(m_x - 400.0f * m_scale, m_x + 400.0f * m_scale);
            std::uniform_real_distribution<float> yDist(m_y - 800.0f * m_scale, m_y - 200.0f * m_scale);
            
            if (m_currentSeason == Season::WINTER) {
                // Snow falls from everywhere above the tree
                xDist = std::uniform_real_distribution<float>(m_x - 1200.0f, m_x + 1200.0f);
                yDist = std::uniform_real_distribution<float>(0.0f, m_y - 800.0f * m_scale);
            }

            Particle p;
            p.x = xDist(rng);
            p.y = yDist(rng);
            p.vx = std::uniform_real_distribution<float>(-20.0f * m_scale, 20.0f * m_scale)(rng);
            p.vy = std::uniform_real_distribution<float>(30.0f * m_scale, 150.0f * m_scale)(rng);
            p.rotation = chance(rng) * M_PI * 2.0f;
            p.rotationSpeed = std::uniform_real_distribution<float>(-2.0f, 2.0f)(rng);
            p.scale = std::uniform_real_distribution<float>(0.3f * m_scale, 0.7f * m_scale)(rng);
            p.life = 15.0f;
            
            m_particles.push_back(p);
        }
    }

    void AnimeTree::Render(Graphics::Renderer& renderer)
    {
        // Calculate wind influence using m_time
        float windOffset = sin(m_time + m_x) * 0.05f;

        // Base angle is -90 degrees (pointing straight up)
        float baseAngle = -M_PI / 2.0f;
        
        // Two-pass rendering: First draw all branches, then draw all leaves on top
        DrawBranch(renderer, m_root, m_x, m_y, baseAngle, windOffset, false);
        DrawBranch(renderer, m_root, m_x, m_y, baseAngle, windOffset, true);

        // Draw particles
        ID2D1Bitmap* tex = nullptr;
        if (m_currentSeason == Season::SPRING) tex = m_skin.leafTextureSpring;
        if (m_currentSeason == Season::SUMMER) tex = m_skin.leafTextureSummer;
        if (m_currentSeason == Season::AUTUMN) tex = m_skin.leafTextureAutumn;
        if (m_currentSeason == Season::WINTER) tex = m_skin.leafTextureWinter;

        if (tex) {
            for (const auto& p : m_particles) {
                float opacity = (p.life < 1.0f) ? p.life : 1.0f;
                float rotDeg = p.rotation * (180.0f / M_PI);
                float size = 64.0f * p.scale;
                // Center the particle drawing on its coordinates
                renderer.DrawTexture(tex, p.x - size/2.0f, p.y - size/2.0f, size, size, opacity, rotDeg, 0.5f, 0.5f);
            }
        }
    }

    void AnimeTree::DrawBranch(Graphics::Renderer& renderer, const Branch& branch, float startX, float startY, float currentAngle, float windOffset, bool isLeafPass)
    {
        // Add wind sway (thinner branches sway more)
        float sway = windOffset * (branch.depth * 0.5f + 1.0f);
        float finalAngle = currentAngle + branch.angle + sway;
        float endX = startX + cos(finalAngle) * branch.length * m_scale;
        float endY = startY + sin(finalAngle) * branch.length * m_scale;

        if (!isLeafPass) {
            // Pass 1: Draw the physical branch lines
            float r = m_skin.trunkColorR;
            float g = m_skin.trunkColorG;
            float b = m_skin.trunkColorB;
            if (m_currentSeason == Season::WINTER) {
                r = m_skin.winterTrunkColorR; 
                g = m_skin.winterTrunkColorG; 
                b = m_skin.winterTrunkColorB;
            }
            float scaledThickness = branch.thickness * m_scale;
            renderer.DrawLine(startX, startY, endX, endY, r, g, b, scaledThickness);
        } else {
            // Pass 2: Draw the leaves on top of the branches
            if (!branch.leaves.empty()) {
                ID2D1Bitmap* tex = nullptr;
                if (m_currentSeason == Season::SPRING) tex = m_skin.leafTextureSpring;
                if (m_currentSeason == Season::SUMMER) tex = m_skin.leafTextureSummer;
                if (m_currentSeason == Season::AUTUMN) tex = m_skin.leafTextureAutumn;

                if (tex && m_currentSeason != Season::WINTER) {
                    int leafCount = (m_currentSeason == Season::AUTUMN) ? 1 : branch.leaves.size();

                    for (int i = 0; i < leafCount; ++i) {
                        const auto& leaf = branch.leaves[i];
                        float size = 64.0f * m_scale;
                        
                        // Determine pivot point and rotation offset based on the specific PNG image
                        float pivotX = 0.5f;
                        float pivotY = 0.5f;
                        float textureOffset = 0.0f;

                        if (m_currentSeason == Season::SPRING) {
                            // Pink petal: stem is at bottom-left, points top-right
                            pivotX = 0.05f;
                            pivotY = 0.95f;
                            textureOffset = 45.0f;
                        } else if (m_currentSeason == Season::SUMMER) {
                            // Green maple: stem is at bottom-leftish, points top-right
                            pivotX = 0.25f;
                            pivotY = 0.95f;
                            textureOffset = 45.0f;
                        } else if (m_currentSeason == Season::AUTUMN) {
                            // Orange maple: stem is at bottom-right, points top-left
                            pivotX = 0.8f;
                            pivotY = 0.95f;
                            textureOffset = 135.0f;
                        }
                        
                        // Position X and Y so that the pivot point sits EXACTLY at endX, endY
                        float lx = endX - (size * pivotX);
                        float ly = endY - (size * pivotY);
                        
                        // Calculate branch direction in degrees
                        float branchAngleDeg = finalAngle * (180.0f / M_PI);
                        
                        float rot = branchAngleDeg + textureOffset + leaf.baseRotation + (sway * 50.0f);
                        
                        renderer.DrawTexture(tex, lx, ly, size, size, 1.0f, rot, pivotX, pivotY);
                    }
                }
            }
        }

        // Always recurse regardless of pass
        for (const auto& child : branch.children) {
            DrawBranch(renderer, child, endX, endY, finalAngle, windOffset, isLeafPass);
        }
    }

}
