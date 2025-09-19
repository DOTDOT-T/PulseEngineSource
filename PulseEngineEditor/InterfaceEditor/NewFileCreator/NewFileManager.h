#ifndef NEWFILEMANAGER_H
#define NEWFILEMANAGER_H

#include "Common/common.h"
#include "Common/dllExport.h"
#include <unordered_map>
#include <string>
#include <filesystem>

struct PULSE_ENGINE_DLL_API CategoryFiles
{
    std::string categoryName;
    std::vector<std::pair<std::string, std::string>> fileTypes; // Pair of (Display Name, Extension)

    CategoryFiles(std::string name, std::vector<std::pair<std::string, std::string>> types)
        : categoryName(std::move(name)), fileTypes(std::move(types)) {}
};

class PULSE_ENGINE_DLL_API NewFileManager
{
    public:
        NewFileManager();
        void OpenPopup();
        void RenderPopup(const std::filesystem::path &currentDir, std::filesystem::path &selectedFile);
        void RenderFilesTypeColumn(const std::vector<std::pair<std::string, std::string>> &fileTypes);
        void RenderCategories();
        void GenerateCppAndHeaderFiles(const std::filesystem::path &currentDir, std::string &fileNameStr, std::filesystem::path &selectedFile);

    private:
        std::vector<CategoryFiles> categories;
        int selectedCategory = 0;
        int selectedFileType = 0;
        char newFileName[128] = "";
};


#endif