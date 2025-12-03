mkdir build
mkdir build/engine
cmake -B build/engine -S . -D ENABLE_ENGINE_EDITOR=OFF
cmake --build build/engine --config Debug