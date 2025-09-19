@echo off
echo === Copying assets For window ===
xcopy PulseEngineEditor "Build/assets" /E /I /Y
if %errorlevel% neq 0 exit /b %errorlevel%
echo Assets copied.