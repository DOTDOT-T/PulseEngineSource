cd external
cd assimp

cmake -S . -B build -G "Visual Studio 17 2022" ` -DASSIMP_BUILD_FBX_IMPORTER=ON ` -DBUILD_SHARED_LIBS=ON ` -DASSIMP_NO_EXPORT=OFF ` -DASSIMP_BUILD_TESTS=OFF
cmake --build build --config Release -- /m
cd ../../


mkdir build
mkdir build/editor
cmake -B build/editor -S . -D ENABLE_ENGINE_EDITOR=ON 
cmake --build build/editor --config Debug