#pragma once

#include <string>

#include "components/simple_scene.h"
#include "core/gpu/frame_buffer.h"


namespace m2
{
    class Tema2 : public gfxc::SimpleScene
    {
     public:
        Tema2();
        ~Tema2();

        void Init() override;

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

        void OpenDialog();
        void OnFileSelected(const std::string &fileName);
        void SaveImage(const std::string &fileName);

        void LoadShader(const std::string& name);
        void RenderOriginal();
        void RenderHorizontalBlur();
        void RenderTextureToScreen(GLuint texture);
        void RenderFinalBlur();
        void RenderSobel();
        void RenderPattern(GLuint fbo, int pattern, float c, float a, float b, float intensityThreshold);
        void RenderPatternComposition();
        void RenderFinal();

     private:
        Texture2D *originalImage;
        Texture2D *processedImage;

        int outputMode;

        GLuint horizontalBlurFBO, horizontalBlurTexture;
        GLuint finalBlurFBO, finalBlurTexture;
        GLuint sobelFBO, sobelTexture;
        GLuint pattern1FBO, pattern1Texture;
        GLuint pattern2FBO, pattern2Texture;
        GLuint pattern3FBO, pattern3Texture;
        GLuint patternFinalFBO, patternFinalTexture;
        GLuint finalFBO, finalTexture;
    };
}   // namespace m2
