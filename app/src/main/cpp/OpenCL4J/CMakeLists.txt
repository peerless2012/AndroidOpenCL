cmake_minimum_required(VERSION 3.22)

project(OpenCL4J)

string(TOUPPER ${CMAKE_BUILD_TYPE} DEFINE_BUILD_TYPE)

file(GLOB INC include/*.h)
file(GLOB SRC src/*.cpp)
file(GLOB JNI jni/*.*)

add_library(${PROJECT_NAME} SHARED ${INC} ${SRC} ${JNI})
target_compile_definitions(${PROJECT_NAME} PRIVATE CL_TARGET_OPENCL_VERSION=200)
target_compile_definitions(${PROJECT_NAME} PRIVATE OPENCL4J_${DEFINE_BUILD_TYPE} GL_GLEXT_PROTOTYPES EGL_EGLEXT_PROTOTYPES)
target_include_directories(${PROJECT_NAME} PUBLIC include)
find_package(lib_open_cl REQUIRED CONFIG)
target_link_libraries(${PROJECT_NAME} PRIVATE log lib_open_cl::OpenCL GLESv2 EGL)