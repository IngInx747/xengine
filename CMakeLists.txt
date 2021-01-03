  # cmake_minimum_require must be the first command of the file
cmake_minimum_required(VERSION 3.5.0)

# This is your project statement. You should always list languages;
# Listing the version is nice here since it sets lots of useful variables
project(XEngine VERSION 1.0 LANGUAGES CXX)

# we add the sub-directories that we want CMake to scan
add_subdirectory(3rdparty)
add_subdirectory(xengine)
add_subdirectory(demo)