@echo off
echo === Copying assets For window engine ===
xcopy PulseEngineEditor "PulseEngineBuild/PulseEngineEditor" /E /I /Y
if %errorlevel% neq 0 exit /b %errorlevel%
echo Assets copied.