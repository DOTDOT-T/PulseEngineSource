@echo off
setlocal enabledelayedexpansion

:: Répertoire source et output
set SRC=BuilderScript/ObjBuilder/src
set OUT=ObjectFiles

:: Inclues communes
set INCLUDES=-I. -Iinclude -Iinclude/websocketpp -Iinclude/asio -Iinclude/imgui -Iinclude/imgui-node -I./src -IE:/Editor/Include -Iexternal/assimp/include

:: Crée le répertoire de sortie s'il n'existe pas
if not exist %OUT% mkdir %OUT%

echo === Compilation des .cpp en .o ===

:: Parcours récursif de tous les .cpp
for /r %SRC% %%f in (*.cpp) do (
    set "SRCFILE=%%f"
    set "OBJFILE=%OUT%\%%~nf.o"

    REM crée le répertoire parent si besoin
    for %%d in ("!OBJFILE!") do (
        mkdir "%%~dpd" >nul 2>nul
    )

    echo Compilation de %%f ...
    g++ -c "%%f" %INCLUDES% -o "!OBJFILE!" -DBUILDING_DLL -DENGINE_EDITOR -DPULSE_GRAPHIC_OPENGL -DPULSE_WINDOWS -DIMGUI_IMPL_OPENGL_LOADER_GLAD
)

echo === Compilation terminée ===
