#include "BuildGameCoroutine.h"
#include "PulseEngine/core/PulseEngineBackend.h"
#include "PulseEngineEditor/InterfaceEditor/InterfaceEditor.h"
#include "PulseEngineEditor/InterfaceEditor/TopBar.h"

bool BuildGameCoroutine::Update(float deltaTime)
    {

        timer += deltaTime;
        EDITOR_ONLY(
        switch (currentStep)
        {
            case Init:
                EDITOR_LOG("Starting build...\n");
                editor->AddLoadingPopup([]() {
                ImGui::Text("Building game for window...");
                }, 0.0f, "Building Game");
                currentStep = Step1;
                timer = 0;
                break;

            case Step1:
                if (timer >= 1.0f)
                {
                    editor->ChangePorgressIn("Building Game", 0.1f);
                    editor->ChangeProgressContent([]() {
                        ImGui::Text("Generating directories...");
                    }, "Building Game");
                    topbar->GenerateWindowsDirectory();
                    currentStep = Step2;
                    timer = 0;
                }
                break;

            case Step2:
                if (timer >= 0.5f)
                {
                    editor->ChangePorgressIn("Building Game", 0.4f);
                    editor->ChangeProgressContent([]() {
                        ImGui::Text("Copying assets...");
                    }, "Building Game");
                    topbar->CopyAssetForWindow();
                    EDITOR_LOG("Step 2 done\n");
                    currentStep = Step3;
                    timer = 0;
                }
                break;

            case Step3:
                if (timer >= 0.5f)
                {
                    editor->ChangePorgressIn("Building Game", 0.6f);
                    editor->ChangeProgressContent([]() {
                        ImGui::Text("Copying engine DLL...");
                    }, "Building Game");
                    topbar->CopyDllForWindow();
                    EDITOR_LOG("Step 3 done\n");
                    currentStep = Step4;
                    timer = 0;
                }
                break;

            case Step4:
                if (timer >= 0.5f)
                {
                    // editor->ChangePorgressIn("Building Game", 0.8f);
                    // editor->ChangeProgressContent([]() {
                    //     ImGui::Text("Generating executable for window...");
                    // }, "Building Game");
                    // topbar->GenerateExecutableForWindow(engine);
                    // EDITOR_LOG("Step 4 done\n");
                    currentStep = Compile;
                    timer = 0;
                }
                break;

            case Compile:
                
                editor->ChangeProgressContent([]() {
                ImGui::Text("Compiling user scripts...");
                }, "Building Game");
                topbar->CompileUserScripts(editor);
                // system("g++ ..."); // Do actual work
                currentStep = Done;
                break;

            case Done:
                EDITOR_LOG("Build Done!\n");
                return false;
        })

        return true;
    }