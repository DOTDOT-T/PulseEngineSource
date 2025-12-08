#include "MaterialEditor.h"
#include "PulseEngineEditor/InterfaceEditor/InterfaceAPI/PulseInterfaceAPI.h"
#include "PulseEngine/core/Entity/Entity.h"
#include "PulseEngine/core/Math/MathUtils.h"
#include "PulseEngine/core/GUID/GuidReader.h"
#include "PulseEngine/core/Material/MaterialManager.h"
#include "PulseEngine/CustomScripts/IScripts.h"
#include "PulseEngine/CustomScripts/ScriptsLoader.h"
#include "PulseEngine/core/Meshes/Mesh.h"
#include "PulseEngine/core/Material/Material.h"
#include "PulseEngine/core/GUID/GuidGenerator.h"
#include "PulseEngine/core/Meshes/primitive/Primitive.h"
#include "PulseEngine/core/FileManager/FileReader/FileReader.h"
#include "Shader.h"
#include "camera.h"
#include <windows.h>
#include "json.hpp"

std::string MaterialEditor::GetName() const
{
    return std::string("Material Editor");
}

std::string MaterialEditor::GetVersion() const
{
    return std::string("v0.0.1");
}

void MaterialEditor::Initialize()
{
    EDITOR_LOG("init of material interface")
    PulseInterfaceAPI::AddFunctionToFileClickedCallbacks(
        [this](const ClickedFileData &data)
        { this->NewFileClicked(data); });
    
    if(!cam) cam = new Camera();
    if(!materialRenderer) materialRenderer = new Entity("MaterialRenderer", PulseEngine::Vector3(0.0f, 0.0f, 0.0f), Primitive::Sphere(), materialSelected);
    if(!forRender) forRender = new Shader("PulseEngineEditor/EngineConfig/shaders/basic.vert", "PulseEngineEditor/EngineConfig/shaders/basic.frag", PulseEngineInstance->graphicsAPI);
    entities = { materialRenderer };
    EDITOR_LOG("end of material interface init")
}

void MaterialEditor::Shutdown()
{
    if (cam) {
        delete cam;
        cam = nullptr;
    }
    if (materialRenderer) {
        delete materialRenderer;
        materialRenderer = nullptr;
    }
    if (forRender) {
        delete forRender;
        forRender = nullptr;
    }
}

void MaterialEditor::Render()
{
    std::string matName = materialSelected ? "- " + materialSelected->GetName() : "";

    ImGui::Begin(("Material Editor " + matName + "###MaterialEditor").c_str());

    // -------- VIEWPORT (left) --------
    ImVec2 windowSize = ImGui::GetContentRegionAvail();
    ImVec2 viewportSize = ImVec2(windowSize.x * 0.5f, windowSize.y);

    ImGui::BeginChild("MaterialViewport", viewportSize, true);

    if (!materialSelected)
    {
        // Empty grid + center card
        ImDrawList* dl = ImGui::GetWindowDrawList();
        ImVec2 pos = ImGui::GetCursorScreenPos();
        ImVec2 size = ImGui::GetContentRegionAvail();

        const float GRID_SIZE = 32.0f;
        const ImU32 GRID_COL = IM_COL32(40, 40, 42, 255);
        const ImU32 BG_COL   = IM_COL32(28, 28, 29, 255);

        dl->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y), BG_COL);

        for (float x = fmodf(pos.x, GRID_SIZE); x < pos.x + size.x; x += GRID_SIZE)
            dl->AddLine(ImVec2(x, pos.y), ImVec2(x, pos.y + size.y), GRID_COL, 1.0f);
        for (float y = fmodf(pos.y, GRID_SIZE); y < pos.y + size.y; y += GRID_SIZE)
            dl->AddLine(ImVec2(pos.x, y), ImVec2(pos.x + size.x, y), GRID_COL, 1.0f);

        // Center card
        const ImVec2 cardSize = ImVec2(380, 160);
        ImVec2 cardPos = ImVec2(pos.x + size.x * 0.5f - cardSize.x * 0.5f,
                                pos.y + size.y * 0.4f - cardSize.y * 0.5f);

        const ImU32 CardBG      = IM_COL32(46, 46, 48, 230);
        const ImU32 CardBorder  = IM_COL32(70, 70, 72, 255);
        const ImVec4 TitleColor = ImVec4(0.92f, 0.92f, 0.94f, 1.0f);
        const ImVec4 TextColor  = ImVec4(0.72f, 0.72f, 0.76f, 1.0f);

        dl->AddRectFilled(cardPos, ImVec2(cardPos.x + cardSize.x, cardPos.y + cardSize.y), CardBG, 6.0f);
        dl->AddRect(cardPos, ImVec2(cardPos.x + cardSize.x, cardPos.y + cardSize.y), CardBorder, 6.0f, 0, 1.5f);

        ImGui::SetCursorScreenPos(cardPos);
        ImGui::BeginChild("NoMaterialCard", cardSize, false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground);

        // Title
        ImGui::SetCursorPosY(20);
        ImGui::PushStyleColor(ImGuiCol_Text, TitleColor);
        ImGui::SetWindowFontScale(1.4f);
        ImGui::SetCursorPosX((cardSize.x - ImGui::CalcTextSize("No Material Selected").x) * 0.5f);
        ImGui::TextUnformatted("No Material Selected");
        ImGui::PopStyleColor();

        // Subtitle
        ImGui::SetWindowFontScale(1.0f);
        ImGui::Dummy(ImVec2(0, 10));
        ImGui::PushStyleColor(ImGuiCol_Text, TextColor);
        const char* msg = "Select a material in the Content Browser\nor create a new material to start editing.";
        ImVec2 textSize = ImGui::CalcTextSize(msg);
        ImGui::SetCursorPosX((cardSize.x - textSize.x) * 0.5f);
        ImGui::TextUnformatted(msg);
        ImGui::PopStyleColor();

        ImGui::EndChild();
    }
    else
    {
        // Render material preview with camera
        ManageCamera();
        PulseInterfaceAPI::RenderCameraToInterface(&previewData, cam, "Material Editor", PulseEngine::Vector2(viewportSize.x, viewportSize.y), entities, forRender);
    }

    ImGui::EndChild(); // viewport

    ImGui::SameLine();

    // -------- PROPERTY PANEL (right) --------
    ImGui::BeginChild("MaterialProperties", ImVec2(-1.0f, windowSize.y), true);

    ImGui::Text("Material Properties");
    ImGui::Separator();

    if (materialSelected)
    {
        ImGui::SliderFloat("Specular", &materialSelected->specular, 0.0f, 1.0f);

       ImGui::Checkbox("Has Y Flip", materialSelected->GetFlipPtr());

        float color[3] = { materialSelected->color.x, materialSelected->color.y, materialSelected->color.z };
        if (ImGui::ColorEdit3("Color", color))
        {
            materialSelected->color = PulseEngine::Vector3(color[0], color[1], color[2]);
        }

        std::vector<std::string> textureTypes = { "albedo", "normal", "roughness" };
        for (const auto& type : textureTypes)
            TextureSelector(type);

        if (ImGui::Button("Save Material", ImVec2(-1.0f, 0.0f)))
        {
            nlohmann::json materialData;
            materialData["flip"] = materialSelected->HasYFlip();
            materialData["name"] = materialSelected->GetName();
            materialData["guid"] = materialSelected->guid;
            materialData["specular"] = materialSelected->specular;
            materialData["color"] = { materialSelected->color.x, materialSelected->color.y, materialSelected->color.z };

            for (const auto& tex : materialSelected->GetAllTextures())
                if (tex.second && !tex.second->GetPath().empty())
                    materialData[tex.first] = tex.second->GetPath();

            std::ofstream file(std::string(ASSET_PATH) + materialSelected->GetPath());
            if (file.is_open())
            {
                file << materialData.dump(4);
                file.close();
            }
        }
    }

    ImGui::EndChild(); // properties

    ImGui::End(); // Material Editor window
}

void MaterialEditor::NotSelectedDesign()
{
    // -------- MAIN GRID VIEWPORT --------
    ImVec2 screenPos = ImGui::GetCursorScreenPos();
    ImVec2 region    = ImGui::GetContentRegionAvail();
    ImDrawList* dl   = ImGui::GetWindowDrawList();

    const float GRID_SIZE = 32.0f;
    const ImU32 GRID_COL  = IM_COL32(40, 40, 42, 255);  // UE-ish gray
    const ImU32 BG_COL    = IM_COL32(28, 28, 29, 255);  // Material graph dark background

    // Background fill
    dl->AddRectFilled(screenPos, ImVec2(screenPos.x + region.x, screenPos.y + region.y), BG_COL);

    // Grid rendering (small + large UE-like spacing)
    for (float x = fmodf(screenPos.x, GRID_SIZE); x < screenPos.x + region.x; x += GRID_SIZE)
        dl->AddLine(ImVec2(x, screenPos.y), ImVec2(x, screenPos.y + region.y), GRID_COL, 1.0f);

    for (float y = fmodf(screenPos.y, GRID_SIZE); y < screenPos.y + region.y; y += GRID_SIZE)
        dl->AddLine(ImVec2(screenPos.x, y), ImVec2(screenPos.x + region.x, y), GRID_COL, 1.0f);

    // -------- CENTER CARD (Unreal-ish) --------
    const ImVec2 cardSize  = ImVec2(380, 160);
    ImVec2 cardPos = ImVec2(
        screenPos.x + region.x * 0.5f - cardSize.x * 0.5f,
        screenPos.y + region.y * 0.4f - cardSize.y * 0.5f
    );

    const ImU32 CardBG      = IM_COL32(46, 46, 48, 230);  // translucent dark gray
    const ImU32 CardBorder  = IM_COL32(70, 70, 72, 255);
    const ImVec4 TitleColor = ImVec4(0.92f, 0.92f, 0.94f, 1.0f);
    const ImVec4 TextColor  = ImVec4(0.72f, 0.72f, 0.76f, 1.0f);

    // Background + border
    dl->AddRectFilled(cardPos, ImVec2(cardPos.x + cardSize.x, cardPos.y + cardSize.y), CardBG, 6.0f);
    dl->AddRect(cardPos, ImVec2(cardPos.x + cardSize.x, cardPos.y + cardSize.y), CardBorder, 6.0f, 0, 1.5f);

    // BeginChild for centered text and layout
    ImGui::SetCursorScreenPos(cardPos);
    ImGui::BeginChild("NoMaterialCard", cardSize, false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground);

    // Title
    ImGui::SetCursorPosY(20);
    ImGui::PushStyleColor(ImGuiCol_Text, TitleColor);
    ImGui::SetWindowFontScale(1.4f);
    ImGui::SetCursorPosX((cardSize.x - ImGui::CalcTextSize("No Material Selected").x) * 0.5f);
    ImGui::TextUnformatted("No Material Selected");
    ImGui::PopStyleColor();

    ImGui::SetWindowFontScale(1.0f);

    // Subtitle
    ImGui::Dummy(ImVec2(0, 10));
    ImGui::PushStyleColor(ImGuiCol_Text, TextColor);
    const char* msg = "Select a material in the Content Browser\nor create a new material to start editing.";
    ImVec2 textSize = ImGui::CalcTextSize(msg);
    ImGui::SetCursorPosX((cardSize.x - textSize.x) * 0.5f);
    ImGui::TextUnformatted(msg);
    ImGui::PopStyleColor();

    ImGui::EndChild();

    // Stop normal editor
    PulseInterfaceAPI::CloseWindow();
}
void MaterialEditor::TextureSelector(const std::string &textureName)
{
    PulseInterfaceAPI::BeginChild("Texture Selector#xx" + textureName, PulseEngine::Vector2(0.0f, 160.0f), true);
    if (PulseInterfaceAPI::BeginCombo(textureName + " Texture", materialSelected->GetTexture(textureName) ? materialSelected->GetTexture(textureName)->GetPath() : "None"))
    {
        for (const auto pr : GuidReader::GetAllAvailableFiles("guidCollectionTextures.puid"))
        {
            if (PulseInterfaceAPI::Selectable(pr.second, true))
            {
                auto albedoTexture = std::make_shared<Texture>(pr.second, PulseEngineInstance->graphicsAPI);
                if (albedoTexture)
                {
                    materialSelected->SetTexture(textureName, albedoTexture);
                }
            }
        }
        PulseInterfaceAPI::EndCombo();
    }
    else if (materialSelected->GetTexture(textureName))
    {
        PulseInterfaceAPI::Image(materialSelected->GetTexture(textureName)->id, PulseEngine::Vector2(52.0f, 52.0f),
                                 PulseEngine::Vector2(0.0f, 0.0f), PulseEngine::Vector2(1.0f, 1.0f));
    }
    PulseInterfaceAPI::EndChild();
}
void MaterialEditor::ManageCamera()
{
    if (PulseInterfaceAPI::IsCurrentWindowFocused())
    {
        if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
        {
            distCam += PulseInterfaceAPI::MouseDelta().y * 0.1f;
        }
        if ((GetAsyncKeyState(VK_RBUTTON) & 0x8000))
        {
            cam->Yaw += PulseInterfaceAPI::MouseDelta().x * 0.1f;
            cam->Pitch -= PulseInterfaceAPI::MouseDelta().y * 0.1f;
            if (cam->Pitch > 89.0f)
                cam->Pitch = 89.0f;
            if (cam->Pitch < -89.0f)
                cam->Pitch = -89.0f;

            cam->Position = PulseEngine::MathUtils::RotateAround(materialRenderer->GetPosition(), cam->Yaw, cam->Pitch, distCam);
            cam->Front = (materialRenderer->GetPosition() - cam->Position).Normalized();
        }
    }
}
void MaterialEditor::NewFileClicked(const ClickedFileData &data)
{
    std::string fullPath = data.name.string();
    // Remove "PulseEngineEditor/" from fullPath if present
    std::string prefix = "PulseEngineEditor\\";
    if (fullPath.rfind(prefix, 0) == 0)
    {
        fullPath = fullPath.substr(prefix.length());
    }
    prefix = "PulseEngineEditor/";
    if (fullPath.rfind(prefix, 0) == 0)
    {
        fullPath = fullPath.substr(prefix.length());
    }

    if (fullPath.size() >= 4 && fullPath.substr(fullPath.size() - 4) == ".mat")
    {
        if (PulseInterfaceAPI::Selectable("Material Editor"))
        {
            PulseInterfaceAPI::ChangeWindowState(this, true);
            materialSelected = MaterialManager::loadMaterial(fullPath);
            materialRenderer->SetMaterial(materialSelected);
            std::cout << "new material selected " << fullPath << std::endl;
        }
    }
}