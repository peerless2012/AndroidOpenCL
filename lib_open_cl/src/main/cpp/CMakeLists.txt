cmake_minimum_required(VERSION 3.22)

project(lib_open_cl)

add_subdirectory(${PROJECT_SOURCE_DIR}/../../../../contrib/OpenCL-Headers OpenCLHeaders)

option(BUILD_SHARED_LIBS "Build OpenCL Shared Library." ON)
add_subdirectory(${PROJECT_SOURCE_DIR}/../../../../contrib/OpenCL-ICD-Loader OpenCLICDLoader)