@echo off
echo === Generating folders for Window ===
if not exist Build mkdir Build
if not exist "Build/assets" mkdir "Build/assets"
if not exist "Build/Logs" mkdir "Build/Logs"
