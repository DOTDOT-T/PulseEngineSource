@echo off
echo === Compilation of heavy headers (and the one that didnt change a lot) into a PCH (precompiled headers) ===
g++ -x c++-header src/pch.h ^
-Iinclude ^
-Iinclude/imgui ^
-o src/pch.h.gch
