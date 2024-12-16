#pragma once

#include <string>
#include <vector>
#include <cmath>

#include "components/simple_scene.h"
#include "components/transform.h"
#include "core/gpu/frame_buffer.h"
#include "core/gpu/particle_effect.h"

namespace m2
{
    struct LightInfo
    {
        glm::vec3 position;
        glm::vec3 color;
        float radius;
    };


    class Tema1 : public gfxc::SimpleScene
    {
     public:
        Tema1();
        ~Tema1();

        void Init() override;
        void GenerateTerrain(float width, float height, int resolutionX, int resolutionZ, float h_max, float r);
        void AddWaterfall(std::vector<VertexFormat>& vertices, float r_cascade, glm::vec3 P0, glm::vec3 P1, glm::vec3 P2, glm::vec3 P3);
        glm::vec3 RotatePointAroundCenter(const glm::vec3& point, const glm::vec3& center, float angle);

        unsigned int Tema1::UploadCubeMapTexture(const std::string& pos_x, const std::string& pos_y, const std::string& pos_z, const std::string& neg_x, const std::string& neg_y, const std::string& neg_z);
        int cubeMapTextureID; 
        void RenderSkybox();

        void ResetParticlesRain(int xSize, int ySize, int zSize);
    
    private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;

        void LoadShader(const std::string &fileName, bool hasGeometry=false);       


     private:
        FrameBuffer *frameBuffer;
        FrameBuffer *lightBuffer;
        FrameBuffer *reflectionBuffer;
        std::vector<LightInfo> lights;
        int outputType;

        glm::vec3 P0, P1, P2, P3;
        glm::vec3 generator_position;
        float offset;
    };
}   // namespace m2
