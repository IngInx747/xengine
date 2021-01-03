project(demo)

# Locate root dir
SET(ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}")

# collect all header and source files
file(GLOB SRCS "*.h" "*.cpp")

add_executable(${PROJECT_NAME} ${SRCS})

add_dependencies(${PROJECT_NAME} xengine)

target_link_libraries(${PROJECT_NAME} xengine)

include_directories("${ROOT_DIR}")
include_directories("${CMAKE_SOURCE_DIR}")
include_directories("${CMAKE_SOURCE_DIR}/xengine")
include_directories("${CMAKE_SOURCE_DIR}/3rdparty")