# Variables : SOURCE_DIR, TARGET_DIR
file(COPY ${SOURCE_DIR} DESTINATION ${TARGET_DIR})

# Supprimer tous les .cpp dans TARGET_DIR
file(GLOB_RECURSE CPP_FILES "${TARGET_DIR}/*.cpp")
foreach(f ${CPP_FILES})
    file(REMOVE ${f})
endforeach()
