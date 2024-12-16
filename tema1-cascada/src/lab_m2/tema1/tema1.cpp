#include "lab_m2/tema1/tema1.h"

#include <vector>
#include <iostream>
#include <cmath>
#include <glm/gtc/noise.hpp>
#include <stb/stb_image.h>

using namespace std;
using namespace m2;

inline float Rand01()
{
    return rand() / static_cast<float>(RAND_MAX);
}

/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


struct Particle
{
    glm::vec4 position;
    glm::vec4 speed;
    glm::vec4 initialPos;
    glm::vec4 initialSpeed;
    float delay;
    float initialDelay;
    float lifetime;
    float initialLifetime;

    Particle() {}

    Particle(const glm::vec4& pos, const glm::vec4& speed)
    {
        SetInitial(pos, speed);
    }

    void SetInitial(const glm::vec4& pos, const glm::vec4& speed,
        float delay = 0, float lifetime = 0)
    {
        position = pos;
        initialPos = pos;

        this->speed = speed;
        initialSpeed = speed;

        this->delay = delay;
        initialDelay = delay;

        this->lifetime = lifetime;
        initialLifetime = lifetime;
    }
};


ParticleEffect<Particle>* particleEffect;



Tema1::Tema1()
{
}


Tema1::~Tema1()
{
}


void RecalculateNormals(std::vector<VertexFormat>& vertices, const std::vector<unsigned int>& indices) {
    for (auto& vertex : vertices) {
        vertex.normal = glm::vec3(0);
    }

    for (size_t i = 0; i < indices.size(); i += 3) {
        unsigned int i1 = indices[i];
        unsigned int i2 = indices[i + 1];
        unsigned int i3 = indices[i + 2];

        glm::vec3 v1 = vertices[i1].position;
        glm::vec3 v2 = vertices[i2].position;
        glm::vec3 v3 = vertices[i3].position;

        glm::vec3 normal = glm::normalize(glm::cross(v2 - v1, v3 - v1));

        vertices[i1].normal += normal;
        vertices[i2].normal += normal;
        vertices[i3].normal += normal;
    }

    for (auto& vertex : vertices) {
        vertex.normal = glm::normalize(vertex.normal);
    }
}


void Tema1::GenerateTerrain(float width, float height, int resolutionX, int resolutionZ, float h_max, float r) {

    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;
    glm::vec3 center(0, 0, 0);

    float stepX = width / (resolutionX - 1);
    float stepZ = height / (resolutionZ - 1);

    float noiseScale = 0.14f;
    float noiseAmplitude = 1.2f;

    float maxHeight = 0.0f;

    for (int i = 0; i < resolutionZ; i++) {
        for (int j = 0; j < resolutionX; j++) {

            float x = j * stepX - width / 2.0f;
            float z = i * stepZ - height / 2.0f;

            glm::vec3 vPos(x, 0, z);
            float d = glm::distance(glm::vec2(vPos.x, vPos.z), glm::vec2(center.x, center.z)) / r;
            float h = 0.0f;

            if (d < 1.0f) {
                h = ((d * d) / 2) * h_max;
            }
            else {

                float noiseValue = glm::perlin(glm::vec2(vPos.x, vPos.z) * noiseScale);
                noiseValue = 0.5f * (noiseValue + 1.0f);

                float noiseWeight = glm::clamp((d - 1.0f) / 1.0f, 0.0f, 1.0f);

                float baseHeight = (1 - (2 - d) * (2 - d) / 2) * h_max;
                h = baseHeight + noiseWeight * noiseValue * h_max * noiseAmplitude;
            }

            maxHeight = glm::max(maxHeight, h);

            vPos.y = h;
            vertices.emplace_back(vPos, glm::vec3(0, 1, 0));
        }
    }

    glm::vec3 P0(0.0f, -1.0f, 0.0f);
    glm::vec3 P1(0.0f, 2.0f, 10.0f);
    glm::vec3 P2(0.0f, 6.0f, 20.0f);
    glm::vec3 P3(0.0f, 15.0f, 30.0f);

    float constexpr rotationAngle = glm::radians(-90.0f);
    float r_cascade = 3.0f;

    P0 = P0;
    P1 = RotatePointAroundCenter(P1, center, rotationAngle);
    P2 = RotatePointAroundCenter(P2, center, rotationAngle);
    P3 = RotatePointAroundCenter(P3, center, rotationAngle);

    AddWaterfall(vertices, r_cascade, P0, P1, P2, P3);

    // Generate indices
    for (int i = 0; i < resolutionZ - 1; ++i) {
        for (int j = 0; j < resolutionX - 1; ++j) {
            int current = i * resolutionX + j;
            int next = current + resolutionX;

            indices.push_back(current);
            indices.push_back(next);
            indices.push_back(current + 1);

            indices.push_back(current + 1);
            indices.push_back(next);
            indices.push_back(next + 1);
        }
    }

    RecalculateNormals(vertices, indices);

    Mesh* terrainMesh = new Mesh("terrain");
    terrainMesh->InitFromData(vertices, indices);
    meshes["terrain"] = terrainMesh;
}


void Tema1::AddWaterfall(std::vector<VertexFormat>& vertices, float r_cascade, glm::vec3 P0, glm::vec3 P1, glm::vec3 P2, glm::vec3 P3)
{
    
    for (auto& vertex : vertices) {

        glm::vec3 vPos = vertex.position;

        // Project vertex onto XZ plane
        glm::vec2 vXZ(vPos.x, vPos.z);

        glm::vec3 b_closest;
        float t_closest = 0.0f;
        float min_distance = std::numeric_limits<float>::max();

        // Iterate along the curve with small steps
        const int steps = 400;

        for (int i = 0; i <= steps; ++i) {

            float t = static_cast<float>(i) / steps;

            glm::vec3 B = (1 - t) * (1 - t) * (1 - t) * P0 +
                3.0f * (1 - t) * (1 - t) * t * P1 +
                3.0f * (1 - t) * t * t * P2 +
                t * t * t * P3;

            // Project point on XZ plane
            glm::vec2 bXZ(B.x, B.z);

            // Calculate distance in XZ plane
            float dist = glm::distance(vXZ, bXZ);

            if (dist < min_distance) {
                min_distance = dist;
                b_closest = B;
                t_closest = t;
            }
        }

        // Distance to the curve in XZ plane
        float d_bezier = glm::distance(vXZ, glm::vec2(b_closest.x, b_closest.z));

        // Normalize
        float d = glm::clamp(d_bezier / r_cascade, 0.0f, 1.0f);

        // Apply smoothing
        float influence = 1 - glm::sin(glm::half_pi<float>() - d * glm::half_pi<float>());
        vertex.position.y = glm::mix(b_closest.y, vertex.position.y, influence);
    }
}


glm::vec3 Tema1::RotatePointAroundCenter(const glm::vec3& point, const glm::vec3& center, float angle) {
    // Translate point to origin
    glm::vec3 translated = point - center;

    // Rotate around Y
    float cosTheta = glm::cos(angle);
    float sinTheta = glm::sin(angle);
    float x = translated.x * cosTheta - translated.z * sinTheta;
    float z = translated.x * sinTheta + translated.z * cosTheta;

    // Translate back
    return glm::vec3(x, translated.y, z) + center;
}




void Tema1::Init()
{
    outputType = 0;

    auto camera = GetSceneCamera();
    camera->SetPositionAndRotation(glm::vec3(0, 25, 35.5), glm::quat(glm::vec3(-20 * TO_RADIANS, 0, 0)));
    camera->Update();

    {
        Mesh* mesh = new Mesh("box");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "box.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = new Mesh("plane");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "plane50.obj");
        mesh->UseMaterials(false);
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = new Mesh("quad");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "quad.obj");
        mesh->UseMaterials(false);
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
		Mesh* mesh = new Mesh("sphere");
		mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "sphere.obj");
		mesh->UseMaterials(false);
		meshes[mesh->GetMeshID()] = mesh;
	}

    LoadShader("Render2Texture");
    LoadShader("Composition");
    LoadShader("LightPass");
    LoadShader("Normal");
    LoadShader("Test");


    auto resolution = window->GetResolution();

    frameBuffer = new FrameBuffer();
    frameBuffer->Generate(resolution.x, resolution.y, 3);

    lightBuffer = new FrameBuffer();
    lightBuffer->Generate(resolution.x, resolution.y, 1, false);


    for (int i = 0; i < 20; ++i)
    {
        LightInfo lightInfo;

        auto x = -20.0f + Rand01() * 40.0f;
        auto z = -20.0f + Rand01() * 40.0f;
        auto distance = glm::distance(glm::vec2(x, z), glm::vec2(0, 0));
        float y = glm::clamp((distance / 40.0f) * 20.0f, 0.0f, 20.0f) * 2.0f;

        lightInfo.position = glm::vec3(x, y, z);
        lightInfo.color = glm::vec3(Rand01(), Rand01(), Rand01());
        lightInfo.radius = Rand01() * 10.0f;

        lights.push_back(lightInfo);
    }


    // Terrain
    GenerateTerrain(50.0f, 50.0f, 100, 100, 9.0f, 9.0f);

    // Skybox 
    std::string texturePath = PATH_JOIN(window->props.selfDir, RESOURCE_PATH::TEXTURES, "tema1_cube");

    cubeMapTextureID = UploadCubeMapTexture(
        PATH_JOIN(texturePath, "pos_x.png"),
        PATH_JOIN(texturePath, "pos_y.png"),
        PATH_JOIN(texturePath, "pos_z.png"),
        PATH_JOIN(texturePath, "neg_x.png"),
        PATH_JOIN(texturePath, "neg_y.png"),
        PATH_JOIN(texturePath, "neg_z.png"));

    // Particles
    {
        TextureManager::LoadTexture(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::TEXTURES), "rain.png");
        LoadShader("Particle", true);

        ResetParticlesRain(40, 10, 10);

        glm::vec3 P0(0.0f, -1.0f, 0.0f);
        glm::vec3 P1(0.0f, 2.0f, 10.0f);
        glm::vec3 P2(0.0f, 6.0f, 20.0f);
        glm::vec3 P3(0.0f, 15.0f, 30.0f);

        float constexpr rotationAngle = glm::radians(-90.0f);

        glm::vec3 center(0, 0, 0);
        P1 = RotatePointAroundCenter(P1, center, rotationAngle);
        P2 = RotatePointAroundCenter(P2, center, rotationAngle);
        P3 = RotatePointAroundCenter(P3, center, rotationAngle);

        generator_position = P3;
        offset = 0.05;
    }
}


void Tema1::ResetParticlesRain(int xSize, int ySize, int zSize)
{
    unsigned int nrParticles = 3000;

    particleEffect = new ParticleEffect<Particle>();
    particleEffect->Generate(nrParticles, true);

    auto particleSSBO = particleEffect->GetParticleBuffer();
    Particle* data = const_cast<Particle*>(particleSSBO->GetBuffer());


    int xhSize = xSize / 2;
    int yhSize = ySize / 2;
    int zhSize = zSize / 2;

    for (unsigned int i = 0; i < nrParticles; i++)
    {
        glm::vec4 pos(1);
        pos.x = (rand() % xSize - xhSize) / 10.0f;
        pos.y = (rand() % ySize - yhSize) / 10.0f;
        pos.z = (rand() % zSize - zhSize) / 10.0f;

        glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0, 1, 0));
        pos = rotationMatrix * pos;

        glm::vec4 speed(0);
        speed.x = - (rand() % 20 - 10) / 10.0f;
        speed.z = - (rand() % 20 - 10) / 10.0f;
        speed.y = - (rand() % 2 + 2.0f);

        float delay = (rand() % 100 / 100.0f) * 3.0f;

        float lifetime = 1;

        data[i].SetInitial(pos, speed, delay, lifetime);
    }

    particleSSBO->SetBufferData(data);
}


unsigned int Tema1::UploadCubeMapTexture(const std::string& pos_x, const std::string& pos_y, const std::string& pos_z, const std::string& neg_x, const std::string& neg_y, const std::string& neg_z)
{
    int width, height, chn;

    unsigned char* data_pos_x = stbi_load(pos_x.c_str(), &width, &height, &chn, 0);
    unsigned char* data_pos_y = stbi_load(pos_y.c_str(), &width, &height, &chn, 0);
    unsigned char* data_pos_z = stbi_load(pos_z.c_str(), &width, &height, &chn, 0);
    unsigned char* data_neg_x = stbi_load(neg_x.c_str(), &width, &height, &chn, 0);
    unsigned char* data_neg_y = stbi_load(neg_y.c_str(), &width, &height, &chn, 0);
    unsigned char* data_neg_z = stbi_load(neg_z.c_str(), &width, &height, &chn, 0);

    unsigned int textureID = 0;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    if (GLEW_EXT_texture_filter_anisotropic) {
        float maxAnisotropy;

        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy);
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data_pos_x);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data_neg_x);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data_pos_y);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data_neg_y);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data_pos_z);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data_neg_z);


    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    // Free memory
    SAFE_FREE(data_pos_x);
    SAFE_FREE(data_pos_y);
    SAFE_FREE(data_pos_z);
    SAFE_FREE(data_neg_x);
    SAFE_FREE(data_neg_y);
    SAFE_FREE(data_neg_z);

    return textureID;
}


void Tema1::FrameStart()
{
}


void Tema1::RenderSkybox() {
    Shader* shader = shaders["Normal"];
    shader->Use();

    glm::mat4 modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, 10.0f, 0.0f));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(200));

    auto camera = GetSceneCamera();

    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));
    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetProjectionMatrix()));
    glUniform1i(glGetUniformLocation(shader->program, "is_skybox"), 1);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTextureID);
    int loc_texture = shader->GetUniformLocation("texture_cubemap");
    glUniform1i(loc_texture, 0);

    RenderMesh(meshes["box"], shader, modelMatrix);
}


void Tema1::Update(float deltaTimeSeconds)
{
    ClearScreen();


    for (auto& l : lights)
    {
        float constexpr radiansPerSecond = glm::radians(6.0f);
        float rotationRadians = radiansPerSecond * deltaTimeSeconds;

        glm::mat4 rotateMatrix = glm::rotate(glm::mat4(1.0f), rotationRadians, glm::vec3(0, 1, 0));
        l.position = rotateMatrix * glm::vec4(l.position, 1.0f);
    }

    glEnable(GL_DEPTH_TEST);

    auto camera = GetSceneCamera();


        // Render to the screen.
        
    {
        // ------------------------------------------------------------------------
        // Deferred rendering pass
        {
            frameBuffer->Bind();
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glViewport(0, 0, window->GetResolution().x, window->GetResolution().y);

            // Render the skybox and terrain
            RenderSkybox();
            RenderMesh(meshes["terrain"], shaders["Render2Texture"], glm::vec3(0));

            // Particles

            {
                auto shader = shaders["Particle"];
                if (shader->GetProgramID())
                {
                    shader->Use();

                    TextureManager::GetTexture("rain.png")->BindToTextureUnit(GL_TEXTURE0);
                    particleEffect->Render(GetSceneCamera(), shader);

                    glm::vec3 P0(0.0f, 1.5f, 0.0f);
                    glm::vec3 P1(0.0f, 3.0f, 10.0f);
                    glm::vec3 P2(0.0f, 7.0f, 20.0f);
                    glm::vec3 P3(0.0f, 13.0f, 25.0f);

                    float constexpr rotationAngle = glm::radians(-90.0f);

                    glm::vec3 center(0, 0, 0);
                    P1 = RotatePointAroundCenter(P1, center, rotationAngle);
                    P2 = RotatePointAroundCenter(P2, center, rotationAngle);
                    P3 = RotatePointAroundCenter(P3, center, rotationAngle);

                    generator_position = P3;

                    GLint loc_generator_position = glGetUniformLocation(shader->program, "generator_position");
                    glUniform3fv(loc_generator_position, 1, glm::value_ptr(generator_position));

                    GLint loc_deltaTime = glGetUniformLocation(shader->program, "deltaTime");
                    glUniform1f(loc_deltaTime, deltaTimeSeconds);

                    GLint loc_offset = glGetUniformLocation(shader->program, "offset");
                    glUniform1f(loc_offset, offset);
                    

                    GLint loc_P0 = glGetUniformLocation(shader->program, "P0");
                    glUniform3fv(loc_P0, 1, glm::value_ptr(P0));

                    GLint loc_P1 = glGetUniformLocation(shader->program, "P1");
                    glUniform3fv(loc_P1, 1, glm::value_ptr(P1));

                    GLint loc_P2 = glGetUniformLocation(shader->program, "P2");
                    glUniform3fv(loc_P2, 1, glm::value_ptr(P2));

                    GLint loc_P3 = glGetUniformLocation(shader->program, "P3");
                    glUniform3fv(loc_P3, 1, glm::value_ptr(P3));

                }
            }

            // Lake
            {
                auto shader = shaders["Test"];
                shader->Use();

                auto modelMatrix = glm::mat4(1);
                modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 3, 0));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5));

                RenderMesh(meshes["plane"], shader, modelMatrix);
            }
        }



        // ------------------------------------------------------------------------
        // Lighting pass
        {
            glm::vec3 ambientLight(0.3f);
            lightBuffer->SetClearColor(glm::vec4(ambientLight.x, ambientLight.y, ambientLight.z, 1.0f));
            lightBuffer->Bind();
            //glClearColor(0, 0, 0, 1);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Enable buffer color accumulation
            glDepthMask(GL_FALSE);
            glEnable(GL_BLEND);
            glBlendEquation(GL_FUNC_ADD);
            glBlendFunc(GL_ONE, GL_ONE);

            auto shader = shaders["LightPass"];
            shader->Use();

            {
                int texturePositionsLoc = shader->GetUniformLocation("texture_position");
                glUniform1i(texturePositionsLoc, 0);
                frameBuffer->BindTexture(0, GL_TEXTURE0);
            }

            {
                int textureNormalsLoc = shader->GetUniformLocation("texture_normal");
                glUniform1i(textureNormalsLoc, 1);
                frameBuffer->BindTexture(1, GL_TEXTURE0 + 1);
            }

            auto camera = GetSceneCamera();
            glm::vec3 cameraPos = camera->m_transform->GetWorldPosition();
            int loc_eyePosition = shader->GetUniformLocation("eye_position");
            glUniform3fv(loc_eyePosition, 1, glm::value_ptr(cameraPos));

            auto resolution = window->GetResolution();
            int loc_resolution = shader->GetUniformLocation("resolution");
            glUniform2i(loc_resolution, resolution.x, resolution.y);

            //Front face culling
            glEnable(GL_CULL_FACE);
            glCullFace(GL_FRONT);

            for (auto& lightInfo : lights)
            {
                glUniform1f(shader->GetUniformLocation("light_radius"), lightInfo.radius);
                glUniform3fv(shader->GetUniformLocation("light_position"), 1, glm::value_ptr(lightInfo.position));
                glUniform3fv(shader->GetUniformLocation("light_color"), 1, glm::value_ptr(lightInfo.color));

                RenderMesh(meshes["sphere"], shader, lightInfo.position, glm::vec3(lightInfo.radius * 2));

            }

            glDisable(GL_CULL_FACE);

            glDepthMask(GL_TRUE);
            glDisable(GL_BLEND);

        }



        // ------------------------------------------------------------------------
        // Composition pass
        {
            FrameBuffer::BindDefault();
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glViewport(0, 0, window->GetResolution().x, window->GetResolution().y);


            auto shader = shaders["Composition"];
            shader->Use();

            int outputTypeLoc = shader->GetUniformLocation("output_type");
            glUniform1i(outputTypeLoc, outputType);

            {
                int texturePositionsLoc = shader->GetUniformLocation("texture_position");
                glUniform1i(texturePositionsLoc, 1);
                frameBuffer->BindTexture(0, GL_TEXTURE0 + 1);
            }

            {
                int textureNormalsLoc = shader->GetUniformLocation("texture_normal");
                glUniform1i(textureNormalsLoc, 2);
                frameBuffer->BindTexture(1, GL_TEXTURE0 + 2);
            }

            {
                int textureColorLoc = shader->GetUniformLocation("texture_color");
                glUniform1i(textureColorLoc, 3);
                frameBuffer->BindTexture(2, GL_TEXTURE0 + 3);
            }

            {
                int textureDepthLoc = shader->GetUniformLocation("texture_depth");
                glUniform1i(textureDepthLoc, 4);
                frameBuffer->BindDepthTexture(GL_TEXTURE0 + 4);
            }

            {
                int textureLightLoc = shader->GetUniformLocation("texture_light");
                glUniform1i(textureLightLoc, 5);
                lightBuffer->BindTexture(0, GL_TEXTURE0 + 5);
            }
            
            
            RenderMesh(meshes["quad"], shader, glm::vec3(0, 0, 0));


        }
    }
}



void Tema1::FrameEnd()
{
}


void Tema1::LoadShader(const std::string& name, bool hasGeometry)
{
    std::string shaderPath = PATH_JOIN(window->props.selfDir, SOURCE_PATH::M2, "tema1", "shaders");

    {
        Shader* shader = new Shader(name);
        shader->AddShader(PATH_JOIN(shaderPath, name + ".VS.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(shaderPath, name + ".FS.glsl"), GL_FRAGMENT_SHADER);
        if (hasGeometry)
        {
            shader->AddShader(PATH_JOIN(shaderPath, name + ".GS.glsl"), GL_GEOMETRY_SHADER);
        }

        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }
}



/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void Tema1::OnInputUpdate(float deltaTime, int mods)
{
    // Treat continuous update based on input
}


void Tema1::OnKeyPress(int key, int mods)
{
    int index = key - GLFW_KEY_0;
    if (index >= 0 && index <= 9)
    {
        outputType = index;
    }
}


void Tema1::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Tema1::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event
}


void Tema1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
}


void Tema1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Tema1::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
    // Treat mouse scroll event
}


void Tema1::OnWindowResize(int width, int height)
{
    // Treat window resize event
    frameBuffer->Resize(width, height, 32);
    lightBuffer->Resize(width, height, 32);
}
