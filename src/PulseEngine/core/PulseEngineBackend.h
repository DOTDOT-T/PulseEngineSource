
/**
 * @file PulseEngineBackend.h
 * @author Dorian LEXTERIAQUE (dlexteriaque@gmail.com)
 * @brief Where all the magic happens, the main class of the Pulse Engine.
 * @brief This class is responsible for initializing the engine, managing the main loop, and rendering the scene.
 * @brief It also handles the window context, input processing, and scene management.
 * @version 0.1
 * @date 2025-06-28
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef PULSEENGINE_H
#define PULSEENGINE_H

#include <string>
// #include "Common/common.h"
#include "Common/dllExport.h"
#include "json.hpp"

#include "PulseEngine/core/Input/InputSystem.h"
#include "PulseEngine/core/Math/Vector.h"
#include "PulseEngine/core/Math/Mat4.h"

#define GUID_COLLECTION_PATH std::string(ASSET_PATH) + "EngineConfig/Guid/"
#define DEFAULT_SHADOW_MAP_RES 2048

class WindowContext;
class Entity;
class LightData;
class Camera;
class InterfaceEditor;
class LightManager;
class CoroutineManager;
class GLFWwindow;
class Shader;
class GuidCollection;
class PulseExecutable;
class IGraphicsAPI;
class Account;
class PulseScriptsManager;
class Gamemode;

class PhysicManager;

/**
 * @brief PulseEngineBackend is the main class of the Pulse Engine.
 * 
 */
class PULSE_ENGINE_DLL_API PulseEngineBackend
{
public:
    friend InterfaceEditor;
    friend LightManager;


    static PulseEngineBackend* GetInstance();
    ~PulseEngineBackend() {}

    void LoadModel(std::string path);

    // === main functions Engine ===
    int Initialize(); 
    bool IsRunning(); 
    void PollEvents(); 
    void Update(); 
    void Render(); 
    void SpecificRender(Camera* cam, int specificVBO, std::vector<Entity*> entitiesToRender, 
        PulseEngine::Vector2 viewportSize = PulseEngine::Vector2(800,600),
        Shader* specificShader = nullptr );
    void RenderShadow(); 
    void Shutdown(); 
    // Editor grid quad rendering
    void DrawGridQuad(PulseEngine::Mat4 viewCam,const PulseEngine::Mat4& specificProjection  );
    void ClearScene();
    void DeleteEntity(Entity* entity);

    // === getters ===
    WindowContext* GetWindowContext() { return windowContext; }

    // === static functions ===
    static Camera* GetActiveCamera() {return activeCamera;}
    static void SetActiveCamera(Camera* camera) { activeCamera = camera; }


    std::vector<Entity*> entities;
    std::vector<LightData*> lights;

    std::string actualMapPath = "Scenes/defaultMap.pmap";
    std::string actualMapName = "defaultMap.pmap";

    std::string GetWindowName(const std::string& location) ;
    void SetWindowName(const std::string& location);

    // === getters ===
    const std::string& GetEngineName() const { return engine; }
    const std::string& GetEngineVersion() const { return version; }
    const std::string& GetDevMonth() const { return devMonth; }
    const std::string& GetCompanyName() const { return company; }

    const void SetGameName(const std::string& name) { gameName = name; }
    const std::string& GetGameName() const { return gameName; }

    const void SetGameVersion(const std::string& version) { gameVersion = version; }
    const std::string& GetGameVersion() const { return gameVersion; }

    const float GetDeltaTime() {return deltaTime;}
    PulseEngine::Vector3 GetCameraPosition();
    PulseEngine::Vector3 GetCameraRotation();

    Account* GetAccountLoggedIn() {return account; }
    Gamemode* GetGamemode();


    static IGraphicsAPI* graphicsAPI;
    CoroutineManager* coroutineManager = nullptr;

    // #ifdef ENGINE_EDITOR
        static InterfaceEditor* editor;
    // #endif
    
    Shader* shadowShader;
    Shader* pointLightShadowShader;
    Shader* debugShader;

    std::unordered_map<std::string, GuidCollection*> guidCollections;
    PulseLibs::InputSystem* inputSystem;

    PulseEngine::Mat4 view;
    PulseEngine::Mat4 projection;
    PulseEngine::Mat4 lastView;
    PulseEngine::Mat4 lastProjection;
    
private:
    WindowContext* windowContext = nullptr;

    int width = 1920;
    int height = 1080;
    std::string gameName = "NULL";
    std::string gameVersion = "0.0.1";

    std::string engine = "Pulse Engine";
    std::string version = "V0.3.0";
    std::string devMonth = "December 2025";
    std::string company = "Pulse Software";

    static float deltaTime;
    float lastFrame = 0.0f;
    float mapLoading = 0.0f;

    static Camera* activeCamera;

    Gamemode* gamemode = nullptr;
    nlohmann::json_abi_v3_12_0::json engineConfig;

    PulseExecutable* discordLauncher = nullptr; 

    Account* account;
    PhysicManager* physicManager = nullptr;


    void ProcessInput(GLFWwindow* window);
    // glm::vec3 CalculateLighting(const glm::vec3& position, const glm::vec3& normal, const glm::vec3& viewPos, const LightData& light);
    bool IsRenderable(Entity* entity) const;
    PulseEngineBackend();
    PulseEngineBackend(const PulseEngineBackend&) = delete; // Disable copy constructor
    PulseEngineBackend& operator=(const PulseEngineBackend&) = delete; // Disable assignment operator
    PulseEngineBackend(PulseEngineBackend&&) = delete; // Disable move constructor

    static PulseEngineBackend* instance;

    PulseScriptsManager* runtimeScript = nullptr;
};

#endif
