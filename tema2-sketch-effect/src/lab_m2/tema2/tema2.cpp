#include "lab_m2/lab8/lab8.h"

#include <vector>
#include <iostream>

#include "pfd/portable-file-dialogs.h"

using namespace std;
using namespace m2;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Tema2::Tema2()
{
    outputMode = 8;
    window->SetSize(600, 600);
}


Tema2::~Tema2()
{
}


void Tema2::Init()
{
    // Load default texture fore imagine processing
    originalImage = TextureManager::LoadTexture(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::TEXTURES, "cube", "pos_x.png"), nullptr, "image", true, true);
    processedImage = TextureManager::LoadTexture(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::TEXTURES, "cube", "pos_x.png"), nullptr, "newImage", true, true);

    {
        Mesh* mesh = new Mesh("quad");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "quad.obj");
        mesh->UseMaterials(false);
        meshes[mesh->GetMeshID()] = mesh;
    }

    // Load shaders
    LoadShader("Default");
    LoadShader("HorizontalBlur");
    LoadShader("VerticalBlur");
    LoadShader("Sobel");
    LoadShader("Pattern");
    LoadShader("Composition");

    // Set up horizontal blur FBO
    {
        glGenFramebuffers(1, &horizontalBlurFBO);
        glGenTextures(1, &horizontalBlurTexture);

        glBindTexture(GL_TEXTURE_2D, horizontalBlurTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, window->GetResolution().x, window->GetResolution().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glBindFramebuffer(GL_FRAMEBUFFER, horizontalBlurFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, horizontalBlurTexture, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    // Set up final blur FBO
    {
        glGenFramebuffers(1, &finalBlurFBO);
		glGenTextures(1, &finalBlurTexture);

		glBindTexture(GL_TEXTURE_2D, finalBlurTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, window->GetResolution().x, window->GetResolution().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glBindFramebuffer(GL_FRAMEBUFFER, finalBlurFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, finalBlurTexture, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    // Set up Sobel FBO
    {
        glGenFramebuffers(1, &sobelFBO);
		glGenTextures(1, &sobelTexture);

		glBindTexture(GL_TEXTURE_2D, sobelTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, window->GetResolution().x, window->GetResolution().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glBindFramebuffer(GL_FRAMEBUFFER, sobelFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, sobelTexture, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    // Set un pattern 1 FBO
    {
        glGenFramebuffers(1, &pattern1FBO);
        glGenTextures(1, &pattern1Texture);

        glBindTexture(GL_TEXTURE_2D, pattern1Texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, window->GetResolution().x, window->GetResolution().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glBindFramebuffer(GL_FRAMEBUFFER, pattern1FBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pattern1Texture, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    // Set up pattern 2 FBO
    {
        glGenFramebuffers(1, &pattern2FBO);
		glGenTextures(1, &pattern2Texture);

		glBindTexture(GL_TEXTURE_2D, pattern2Texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, window->GetResolution().x, window->GetResolution().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glBindFramebuffer(GL_FRAMEBUFFER, pattern2FBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pattern2Texture, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    // Set up pattern 3 FBO
    {
        glGenFramebuffers(1, &pattern3FBO);
        glGenTextures(1, &pattern3Texture);

        glBindTexture(GL_TEXTURE_2D, pattern3Texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, window->GetResolution().x, window->GetResolution().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glBindFramebuffer(GL_FRAMEBUFFER, pattern3FBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pattern3Texture, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    // Set up final pattern FBO
    {
        glGenFramebuffers(1, &patternFinalFBO);
		glGenTextures(1, &patternFinalTexture);

		glBindTexture(GL_TEXTURE_2D, patternFinalTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, window->GetResolution().x, window->GetResolution().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glBindFramebuffer(GL_FRAMEBUFFER, patternFinalFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, patternFinalTexture, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    // Set up final image FBO
    {
        glGenFramebuffers(1, &finalFBO);
        glGenTextures(1, &finalTexture);

        glBindTexture(GL_TEXTURE_2D, finalTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, window->GetResolution().x, window->GetResolution().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glBindFramebuffer(GL_FRAMEBUFFER, finalFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, finalTexture, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}

void Tema2::LoadShader(const string& name) {

    std::string shaderPath = PATH_JOIN(window->props.selfDir, SOURCE_PATH::M2, "Lab8", "shaders");

    auto shader = new Shader(name);
	shader->AddShader(PATH_JOIN(shaderPath, "Default.VS.glsl"), GL_VERTEX_SHADER);
	shader->AddShader(PATH_JOIN(shaderPath, name + ".FS.glsl"), GL_FRAGMENT_SHADER);

	shader->CreateAndLink();
	shaders[shader->GetName()] = shader;
}

void Tema2::FrameStart()
{
}


void Tema2::RenderOriginal() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    auto simpleShader = shaders["Default"];
    simpleShader->Use();

    glActiveTexture(GL_TEXTURE0);
    originalImage->BindToTextureUnit(GL_TEXTURE0);

    int textureLoc = simpleShader->GetUniformLocation("textureImage");
    glUniform1i(textureLoc, 0);

    int flipLoc = simpleShader->GetUniformLocation("flipVertical");
    glUniform1i(flipLoc, 1);

    int screenSizeLoc = simpleShader->GetUniformLocation("screenSize");
    glm::ivec2 resolution = window->GetResolution();
    glUniform2i(screenSizeLoc, resolution.x, resolution.y);

    RenderMesh(meshes["quad"], simpleShader, glm::mat4(1));
}


void Tema2::RenderTextureToScreen(GLuint texture) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    auto simpleShader = shaders["Default"];
    simpleShader->Use();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    int textureLoc = simpleShader->GetUniformLocation("textureImage");
    glUniform1i(textureLoc, 0);

    int flipLoc = simpleShader->GetUniformLocation("flipVertical");
    glUniform1i(flipLoc, 1);

    int screenSizeLoc = simpleShader->GetUniformLocation("screenSize");
    glm::ivec2 resolution = window->GetResolution();
    glUniform2i(screenSizeLoc, resolution.x, resolution.y);

    RenderMesh(meshes["quad"], simpleShader, glm::mat4(1));
}


void Tema2::RenderHorizontalBlur() {
    glBindFramebuffer(GL_FRAMEBUFFER, horizontalBlurFBO);
    glClear(GL_COLOR_BUFFER_BIT);

    auto horizontalShader = shaders["HorizontalBlur"];
    horizontalShader->Use();

    glUniform1i(horizontalShader->GetUniformLocation("blurRadius"), 12);
    glUniform2i(horizontalShader->GetUniformLocation("screenSize"), window->GetResolution().x, window->GetResolution().y);

    originalImage->BindToTextureUnit(GL_TEXTURE0);
    RenderMesh(meshes["quad"], horizontalShader, glm::mat4(1));
}


void Tema2::RenderFinalBlur() {
    RenderHorizontalBlur();

    glBindFramebuffer(GL_FRAMEBUFFER, finalBlurFBO);
    glClear(GL_COLOR_BUFFER_BIT);

    auto verticalShader = shaders["VerticalBlur"];
    verticalShader->Use();

    glUniform1i(verticalShader->GetUniformLocation("blurRadius"), 12);
    glUniform2i(verticalShader->GetUniformLocation("screenSize"), window->GetResolution().x, window->GetResolution().y);

    glBindTexture(GL_TEXTURE_2D, horizontalBlurTexture);
    RenderMesh(meshes["quad"], verticalShader, glm::mat4(1));
}


void Tema2::RenderSobel() {
    glBindFramebuffer(GL_FRAMEBUFFER, sobelFBO);
    glClear(GL_COLOR_BUFFER_BIT);

    auto sobelShader = shaders["Sobel"];
    sobelShader->Use();

    glUniform2i(sobelShader->GetUniformLocation("screenSize"), window->GetResolution().x, window->GetResolution().y);

    glActiveTexture(GL_TEXTURE0);
    originalImage->BindToTextureUnit(GL_TEXTURE0);
    RenderMesh(meshes["quad"], sobelShader, glm::mat4(1));
}

void Tema2::RenderPattern(GLuint fbo, int pattern, float c, float a, float b, float intensityThreshold) {
    RenderFinalBlur();

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glClear(GL_COLOR_BUFFER_BIT);

    auto patternShader = shaders["Pattern"];
    patternShader->Use();

    glUniform1i(patternShader->GetUniformLocation("pattern"), pattern);
    glUniform1f(patternShader->GetUniformLocation("c"), c);
    glUniform2f(patternShader->GetUniformLocation("direction"), a, b);
    glUniform1f(patternShader->GetUniformLocation("intensityThreshold"), intensityThreshold);
    glUniform2i(patternShader->GetUniformLocation("screenSize"), window->GetResolution().x, window->GetResolution().y);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, finalBlurTexture);
    RenderMesh(meshes["quad"], patternShader, glm::mat4(1));
}

void Tema2::RenderPatternComposition() {
    RenderPattern(pattern1FBO, 0, 0.90f, 550.0f, -550.0f, 0.105f);
    RenderPattern(pattern2FBO, 1, 0.95f, 400.0f, 400.0f, 0.68f);
    RenderPattern(pattern3FBO, 1, 0.99f, 100.0f, -100.0f, 0.45f);

    glBindFramebuffer(GL_FRAMEBUFFER, patternFinalFBO);
    glClear(GL_COLOR_BUFFER_BIT);

    auto finalPatternShader = shaders["Composition"];
    finalPatternShader->Use();

    glUniform2i(finalPatternShader->GetUniformLocation("screenSize"), window->GetResolution().x, window->GetResolution().y);
    glUniform1i(finalPatternShader->GetUniformLocation("patternCount"), 3);

    auto pattern1Loc = finalPatternShader->GetUniformLocation("pattern1Texture");
    auto pattern2Loc = finalPatternShader->GetUniformLocation("pattern2Texture");
    auto pattern3Loc = finalPatternShader->GetUniformLocation("pattern3Texture");

    glUniform1i(pattern1Loc, 0);
    glUniform1i(pattern2Loc, 1);
    glUniform1i(pattern3Loc, 2);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, pattern1Texture);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, pattern2Texture);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, pattern3Texture);

    RenderMesh(meshes["quad"], finalPatternShader, glm::mat4(1));
}


void Tema2::RenderFinal() {
    RenderSobel();
    RenderPatternComposition();

    glBindFramebuffer(GL_FRAMEBUFFER, finalFBO);
    glClear(GL_COLOR_BUFFER_BIT);

    auto finalShader = shaders["Composition"];
    finalShader->Use();

    glUniform2i(finalShader->GetUniformLocation("screenSize"), window->GetResolution().x, window->GetResolution().y);
    glUniform1i(finalShader->GetUniformLocation("patternCount"), 2);

    auto pattern1Loc = finalShader->GetUniformLocation("pattern1Texture");
    auto pattern2Loc = finalShader->GetUniformLocation("pattern2Texture");

    glUniform1i(pattern1Loc, 0);
    glUniform1i(pattern2Loc, 1);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sobelTexture);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, patternFinalTexture);

    RenderMesh(meshes["quad"], finalShader, glm::mat4(1));
}


void Tema2::Update(float deltaTimeSeconds)
{
    ClearScreen();

    switch (outputMode) {
        
        case 0:
        {
            RenderOriginal();
            break;
        }

        case 1:
        {
            RenderHorizontalBlur();
            RenderTextureToScreen(horizontalBlurTexture);
            break;
        }

        case 2:
        {
            RenderFinalBlur();
			RenderTextureToScreen(finalBlurTexture);
			break;
        }

        case 3:
        {
            RenderSobel();
            RenderTextureToScreen(sobelTexture);
			break;
        }

        case 4:
        {
            RenderPattern(pattern1FBO, 0, 0.90f, 550.0f, -550.0f, 0.105f);
            RenderTextureToScreen(pattern1Texture);
            break;
        }

        case 5:
        {
            RenderPattern(pattern2FBO, 1, 0.95f, 400.0f, 400.0f, 0.68f);
            RenderTextureToScreen(pattern2Texture);
			break;
        }

        case 6:
        {
            RenderPattern(pattern3FBO, 1, 0.99f, 100.0f, -100.0f, 0.45f);
            RenderTextureToScreen(pattern3Texture);
            break;
        }
        
        case 7:
        {
            RenderPatternComposition();
            RenderTextureToScreen(patternFinalTexture);
            break;
        }

        case 8:
        {
            RenderFinal();
            RenderTextureToScreen(finalTexture);
            break;
        }

        default:
            RenderOriginal();
            break;

    }    
}


void Tema2::FrameEnd()
{
    DrawCoordinateSystem();
}


void Tema2::OnFileSelected(const std::string &fileName)
{
    if (fileName.size())
    {
        std::cout << fileName << endl;
        originalImage = TextureManager::LoadTexture(fileName, nullptr, "image", true, true);
        processedImage = TextureManager::LoadTexture(fileName, nullptr, "newImage", true, true);

        float aspectRatio = static_cast<float>(originalImage->GetWidth()) / originalImage->GetHeight();

        if (aspectRatio >= 1.0f) {
            window->SetSize(static_cast<int>(600 * aspectRatio), 600);
        }
        else {
            window->SetSize(600, static_cast<int>(600 / aspectRatio));
        }
    }
}

void Tema2::SaveImage(const std::string &fileName)
{
    cout << "Saving image! ";
    processedImage->SaveToFile((fileName + ".png").c_str());
    cout << "[Done]" << endl;
}


void Tema2::OpenDialog()
{
    std::vector<std::string> filters =
    {
        "Image Files", "*.png *.jpg *.jpeg *.bmp",
        "All Files", "*"
    };

    auto selection = pfd::open_file("Select a file", ".", filters).result();
    if (!selection.empty())
    {
        std::cout << "User selected file " << selection[0] << "\n";
        OnFileSelected(selection[0]);
    }
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void Tema2::OnInputUpdate(float deltaTime, int mods)
{
    // Treat continuous update based on input
}


void Tema2::OnKeyPress(int key, int mods)
{
    // Add key press event
    if (key == GLFW_KEY_F || key == GLFW_KEY_ENTER || key == GLFW_KEY_SPACE)
    {
        OpenDialog();
    }

    if (key >= GLFW_KEY_0 && key <= GLFW_KEY_9)
	{
		outputMode = key - GLFW_KEY_0;
	}
}

void Tema2::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Tema2::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event
}


void Tema2::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
}


void Tema2::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Tema2::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
    // Treat mouse scroll event
}


void Tema2::OnWindowResize(int width, int height)
{
    // Treat window resize event
}
