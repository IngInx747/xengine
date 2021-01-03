# CMakeLists of libraries to build as static libs.
# The libs are used by multiple executables and other libs.

# glad
find_package(glad REQUIRED HINTS "${CMAKE_SOURCE_DIR}/3rdparty/glad")
add_library(glad STATIC ${GLAD_SOURCES} ${GLAD_HEADERS})
include_directories(${GLAD_INCLUDE_DIRS})

# imgui (requires: glad, glfw)
find_package(imgui REQUIRED HINTS "${CMAKE_SOURCE_DIR}/3rdparty/imgui")
find_package(glfw REQUIRED HINTS "${CMAKE_SOURCE_DIR}/3rdparty/glfw")
add_library(imgui STATIC ${IMGUI_SOURCES} ${IMGUI_HEADERS})
add_dependencies(imgui glad)
include_directories("${IMGUI_INCLUDE_DIRS}")
include_directories("${GLAD_INCLUDE_DIRS}")
include_directories("${GLFW_INCLUDE_DIRS}")
target_link_libraries(imgui "${GLFW_LIBS}")

# preprocessing
if (WIN32)
    add_definitions()
endif ()