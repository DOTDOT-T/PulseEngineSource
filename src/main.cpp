

#include "Common/common.h"

#include "shader.h"
#include "camera.h"
#include "PulseEngine/core/PulseEngineBackend.h"
#include "PulseEngine/core/Meshes/Mesh.h"
#include "PulseEngine/core/Entity/Entity.h"
#include "PulseEngine/core/Material/MaterialManager.h"

#include "PulseEngine/core/Material/Material.h"
#include "PulseEngine/core/Meshes/primitive/Primitive.h"
#include "PulseEngine/CustomScripts/IScripts.h"
#include "PulseEngine/core/FileManager/FileManager.h"
#include "PulseEngine/core/Graphics/IGraphicsApi.h"

#ifdef ENGINE_EDITOR
#include "PulseEngineEditor/InterfaceEditor/InterfaceEditor.h"
#endif

#include <vector>
#include <cmath>
#include <cstdlib>
#include <string>
#include <iostream>
#include <map>
#include <filesystem>

using namespace PulseEngine::FileSystem;
namespace fs = std::filesystem;

int main(int argc, char **argv)
{
    std::string workingDir;

    if (argc > 1)
    {
#ifdef ENGINE_EDITOR
        workingDir = argv[1];
#else
        workingDir = fs::current_path().string();
#endif
    }
    else
    {
        workingDir = std::filesystem::current_path().string();
    }

    EDITOR_INFO("Engine dir : " << std::filesystem::current_path())
    // Définit le répertoire courant
    std::filesystem::current_path(workingDir);
    FileManager::workingDirectory = workingDir;

    EDITOR_INFO("Project dir : " << std::filesystem::current_path())

    PulseEngineBackend *engine = PulseEngineBackend::GetInstance();

// during the compilation of the game, some datas are defined in the preprocessor.
// here, we get them and use them with the engine. (the dll didnt have them, so we need to set them manually)
#ifdef GAME_NAME
    std::cout << std::string(GAME_NAME) << std::endl;
    engine->SetGameName(GAME_NAME);
#endif
#ifdef GAME_VERSION
    std::cout << std::string(GAME_VERSION) << std::endl;
    engine->SetGameVersion(GAME_VERSION);
#endif
    if (engine->Initialize() != 0)
    {
        EDITOR_ERROR("Engine failed to initialize")
        while (true)
        {
        }
        return -1;
    }

    if (engine->graphicsAPI == nullptr)
    {
        EDITOR_ERROR("Graphics API failed to initialize")
        while (true)
        {
        }
        return -1;
    }

#ifdef ENGINE_EDITOR
    InterfaceEditor *editor = new InterfaceEditor();
    engine->editor = editor;
    editor->InitAfterEngine();
#endif

    // === Boucle de rendu ===
    while (engine->IsRunning())
    {
        engine->PollEvents();
        engine->Update();

        engine->RenderShadow();
        engine->Render();

#ifdef ENGINE_EDITOR
        editor->Render();
#endif

        engine->graphicsAPI->SwapBuffers();
        engine->graphicsAPI->PollEvents();
    }
    engine->Shutdown();

#ifdef ENGINE_EDITOR
    delete editor;
#endif

    while (true)
    {
    }

    return 0;
}
