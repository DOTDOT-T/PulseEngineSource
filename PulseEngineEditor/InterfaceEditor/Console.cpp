#include "Console.h"

bool Console::scrollBot = false;

void Console::Render()
{
ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoDecoration;
    
    ImGui::Begin("Console Log", nullptr);
    ImGui::BeginChild("ScrollingRegion", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar);
    
    for (const auto& line : GetMessages())
    {
        ImVec4 color =  line.find("[ERROR]") != std::string::npos ? ImVec4(1.0,0.0,0.0,1.0) : 
                        line.find("[INFO]") != std::string::npos ? ImVec4(0.0, 0.5, 1.0, 1.0) : 
                        line.find("[SUCCESS]") != std::string::npos ? ImVec4(0.0, 1.0, 0.5, 1.0) : 
                        line.find("[WARN]") != std::string::npos ? ImVec4(1.0,1.0,0.0,1.0) : ImVec4(1.0,1.0,1.0, 1.0);
        ImGui::PushStyleColor(ImGuiCol_Text, color);
        ImGui::TextUnformatted(line.c_str());
        ImGui::PopStyleColor();
    }
    
    if(scrollBot)
    {
        scrollBot = false;
        ImGui::SetScrollHereY(1.0f);
    }
    
    ImGui::EndChild();
    ImGui::End();
}
