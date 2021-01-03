# Locate root dir
SET(ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}")

FILE(GLOB SOURCES
    "${ROOT_DIR}/imgui.cpp"
    "${ROOT_DIR}/imgui_demo.cpp"
    "${ROOT_DIR}/imgui_draw.cpp"
    "${ROOT_DIR}/imgui_widgets.cpp"
    "${ROOT_DIR}/imgui_impl_glfw.cpp"
    "${ROOT_DIR}/imgui_impl_opengl3.cpp")
    
FILE(GLOB HEADERS
    "${ROOT_DIR}/imconfig.h"
    "${ROOT_DIR}/imgui.h"
    "${ROOT_DIR}/imgui_internal.h"
    "${ROOT_DIR}/imstb_rectpack.h"
    "${ROOT_DIR}/imstb_textedit.h"
    "${ROOT_DIR}/imstb_truetype.h"
    "${ROOT_DIR}/imgui_impl_glfw.h"
    "${ROOT_DIR}/imgui_impl_opengl3.h")
    
# Handle the QUIETLY and REQUIRED arguments and set XXX_FOUND to TRUE if all listed variables are TRUE.
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(
	imgui DEFAULT_MSG
	ROOT_DIR
	SOURCES
	HEADERS)

SET(IMGUI_SOURCES ${SOURCES})
SET(IMGUI_HEADERS ${HEADERS})
SET(IMGUI_INCLUDE_DIRS ${ROOT_DIR})

MARK_AS_ADVANCED(
    IMGUI_SOURCES
    IMGUI_HEADERS
    IMGUI_INCLUDE_DIRS)

# To use imgui sources directly, enable scripts below
IF (${IMGUI_EMBED_SOURCE})
    find_package(imgui REQUIRED HINTS "${CMAKE_SOURCE_DIR}/3rdparty")
    list(APPEND SRCS ${IMGUI_SOURCES})
    include_directories(${IMGUI_INCLUDE_DIRS})
    add_executable(${PROJECT_NAME} ${SRCS})
ENDIF (${IMGUI_EMBED_SOURCE})