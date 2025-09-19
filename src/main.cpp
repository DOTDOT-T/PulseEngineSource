

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
#include <string>
#include <iostream>
#include <map>



int main(int argc, char** argv)
{
    std::string workingDir;

    if (argc > 1) 
    {
        // Le premier argument après l'exe est le répertoire de travail
        #ifdef ENGINE_EDITOR
        workingDir = argv[1];
        #else
        workingDir = std::filesystem::current_path().string();
        #endif
    }
    else 
    {
        workingDir = std::filesystem::current_path().string();
    }

    // Définit le répertoire courant
    std::filesystem::current_path(workingDir);
    FileManager::workingDirectory = workingDir;

    std::cout << "The actual working directory is: " << std::filesystem::current_path() << std::endl;

    PulseEngineBackend* engine = PulseEngineBackend::GetInstance();
    



    //during the compilation of the game, some datas are defined in the preprocessor.
    //here, we get them and use them with the engine. (the dll didnt have them, so we need to set them manually)
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
        std::cerr << "Engine failed to initialize" << std::endl;
        while(true) {}
        return -1;
    }

    if(engine->graphicsAPI == nullptr)
    {
        std::cerr << "Graphics API failed to initialize" << std::endl;
        while(true) {}
        return -1;
    }

    
    EDITOR_ONLY(
        InterfaceEditor* editor = new InterfaceEditor();
        engine->editor = editor;
        editor->InitAfterEngine();
    )
    

    // === Boucle de rendu ===
    while (engine->IsRunning())
    {
        engine->PollEvents();
        engine->Update();

        engine->RenderShadow();        
        engine->Render();
        
        EDITOR_ONLY(
            editor->Render();
        )

        engine->graphicsAPI->SwapBuffers();
        engine->graphicsAPI->PollEvents();
    }
    engine->Shutdown();

    EDITOR_ONLY(
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    )  
    delete engine;
    #ifdef ENGINE_EDITOR
        delete editor;
    #endif

    while(true) {}

    return 0;
}
