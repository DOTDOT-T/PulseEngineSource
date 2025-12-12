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
#include "PulseEngine/core/PulseScript/PulseScriptsManager.h"
#include "PulseEngine/core/Graphics/TextRenderer.h"
#include "PulseEngine/core/Gamemode/Gamemode.h"

#include "PulseEngine/core/Math/MathUtils.h"

#include "PulseEngine/core/Network/Request/Request.h"

#ifdef PULSE_GRAPHIC_OPENGL
#include "PulseEngine/core/Graphics/OpenGLAPI/OpenGLApi.h"
#endif

#include "PulseEngine/core/PulseObject/TypeRegister/TypeRegister.h"
#include "PulseEngine/core/SceneManager/SceneManager.h"
#include "Common/dllExport.h"
#include "PulseEngine/core/PulseScript/utilities.h"

#include "PulseEngine/core/PulseScript/NativeInit.h"
#include "PulseEngine/core/FileManager/Archive/Archive.h"
#include "PulseEngine/core/FileManager/Archive/DiskArchive.h"
#include "PulseEngine/core/Physics/PhysicManager.h"

using namespace PulseEngine::FileSystem;
using namespace PulseLibs;

Camera* PulseEngineBackend::activeCamera = new Camera();
IGraphicsAPI* PulseEngineBackend::graphicsAPI = nullptr;
PulseEngineBackend* PulseEngineBackend::instance = nullptr;
float PulseEngineBackend::deltaTime = 0.0f;

// #ifdef ENGINE_EDITOR
InterfaceEditor* PulseEngineBackend::editor = nullptr;
// #endif

PulseEngineBackend::PulseEngineBackend() 
{ 
    account = new Account("dlexteriaque@gmail.com", "passwordTest");
}

int PulseEngineBackend::Initialize()
{
    PROFILE_TIMER_FUNCTION;
    EDITOR_INFO("Started the initialization of the engine.");
    EDITOR_INFO("Engine version: " + version);
    EDITOR_INFO("Development month: " + devMonth);
    EDITOR_INFO("Project name : " + gameName);
    EDITOR_INFO("Pulse Software © 2025");

    windowContext = new WindowContext();
    activeCamera = new Camera();

    graphicsAPI = new OpenGLAPI();

    if(graphicsAPI == nullptr)
    {
        EDITOR_ERROR("No Graphics API found.");
        return -1;
    }
    graphicsAPI->InitializeApi(GetWindowName("editor").c_str(), &width, &height);
    
    #ifdef PULSE_GRAPHIC_OPENGL
    windowContext->SetGLFWWindow(static_cast<GLFWwindow*>(graphicsAPI->GetNativeHandle()));
    #endif


    coroutineManager = new CoroutineManager;
    inputSystem = new PulseLibs::InputSystem;
    physicManager = new PhysicManager();
    physicManager->InitializePhysicSystem();

    shadowShader = new Shader(std::string(ASSET_PATH) + "EngineConfig/shaders/directionalDepth/dirDepth.vert", std::string(ASSET_PATH) + "EngineConfig/shaders/directionalDepth/dirDepth.frag", graphicsAPI);
    pointLightShadowShader = new Shader(std::string(ASSET_PATH) + "EngineConfig/shaders/pointDepth/pointDepth.vert", std::string(ASSET_PATH) + "EngineConfig/shaders/pointDepth/pointDepth.frag", std::string(ASSET_PATH) + "EngineConfig/shaders/pointDepth/pointDepth.glsl", graphicsAPI);
    debugShader = new Shader(std::string(ASSET_PATH) +"EngineConfig/shaders/debug.vert", std::string(ASSET_PATH) + "EngineConfig/shaders/debug.frag", graphicsAPI);

    // === initialize each collection found in the asset folder ===
    std::vector<std::filesystem::path> guidFiles = FileManager::GetFilesInDirectoryWithExtension(std::string(ASSET_PATH) + "EngineConfig/Guid", ".puid");
    for (const auto& file : guidFiles) 
    {
        EDITOR_LOG("Guid/" + file.filename().string());
        guidCollections[file.filename().string()] = new GuidCollection("Guid/" + file.filename().string());
    }
    
    
    EDITOR_ONLY(
        ScriptsLoader::LoadDLL();
    )

 
    engineConfig = FileManager::OpenEngineConfigFile();

    std::string firstScene = engineConfig["GameData"]["FirstScene"];
    SceneLoader::LoadScene(firstScene, this);

    if(engineConfig["Engine"]["discord"].get<bool>())
    {
        discordLauncher = new PulseExecutable ("DiscordPresence/DiscordPresence.exe", "DiscordPipeTest");
        Sleep(1500);
        discordLauncher->SendExeMessage("[set_presence]In the editor");
    }

    runtimeScript = new PulseScriptsManager();


    gamemode = new Gamemode();
    DiskArchive gmdar("enginegm.gamemode", Archive::Mode::Loading);
    if(gmdar.IsArchiveOpen()) gamemode->Serialize(gmdar);
    else EDITOR_WARN("Gamemode couldn't be loaded from [enginegm.gamemode]")
    InitNativeMethods();

    EDITOR_INFO("Finished the initialization of the engine.");
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
    PROFILE_TIMER_FUNCTION;
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

    SceneManager::GetInstance()->UpdateScene();
    

    coroutineManager->UpdateAll(deltaTime);

    std::vector<Variable> args;
    Variable deltaVar;
    deltaVar.isGlobal = false;
    deltaVar.name = "deltatime";
    deltaVar.value = GetDeltaTime();
    args.push_back(deltaVar);

    gamemode->Update();
    LateUpdate();
}

void PulseEngineBackend::LateUpdate()
{
    #ifndef ENGINE_EDITOR
    physicManager->UpdatePhysicSystem(deltaTime);
    #endif    
}

void PulseEngineBackend::Render()
{
    PROFILE_TIMER_FUNCTION;
    graphicsAPI->StartFrame();

    SceneManager::GetInstance()->RenderScene();
    gamemode->Render();

    // for (Entity* entity : entities)
    // {
    //     if (!IsRenderable(entity)) continue;
    //     Shader* shader = entity->GetMaterial()->GetShader();
    //     shader->Use();
    //     shader->SetMat4("projection", projection);
    //     shader->SetMat4("view", view);
    //     shader->SetVec3("viewPos", GetActiveCamera()->Position);
    //     LightManager::BindLightsToShader(shader, this, entity);
    //      for (size_t i = 0; i < lights.size(); ++i)
    //      {
    //          DirectionalLight* dLight = dynamic_cast<DirectionalLight*>(lights[i]);
    //          if (!dLight)
    //              continue;  
    //          dLight->BindToShader(*shader, -1);   
    //          if (dLight->castsShadow)
    //          {            PulseEngineGraphicsAPI->ActivateTexture(0);
    //              PulseEngineGraphicsAPI->BindTexture(TEXTURE_2D, dLight->depthMapTex);          
    //              shader->SetInt("dirLight.shadowMap", 0);
    //          }  
    //          break; // Only one directional light supported
    //      }
    //      entity->DrawEntity();
    //     // SceneManager::GetInstance()->RenderScene();
    // }

        // Draw grid quad in editor only
#ifdef ENGINE_EDITOR
        DrawGridQuad(view, projection);
#endif

    graphicsAPI->EndFrame(true);
}

// Draws a large quad at y=0 using a grid shader
void PulseEngineBackend::DrawGridQuad(PulseEngine::Mat4 viewCam,const PulseEngine::Mat4& specificProjection )
{
    graphicsAPI->DrawGridQuad(viewCam, specificProjection, graphicsAPI);
}



void PulseEngineBackend::SpecificRender(Camera *cam, int specificVBO, std::vector<Entity*> entitiesToRender, PulseEngine::Vector2 viewportSize,Shader* specificShader)
{
    PROFILE_TIMER_FUNCTION;
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
    
    static ITextRenderer* copyrightText = nullptr;
    if(!copyrightText)
    {
        copyrightText = PulseEngineInstance->graphicsAPI->CreateNewText();
        copyrightText->Init();
    }
    copyrightText->SetScreenSize(viewportSize.x, viewportSize.y);

    copyrightText->RenderText("Pulse Engine-" + version, 0, 25, 25.0f, PulseEngine::Vector3(0.0f, 0.0f, 0.0f));
    copyrightText->Render();

    for (Entity* entity : entitiesToRender)
    {
        if (!IsRenderable(entity)) continue;
        Shader* shader = specificShader ? specificShader : entity->GetMaterial()->GetShader();

        shader->Use();
        shader->SetMat4("view", specificView);
        shader->SetMat4("projection", specificProjection);
        shader->SetVec3("viewPos", cam->Position);

        lastView = specificView;
        lastProjection = specificProjection;

        LightManager::BindLightsToShader(shader, this, entity);

        if(!specificShader) {
            entity->DrawEntity();
            if (entity->collider)
            {
                entity->collider->lineTraceShader->Use();
                entity->collider->lineTraceShader->SetMat4("view", specificView);
                entity->collider->lineTraceShader->SetMat4("projection", specificProjection);
                entity->collider->OnRender();
            }
        }
        else
        {
            entity->BindTexturesToShader();
            shader->SetMat4("model", entity->GetMatrix());
            entity->DrawMeshWithShader(shader);
        } 
    }
        // Draw grid quad in editor only
#ifdef ENGINE_EDITOR
        DrawGridQuad(specificView, specificProjection);
#endif

    gamemode->Render();

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
    if(discordLauncher) discordLauncher->Terminate();

    physicManager->ShutdownPhysicSystem();
}

void PulseEngineBackend::ClearScene()
{
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

Gamemode *PulseEngineBackend::GetGamemode()
{
    return gamemode;
}
