#include "PulseEngineBackend.h"
#include "Common/common.h"
#include "camera.h"
#include "PulseEngine/core/Entity/Entity.h"
#include "PulseEngine/core/Material/Material.h"
#include "PulseEngine/core/GUID/GuidGenerator.h"
#include "json.hpp"
#include <fstream>
#include "PulseEngine/core/Meshes/primitive/Primitive.h"
#include "PulseEngine/core/Material/MaterialManager.h"
#include "PulseEngine/core/GUID/GuidReader.h"
#include "PulseEngine/core/SceneLoader/SceneLoader.h"
#include "PulseEngine/core/Lights/Lights.h"
#include "PulseEngine/core/FileManager/FileManager.h"
#include "PulseEngine/core/Lights/DirectionalLight/DirectionalLight.h"
#include "PulseEngine/core/Lights/PointLight/PointLight.h"
#include "PulseEngine/CustomScripts/ScriptsLoader.h"
#include "PulseEngine/core/WindowContext/WindowContext.h"
#include "PulseEngine/core/Graphics/IGraphicsApi.h"
#include "PulseEngine/core/Physics/Collider/Collider.h"
#include "PulseEngine/core/Physics/Collider/BoxCollider.h"
#include "PulseEngine/core/Lights/LightManager.h"
#include "PulseEngine/core/Physics/CollisionManager.h"
#include "PulseEngine/core/coroutine/CoroutineManager.h"
#include "PulseEngine/core/coroutine/Coroutine.h"
#include "PulseEngine/core/GUID/GuidCollection.h"
#include "PulseEngine/core/ExecutableManager/PulseExecutable.h"
#include "PulseEngine/core/Input/InputSystem.h"
#include "PulseEngineEditor/InterfaceEditor/Account/Account.h"

#include "PulseEngine/core/Math/MathUtils.h"

#include "PulseEngine/core/Network/Request/Request.h"

#ifdef PULSE_GRAPHIC_OPENGL
#include "PulseEngine/core/Graphics/OpenGLAPI/OpenGLApi.h"
#endif

#include "Common/dllExport.h"

Camera* PulseEngineBackend::activeCamera = new Camera();
IGraphicsAPI* PulseEngineBackend::graphicsAPI = nullptr;
PulseEngineBackend* PulseEngineBackend::instance = nullptr;
float PulseEngineBackend::deltaTime = 0.0f;

#ifdef ENGINE_EDITOR
InterfaceEditor* PulseEngineBackend::editor = nullptr;
#endif

PulseEngineBackend::PulseEngineBackend() 
{ 
    account = new Account("dlexteriaque@gmail.com", "passwordTest");
}

int PulseEngineBackend::Initialize()
{
    EDITOR_LOG("Started the initialization of the engine.");
    EDITOR_LOG("Engine version: " + version);
    EDITOR_LOG("Development month: " + devMonth);
    EDITOR_LOG("Project name : " + gameName);
    EDITOR_LOG("Pulse Software © 2025");

    windowContext = new WindowContext();
    activeCamera = new Camera();

    // load the graphic API based on the platform
    //some platform can have multiple graphic API possible.
    // graphicsAPI = dynamic_cast<IGraphicsAPI*>(ModuleLoader::GetModuleFromPath("Modules/OpenGLApi.dll"));
    /**
     * @note For renderer, we wont use dynamic DLL to load the graphic API, we will use inside the game engine dll one. So we need to create a game engine dll for each platform.
     * 
     */
    #ifdef PULSE_GRAPHIC_OPENGL
    graphicsAPI = new OpenGLAPI();
    #endif

    if(graphicsAPI == nullptr)
    {
        EDITOR_ERROR("No Graphics API found.");
        return -1;
    }
    graphicsAPI->InitializeApi(GetWindowName("editor").c_str(), &width, &height, this);
    
    #ifdef PULSE_GRAPHIC_OPENGL
    windowContext->SetGLFWWindow(static_cast<GLFWwindow*>(graphicsAPI->GetNativeHandle()));
    #endif

    coroutineManager = new CoroutineManager;
    inputSystem = new InputSystem;

    shadowShader = new Shader(std::string(ASSET_PATH) + "shaders/directionalDepth/dirDepth.vert", std::string(ASSET_PATH) + "shaders/directionalDepth/dirDepth.frag");
    pointLightShadowShader = new Shader(std::string(ASSET_PATH) + "shaders/pointDepth/pointDepth.vert", std::string(ASSET_PATH) + "shaders/pointDepth/pointDepth.frag", std::string(ASSET_PATH) + "shaders/pointDepth/pointDepth.glsl");
    debugShader = new Shader(std::string(ASSET_PATH) +"shaders/debug.vert", std::string(ASSET_PATH) + "shaders/debug.frag");
    
    // === insert base item to the collection ===
    GuidReader::InsertIntoCollection("Entities/simpleActor.pEntity");    
    GuidReader::InsertIntoCollection("Entities/primitiveCube.pEntity"); 

    // === initialize each collection found in the asset folder ===
    std::vector<std::filesystem::path> guidFiles = FileManager::GetFilesInDirectoryWithExtension(std::string(ASSET_PATH) + "Guid", ".puid");
    for (const auto& file : guidFiles) 
    {
        EDITOR_LOG("Guid/" + file.filename().string());
        guidCollections[file.filename().string()] = new GuidCollection("Guid/" + file.filename().string());
    }
    
    
    EDITOR_ONLY(
        ScriptsLoader::LoadDLL();
    )

 
    engineConfig = FileManager::OpenEngineConfigFile(this);

    std::string firstScene = engineConfig["GameData"]["FirstScene"];
    SceneLoader::LoadScene(firstScene, this);

    discordLauncher = new PulseExecutable ("DiscordPresence/DiscordPresence.exe", "DiscordPipeTest");
    Sleep(1500);
    discordLauncher->SendExeMessage("[set_presence]In the editor");

    EDITOR_LOG("Finished the initialization of the engine.");
    return 0;
}





std::string PulseEngineBackend::GetWindowName(const std::string &location)
{
    if(gameName != "NULL") return std::string(gameName) + " - " + gameVersion;
    else return engine + " | " + version +  " | " + location +  " | " + devMonth + " | " + company + " <" + graphicsAPI->GetApiName() + ">";
}

void PulseEngineBackend::SetWindowName(const std::string &location)
{
    DWORD bytesWritten;
    std::string message = "[set_presence]" + location;
    if(discordLauncher) discordLauncher->SendExeMessage(message);
    graphicsAPI->SetWindowTitle(GetWindowName(location).c_str());
}

bool PulseEngineBackend::IsRunning()
{
    return !graphicsAPI->ShouldClose();
}

void PulseEngineBackend::PollEvents()
{
    graphicsAPI->PollEvents();
}

void PulseEngineBackend::Update()
{
    float currentFrame = PulseEngineGraphicsAPI->GetTime();
    inputSystem->newFrame();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    view = GetActiveCamera()->GetViewMatrix();
    projection = PulseEngine::MathUtils::PerspectiveMat(PulseEngine::MathUtils::ToRadians(GetActiveCamera()->Zoom), static_cast<float>(width) / static_cast<float>(height), 0.1f, 1000.0f);
    mapLoading -= deltaTime;
    for (auto& light : lights) 
    {
        light->RecalculateLightSpaceMatrix();
    }

    for (size_t i = 0; i < entities.size(); ++i)
    {
        Entity* entityA = entities[i];
        entityA->UpdateEntity(deltaTime);
        Collider* col = dynamic_cast<Collider*>(entityA->collider);
        if(col) col->othersCollider.clear();
        for (size_t j = i + 1; j < entities.size(); ++j)
        {
            Entity* entityB = entities[j];
            CollisionManager::ManageCollision(dynamic_cast<Collider*>(entityA->collider), dynamic_cast<Collider*>(entityB->collider));                
        }
    }
    

    coroutineManager->UpdateAll(deltaTime);
}

void PulseEngineBackend::Render()
{
    graphicsAPI->StartFrame();

    for (Entity* entity : entities)
    {
        if (!IsRenderable(entity)) continue;
        Shader* shader = entity->GetMaterial()->GetShader();

        shader->Use();
        shader->SetMat4("projection", projection);
        shader->SetMat4("view", view);
        shader->SetVec3("viewPos", GetActiveCamera()->Position);

        LightManager::BindLightsToShader(shader, this, entity);
        // for (size_t i = 0; i < lights.size(); ++i)
        // {
        //     DirectionalLight* dLight = dynamic_cast<DirectionalLight*>(lights[i]);
        //     if (!dLight)
        //         continue;
        
        //     dLight->BindToShader(*shader, -1);
        
        //     if (dLight->castsShadow)
        //     {            PulseEngineGraphicsAPI->ActivateTexture(0);
        //         PulseEngineGraphicsAPI->BindTexture(TEXTURE_2D, dLight->depthMapTex);
            
        //         shader->SetInt("dirLight.shadowMap", 0);
        //     }
        
        //     break; // Only one directional light supported
        // }
        entity->DrawEntity();
    }

        // Draw grid quad in editor only
    EDITOR_ONLY(
        DrawGridQuad(view, projection);
    )

if (lights.size() > 0)
{
    DirectionalLight* dLight = dynamic_cast<DirectionalLight*>(lights[0]);
    if (dLight && dLight->castsShadow)
    {
        // Static VAO/VBO pour ne créer qu'une seule fois
        static unsigned int quadVAO = 0, quadVBO = 0, quadEBO = 0;
        if (quadVAO == 0)
        {
            float quadVertices[] = {
                // positions   // texCoords
               -0.9f, -0.9f,  0.0f, 0.0f,
               -0.5f, -0.9f,  1.0f, 0.0f,
               -0.5f, -0.5f,  1.0f, 1.0f,
               -0.9f, -0.5f,  0.0f, 1.0f
            };
            unsigned int quadIndices[] = { 0, 1, 2, 2, 3, 0 };

            glGenVertexArrays(1, &quadVAO);
            glGenBuffers(1, &quadVBO);
            glGenBuffers(1, &quadEBO);

            glBindVertexArray(quadVAO);

            glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadEBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);

            // position
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);
            // texCoords
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
            glEnableVertexAttribArray(1);

            glBindVertexArray(0);
        }

        // Draw quad
        debugShader->Use();
        PulseEngineGraphicsAPI->ActivateTexture(10);
        PulseEngineGraphicsAPI->BindTexture(TEXTURE_2D, dLight->depthMapTex);
        debugShader->SetInt("depthMap", 10);

    glBindVertexArray(quadVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    }
}

    graphicsAPI->EndFrame();
}

// Draws a large quad at y=0 using a grid shader
void PulseEngineBackend::DrawGridQuad(PulseEngine::Mat4 viewCam,const PulseEngine::Mat4& specificProjection )
{
    #ifdef PULSE_GRAPHIC_OPENGL
    static unsigned int quadVAO = 0, quadVBO = 0;
    if (quadVAO == 0) {
        float yOffset = -0.001f; // Slightly below y=0 to avoid z-fighting
        float quadVertices[] = {
            // positions
            -100.0f, yOffset, -100.0f,
             100.0f, yOffset, -100.0f,
             100.0f, yOffset,  100.0f,
            -100.0f, yOffset,  100.0f
        };
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    }

    // Load or use your grid shader
    static Shader* gridShader = nullptr;
    if (!gridShader) {
        gridShader = new Shader(
            std::string(ASSET_PATH) + "shaders/Grid.vert",
            std::string(ASSET_PATH) + "shaders/Grid.frag"
        );
    }

    gridShader->Use();
    gridShader->SetMat4("model", PulseEngine::Mat4(1.0f));
    gridShader->SetMat4("view", viewCam);
    gridShader->SetMat4("projection", specificProjection);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_DEPTH_TEST);     
    glDepthMask(GL_FALSE);       

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);

    glDepthMask(GL_TRUE);       
    glDisable(GL_BLEND);
    #endif
}



void PulseEngineBackend::SpecificRender(Camera *cam, int specificVBO, std::vector<Entity*> entitiesToRender, PulseEngine::Vector2 viewportSize,Shader* specificShader)
{
    if (!cam) return;

    // Calculate view and projection matrices for the specific camera
    PulseEngine::Mat4 specificView = cam->GetViewMatrix();
    float aspectRatio = static_cast<float>(viewportSize.x) / static_cast<float>(viewportSize.y);
    PulseEngine::Mat4 specificProjection = PulseEngine::MathUtils::PerspectiveMat(
        PulseEngine::MathUtils::ToRadians(cam->Zoom),
        aspectRatio,
        0.1f,
        1000.0f
    );
    PulseEngineGraphicsAPI->SpecificStartFrame(specificVBO, viewportSize);
    

    for (Entity* entity : entitiesToRender)
    {
        if (!IsRenderable(entity)) continue;
        Shader* shader = specificShader ? specificShader : entity->GetMaterial()->GetShader();

        shader->Use();
        shader->SetMat4("view", specificView);
        shader->SetMat4("projection", specificProjection);
        shader->SetVec3("viewPos", cam->Position);

        LightManager::BindLightsToShader(shader, this, entity);

        if(!specificShader) entity->DrawEntity();
        else
        {
            entity->BindTexturesToShader();
            shader->SetMat4("model", entity->GetMatrix());
            entity->DrawMeshWithShader(shader->getProgramID());
        } 
    }
        // Draw grid quad in editor only
    EDITOR_ONLY(
        DrawGridQuad(specificView, specificProjection);
    )

    graphicsAPI->EndFrame();
}




void PulseEngineBackend::RenderShadow()
{    
    shadowShader->Use();
    for (int i = 0; i < lights.size(); ++i)
    {
        
        lights[i]->RenderShadowMap(*shadowShader, *this);
    }
}

void PulseEngineBackend::Shutdown()
{    
    graphicsAPI->ShutdownApi();
    discordLauncher->Terminate();
}

void PulseEngineBackend::ClearScene()
{
    for(auto ent : entities)
    {
        delete ent;
    }
    entities.clear();
}
void PulseEngineBackend::DeleteEntity(Entity *entity)
{
    auto it = std::find(entities.begin(), entities.end(), entity);
    if (it != entities.end())
    {
        delete *it;
        entities.erase(it);
    }
}

void PulseEngineBackend::ProcessInput(GLFWwindow* window)
{
    // if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) activeCamera->ProcessKeyboard(FORWARD, deltaTime);
    // if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) activeCamera->ProcessKeyboard(BACKWARD, deltaTime);
    // if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) activeCamera->ProcessKeyboard(LEFT, deltaTime);
    // if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) activeCamera->ProcessKeyboard(RIGHT, deltaTime);
}

// glm::vec3 PulseEngineBackend::CalculateLighting(const glm::vec3 &position, const glm::vec3 &normal, const glm::vec3 &viewPos, const LightData& light)
// {
// #ifndef PULSE_GRAPHIC_OPENGL
//     // Calculate the direction from the object to the light
//     glm::vec3 lightDir = glm::normalize(glm::vec3(light.GetPosition().x, light.GetPosition().y, light.GetPosition().z) - position);

//     // Diffuse lighting: Lambertian reflectance (dot product of normal and light direction)
//     float diff = glm::max(glm::dot(normal, lightDir), 0.0f);

//     // Specular lighting: Blinn-Phong reflection model
//     glm::vec3 reflectDir = glm::reflect(-lightDir, normal);
//     glm::vec3 viewDir = glm::normalize(viewPos - position);
//     float spec = pow(glm::max(glm::dot(viewDir, reflectDir), 0.0f), 32); // 32 is the shininess

//     // Apply attenuation (optional for point lights)
//     float distance = glm::length(glm::vec3(light.GetPosition().x, light.GetPosition().y, light.GetPosition().z) - position);
//     float attenuation = 1.0f / (1.0f + light.attenuation * distance * distance);

//     // Final color calculation (ambient + diffuse + specular)
//     glm::vec3 ambientColor = glm::vec3(light.GetColor().r, light.GetColor().g, light.GetColor().b);
//     glm::vec3 ambient = 0.1f * ambientColor; // Ambient light
//     glm::vec3 diffuse = diff * ambientColor * light.intensity;
//     glm::vec3 specular = spec * ambientColor * light.intensity;

//     // Combine all lighting effects, considering attenuation for point lights
//     glm::vec3 finalColor = (ambient + diffuse + specular) * attenuation;
    
//     return finalColor;
//     #endif
//     return  glm::vec3(1.0f, 1.0f, 1.0f); // Default color if not in export mode
// }

bool PulseEngineBackend::IsRenderable(Entity *entity) const
{
    return entity != nullptr && entity->GetMaterial() != nullptr && entity->GetMaterial()->GetShader() != nullptr;
}

PulseEngineBackend* PulseEngineBackend::GetInstance()
{
    if (!instance)
    {
        instance = new PulseEngineBackend();
    }
    return instance;
}


PulseEngine::Vector3 PulseEngineBackend::GetCameraPosition()
{
    return GetActiveCamera()->Position;
}

PulseEngine::Vector3 PulseEngineBackend::GetCameraRotation()
{
    return PulseEngine::Vector3(GetActiveCamera()->Pitch, GetActiveCamera()->Yaw, 0.0f);
}

