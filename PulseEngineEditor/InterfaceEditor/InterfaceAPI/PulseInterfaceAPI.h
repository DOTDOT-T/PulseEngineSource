/**
 * @file PulseInterfaceAPI.h
 * @author Dorian LEXTERIAQUE (dlexteriaque@gmail.com)
 * @brief API of the interface of the editor. give all the methods to easily make module for the engine.
 * @version 0.1
 * @date 2025-07-18
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef PULSEINTERFACEAPI_H
#define PULSEINTERFACEAPI_H

#include "Common/common.h"
#include "Common/dllExport.h"

class Entity;
class Mesh;
class Material;
class IScript;
class IModuleInterface;
class RenderableMesh;

enum EditorWidgetComponent
{
    TEXT,
    BUTTON,
    SELECTABLE,
    MENU_ITEM,
    SEPARATOR,
    SPACE,

    EDITOR_WIDGET__COMP_COUNT
};

struct OutputContextMenuType
{
    bool isClicked = false;
};

struct ContextMenuItem
{
    std::string label;
    std::function<void()> onClick;
    EditorWidgetComponent type;
    nlohmann::json style;

    OutputContextMenuType output;
};



/**
 * @brief All the style needed to modify (push/pop) style for the element of the UI of the editor.
 * 
 */
enum class PulseEngineStyle
{
    Text,
    TextDisabled,
    WindowBg,
    ChildBg,
    PopupBg,
    Border,
    BorderShadow,
    FrameBg,
    FrameBgHovered,
    FrameBgActive,
    TitleBg,
    TitleBgActive,
    TitleBgCollapsed,
    MenuBarBg,
    ScrollbarBg,
    ScrollbarGrab,
    ScrollbarGrabHovered,
    ScrollbarGrabActive,
    CheckMark,
    SliderGrab,
    SliderGrabActive,
    Button,
    ButtonHovered,
    ButtonActive,
    Header,
    HeaderHovered,
    HeaderActive,
    Separator,
    SeparatorHovered,
    SeparatorActive,
    ResizeGrip,
    ResizeGripHovered,
    ResizeGripActive,
    InputTextCursor,
    TabHovered,
    Tab,
    TabSelected,
    TabSelectedOverline,
    TabDimmed,
    TabDimmedSelected,
    TabDimmedSelectedOverline,
    DockingPreview,
    DockingEmptyBg,
    PlotLines,
    PlotLinesHovered,
    PlotHistogram,
    PlotHistogramHovered,
    TableHeaderBg,
    TableBorderStrong,
    TableBorderLight,
    TableRowBg,
    TableRowBgAlt,
    TextLink,
    TextSelectedBg,
    TreeLines,
    DragDropTarget,
    NavCursor,
    NavWindowingHighlight,
    NavWindowingDimBg,
    ModalWindowDimBg,

    COUNT
};


class PULSE_ENGINE_DLL_API PulseInterfaceAPI
{
    private:

public:
/**
 * @brief Inform the engine that its the beginning of a window. this didnt tell to opent the window, 
 * it tells the engine to generate a new window from this point up to CloseWindow()
 * 
 * @param name Name of the window to show on top.
 */
    static void OpenWindow(const std::string& name);
    /**
     * @brief All content between open and close will be shown inside of the window. After the close, nothing can be in the window.
     * 
     */
    static void CloseWindow();

    /**
     * @brief write a simple text in line.
     * 
     * @param text text to write.
     */
    static void WriteText(const std::string& text);

    /**
     * @brief display an image on the window from a OpenGL texture ID.
     * 
     * @param user_texture_id id saved from an opengl importation
     * @param image_size size of the image wanted on the window.
     * @param uv0 start of the uv (mostly 0,0)
     * @param uv1 end of the uv (mostly 1,1)
     */
    static void Image(int user_texture_id, const PulseEngine::Vector2& image_size, const PulseEngine::Vector2& uv0, const PulseEngine::Vector2& uv1);

    /**
     * @brief Add space for a certain number of  "line".
     * 
     * @param amount amount of line to space.
     */
    static void AddSpace(int amount = 1);

    /**
     * @brief Create a button with a specific name and size.
     * 
     * @param name name of the button
     * @param size size in the relative window space
     * @return true The button has been clicked this frame
     * @return false the button has not been clicked this frame
     */
    static bool Button(const std::string& name, const PulseEngine::Vector2& size);

    /**
     * @brief Add a separator in line/column on the window size.
     * 
     */
    static void Separator();

    /**
     * @brief Make the next element on the same line has the last one.
     * 
     */
    static void SameLine();

    static void PushStyleColor(PulseEngineStyle syle, const PulseEngine::Vector3& color, const float& alpha = 1.0f);
    static void PopStyleColor(int amount = 1);

    /**
     * @brief Renders the scene from a specific camera and displays it in an interface window.
     * @param camera The camera to render from.
     * @param windowName The name of the ImGui window.
     * @param imageSize The size to display the image.
     */
    static void RenderCameraToInterface(PulseEngine::Vector2* previewData, Camera* camera, const std::string& windowName, const PulseEngine::Vector2& imageSize, std::vector<Entity*> entitiesToRender, Shader* shader = nullptr);

    /**
     * @brief Get the Selected Entity object
     * 
     * @return Entity* 
     */
    static Entity* GetSelectedEntity();

    /**
     * @brief Returns true if the current ImGui window is focused.
     * Call this between OpenWindow/Begin and CloseWindow/End.
     */
    static bool IsCurrentWindowFocused();

    /**
     * @brief Get the current mouse position in the interface window.
     * 
     * @return PulseEngine::Vector2 
     */
    static PulseEngine::Vector2 MouseIn();
    
    /**
     * @brief Get the current mouse delta in the interface window.
     * for more detail : the delta is the difference between the last frame and the current frame mouse position.
     * it is useful to know how much the mouse has moved since the last frame.
     * 
     * @return PulseEngine::Vector2 
     */
    static PulseEngine::Vector2 MouseDelta();

    /**
     * @brief Permit an easy way to add callback when a files is clicked in the file explorer.
     * 
     * @param callback the function to call
     */
    static void AddFunctionToFileClickedCallbacks(std::function<void(const ClickedFileData&)> callback);

    /**
     * @brief Begin a child window with a specific name, size, border, and flags.
     * 
     * @param name The name of the child window.
     * @param size The size of the child window.
     * @param border Whether to show a border.
     * @param flags Additional flags for the child window.
     */
    static void BeginChild(const std::string& name, const PulseEngine::Vector2& size, bool border = false);

    /**
     * @brief End the current child window.
     * 
     */
    static void EndChild();

    /**
     * @brief Begin a combo box with a specific label and preview value.
     * 
     * @param label The label for the combo box.
     * @param previewValue The preview value displayed in the combo box.
     * @return true If the combo box is open.
     * @return false If the combo box is closed.
     */
    static bool BeginCombo(const std::string& label, const std::string& previewValue);

    /**
     * @brief end the actual combo
     * 
     */
    static void EndCombo();

    /**
     * @brief Add a selectable item to the current combo box.
     * 
     * @param label The label for the selectable item.
     * @param selected Whether the item is selected.
     * @param size The size of the item.
     * @return true If the item was clicked.
     * @return false If the item was not clicked.
     */
    static bool Selectable(const std::string& label, bool selected = false, const PulseEngine::Vector2& size = PulseEngine::Vector2(0, 0));

    static void AddTransformModifier(Entity* entity, const std::string& modifierName);
    static void AddTransformModifierForMesh(RenderableMesh* mesh, const std::string& modifierName);

    static bool StartTreeNode(const std::string &name, bool *open);
    static void EndTreeNode();

    static void AddMaterialPreview(Material *&material, const PulseEngine::Vector2 &imageSize, const std::string &name = "Material Preview");

    static void MaterialPicker(Material *&material);

    static PulseEngine::Vector2 GetActualWindowSize();

    static bool DragFloat(const char* label, float* value, float speed = 1.0f, float minVal = 0.0f, float maxVal = 0.0f, const char* format = "%.3f");

    static bool DragFloat3(const char* label, float* values, float speed = 1.0f, float minVal = 0.0f, float maxVal = 0.0f, const char* format = "%.3f");

    static void ChangeWindowState(IModuleInterface* script, bool state);

    static bool ShowContextMenu(const char* popupId, const std::vector<ContextMenuItem>& items);
    static void OpenContextMenu(const char* popupId);

    static void ComponentBasedOnEnum(ContextMenuItem &item);

    static void OpenTable(const std::string& name, int columns);
    static void DeclareTableColumn(const std::string& name);
    static void NextTableColumn();
    static void DrawTableHeadersRow();
    static void SetTableColumnIndex(int columnN);
    static void EndTable();

};

#endif