/**
 * @file TopBar.h
 * @author Dorian LEXTERIAQUE (dlexteriaque@gmail.com)
 * @brief The topbar of the editor interface, it contains the menu bar and the main functions of the editor.
 * @todo the internal function of the topbar (for compilation) are wrotten in the cpp, but should be moved to a "BuildSystem.h" and "BuildSystem.cpp" files.
 * @note The topbar is not part of the interface module, but it is used by the interface editor to provide a menu bar and some main functions.
 * @note The topbar is not part of the PulseEngineBackend, but it is used by the interface editor to provide a menu bar and some main functions.
 * @version 0.1
 * @date 2025-07-04
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef TOPBAR_H
#define TOPBAR_H

#include "pch.h"

#include "Common/common.h"
#include "Common/dllExport.h"

class PulseEngineBackend;

class PULSE_ENGINE_DLL_API TopBar
{
private:

public:
    friend class InterfaceEditor;
    TopBar() {}
    ~TopBar() {}

    void UpdateBar(PulseEngineBackend *engine, InterfaceEditor *editor);
    void ImportMesh(InterfaceEditor *editor, std::string &name, std::string &guidPath, char filePath[260], std::string &meshPath);
    void DeletePrefix(std::string &fileStr, std::string &prefix, std::string &guidPath);
    void BuildGameToWindow(PulseEngineBackend *engine, InterfaceEditor *editor);
    void CompileUserScripts(InterfaceEditor *editor, std::string output = "Build/CustomScripts.dll");
    void AnalyzeEntry(const std::filesystem::directory_entry &entry, std::string &sources);
    void GenerateExecutableForWindow(PulseEngineBackend *engine);
    void CopyDllForWindow();
    void CopyAssetForWindow();
    void GenerateWindowsDirectory();
};


#endif