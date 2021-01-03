# append all files with extension "ext" in the "dirs" directories to "ret"
# excludes all files starting with a '.' (dot)
macro (acg_append_files ret ext)
    foreach (_dir ${ARGN})
        file (GLOB _files "${_dir}/${ext}")
        foreach (_file ${_files})
            get_filename_component (_filename ${_file} NAME)
            if (_filename MATCHES "^[.]")
	            list (REMOVE_ITEM _files ${_file})
            endif ()
        endforeach ()
        list (APPEND ${ret} ${_files})
    endforeach ()
endmacro ()

# ---------- Project ----------

project(xengine)

set(CMAKE_CXX_STANDARD 17)

# Locate root dir
SET(ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}")

SET(DIRS
    "${ROOT_DIR}"
    "${ROOT_DIR}/geometry"
    "${ROOT_DIR}/graphics"
    "${ROOT_DIR}/mesh"
    "${ROOT_DIR}/model"
    "${ROOT_DIR}/scene"
    "${ROOT_DIR}/ui"
    "${ROOT_DIR}/utility"
)

# collect all header and source files
acg_append_files (HEADERS "*.h" "${DIRS}")
acg_append_files (SOURCES "*.cpp" "${DIRS}")

add_library(${PROJECT_NAME} STATIC ${SOURCES} ${HEADERS})

include_directories("${ROOT_DIR}")
include_directories("${CMAKE_SOURCE_DIR}/3rdparty")

# ---------- Binary libraries ----------

# glfw
find_package(glfw REQUIRED HINTS "${CMAKE_SOURCE_DIR}/3rdparty/glfw")
target_link_libraries(${PROJECT_NAME} "${GLFW_LIBS}")
include_directories("${GLFW_INCLUDE_DIRS}")

# assimp
find_package(assimp REQUIRED HINTS "${CMAKE_SOURCE_DIR}/3rdparty/assimp")
target_link_libraries(${PROJECT_NAME} "${ASSIMP_LIBS}")
include_directories("${ASSIMP_INCLUDE_DIRS}")

# ---------- Header libraries ----------

# glm
find_package(glm REQUIRED HINTS "${CMAKE_SOURCE_DIR}/3rdparty/glm")
include_directories(${GLM_INCLUDE_DIRS})

# stb image
find_package(stb REQUIRED HINTS "${CMAKE_SOURCE_DIR}/3rdparty/stb")
include_directories(${STB_INCLUDE_DIRS})

# ---------- Source libraries ----------

# glad
find_package(glad REQUIRED HINTS "${CMAKE_SOURCE_DIR}/3rdparty/glad")
add_dependencies(${PROJECT_NAME} glad)
target_link_libraries(${PROJECT_NAME} glad)
include_directories(${GLAD_INCLUDE_DIRS})

# imgui
find_package(imgui REQUIRED HINTS "${CMAKE_SOURCE_DIR}/3rdparty/imgui")
add_dependencies(${PROJECT_NAME} imgui)
target_link_libraries(${PROJECT_NAME} imgui)
include_directories(${IMGUI_INCLUDE_DIRS})

# ---------- Export settings ----------

SET(XE_INCLUDE_DIRS "${ROOT_DIR}")
MARK_AS_ADVANCED(XE_INCLUDE_DIRS)

# ---------- Preprocessing flags ----------

if (WIN32)
    add_definitions()
endif ()