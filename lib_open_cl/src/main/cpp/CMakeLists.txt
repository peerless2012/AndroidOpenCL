cmake_minimum_required(VERSION 3.22)

project(lib_open_cl)

option(BUILD_SHARED_LIBS "This is a option for address" ON)
add_subdirectory(${PROJECT_SOURCE_DIR}/../../../../contrib/OpenCL-Headers OpenCLHeaders)

add_subdirectory(${PROJECT_SOURCE_DIR}/../../../../contrib/OpenCL-ICD-Loader OpenCLICDLoader)

add_subdirectory(OpenCL4J)