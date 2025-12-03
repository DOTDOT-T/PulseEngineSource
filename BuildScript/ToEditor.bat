mkdir build
mkdir build/editor
cmake -B build/editor -S . -D ENABLE_ENGINE_EDITOR=ON 
cmake --build build/editor --config Debug