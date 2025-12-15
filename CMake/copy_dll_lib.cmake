file(GLOB DLL_FILES "${TARGET_DIR}/*.dll")
foreach(f ${DLL_FILES})
    file(COPY ${f} DESTINATION ${TARGET_DIR}/dist)
endforeach()

file(GLOB LIB_FILES "${TARGET_DIR}/*.lib")
foreach(f ${LIB_FILES})
    file(COPY ${f} DESTINATION ${TARGET_DIR}/dist)
endforeach()
