/**
 * @Author peerless2012
 * @Email peerless2012@126.com
 * @DateTime 2023/3/3 16:57
 * @Version V1.0
 * @Description 
 */

#include "OpenCL4J.h"
#include "CL/cl_gl.h"
#include "CL/cl_gl_ext.h"
#include "CL/cl_egl.h"
#include <string>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include "GLES2/gl2.h"
#include "GLES2/gl2ext.h"
#include "GLES3/gl3.h"
#include "EGL/egl.h"
#include "EGL/eglext.h"

#define printf ALOGV

static const char * source =
         "__kernel void rgba_to_gray(__read_only image2d_t input, __write_only image2d_t output)\n"
         "{\n"
         "    int2 coord = (int2)(get_global_id(0), get_global_id(1));\n"
         "    float4 pixel = read_imagef(input, CLK_NORMALIZED_COORDS_FLOAT, coord);\n"
         "    float gray = dot(pixel.rgb, (float3)(0.299, 0.587, 0.114));\n"
         "    write_imagef(output, coord, (float4)(gray, gray, gray, pixel.a));\n"
         "}";

void printOpenCLInfo() {

    int i, j;
    char* value;
    size_t valueSize;
    cl_uint platformCount;
    cl_platform_id* platforms;
    cl_uint deviceCount;
    cl_device_id* devices;
    cl_uint maxComputeUnits;
    cl_uint maxClockFreq;
    unsigned int maxWorkItemDimension;
    size_t maxWorkItemSize[3];
    size_t maxWorkGroupSize;
    cl_uint devAddressBit;
    cl_uint memBaseAddressAlign;
    cl_ulong maxMemAllocSize;
    cl_ulong gloMemSize;
    cl_ulong maxConstBuffSize;
    cl_ulong gloMemCacheSize;
    cl_uint gloMemCacheLineSize;
    cl_ulong localMemSize;
    size_t profTimerRes;
    cl_bool imageSupport;
    cl_bool errorCorrSupport;
    cl_bool hostUnifiedMem;
    cl_uint preVecWidthInt;
    cl_uint preVecWidthLong;
    cl_uint preVecWidthFloat;
    cl_uint preVecWidthDouble;
    cl_uint nativeVecWidthInt;
    cl_uint nativeVecWidthLong;
    cl_uint nativeVecWidthFloat;
    cl_uint nativeVecWidthDouble;

    // get all platforms
    clGetPlatformIDs(0, NULL, &platformCount);
    platforms = (cl_platform_id*)malloc(sizeof(cl_platform_id)* platformCount);
    clGetPlatformIDs(platformCount, platforms, NULL);

    printf("There is a total of %d OpenCL platforms.\n", platformCount);

    for (i = 0; i < platformCount; i++) {

        printf("%d. OpenCL Platform [%d]:\n", i + 1, i + 1);

        clGetPlatformInfo(platforms[i], CL_PLATFORM_NAME, 0, NULL, &valueSize);
        value = (char*)malloc(valueSize);
        clGetPlatformInfo(platforms[i], CL_PLATFORM_NAME, valueSize, value, NULL);
        printf("   CL_PLATFORM_NAME: %s\n", value);
        free(value);

        clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, 0, NULL, &deviceCount);
        printf("   Total Device Count: %d\n", deviceCount);

        // get all GPU devices
        clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_GPU, 0, NULL, &deviceCount);
        devices = (cl_device_id*)malloc(sizeof(cl_device_id)* deviceCount);
        clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_GPU, deviceCount, devices, NULL);

        // for each device print critical attributes
        for (j = 0; j < deviceCount; j++) {

            printf("   GPU Device [%d]: \n", j+1);

            // print device name
            clGetDeviceInfo(devices[j], CL_DEVICE_NAME, 0, NULL, &valueSize);
            value = (char*)malloc(valueSize);
            clGetDeviceInfo(devices[j], CL_DEVICE_NAME, valueSize, value, NULL);
            printf("    CL_DEVICE_NAME: %s\n", value);
            free(value);

            // print device vendor
            clGetDeviceInfo(devices[j], CL_DEVICE_VENDOR, 0, NULL, &valueSize);
            value = (char*)malloc(valueSize);
            clGetDeviceInfo(devices[j], CL_DEVICE_VENDOR, valueSize, value, NULL);
            printf("    CL_DEVICE_VENDOR: %s\n", value);
            free(value);

            // print device profile
            clGetDeviceInfo(devices[j], CL_DEVICE_PROFILE, 0, NULL, &valueSize);
            value = (char*)malloc(valueSize);
            clGetDeviceInfo(devices[j], CL_DEVICE_PROFILE, valueSize, value, NULL);
            printf("    CL_DEVICE_PROFILE: %s\n", value);
            free(value);

            // print device version
            clGetDeviceInfo(devices[j], CL_DEVICE_VERSION, 0, NULL, &valueSize);
            value = (char*)malloc(valueSize);
            clGetDeviceInfo(devices[j], CL_DEVICE_VERSION, valueSize, value, NULL);
            printf("    CL_DEVICE_VERSION: %s\n", value);
            free(value);

            // print driver version
            clGetDeviceInfo(devices[j], CL_DRIVER_VERSION, 0, NULL, &valueSize);
            value = (char*)malloc(valueSize);
            clGetDeviceInfo(devices[j], CL_DRIVER_VERSION, valueSize, value, NULL);
            printf("    CL_DRIVER_VERSION: %s\n", value);
            free(value);

            // print c version supported by compiler for device
            clGetDeviceInfo(devices[j], CL_DEVICE_OPENCL_C_VERSION, 0, NULL, &valueSize);
            value = (char*)malloc(valueSize);
            clGetDeviceInfo(devices[j], CL_DEVICE_OPENCL_C_VERSION, valueSize, value, NULL);
            printf("    CL_DEVICE_OPENCL_C_VERSION: %s\n", value);
            free(value);

            // print parallel compute units
            clGetDeviceInfo(devices[j], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(cl_uint), &maxComputeUnits, NULL);
            printf("    CL_DEVICE_MAX_COMPUTE_UNITS: %d\n", maxComputeUnits);

            // print maximum clock frequency
            clGetDeviceInfo(devices[j], CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(cl_uint), &maxClockFreq, NULL);
            printf("    CL_DEVICE_MAX_CLOCK_FREQUENCY: %d MHz\n", maxClockFreq);

            // print maximum work item dimension
            clGetDeviceInfo(devices[j], CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(unsigned int), &maxWorkItemDimension, NULL);
            printf("    CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS: %d\n", maxWorkItemDimension);

            // print maximum work item size
            clGetDeviceInfo(devices[j], CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(size_t)*maxWorkItemDimension, &maxWorkItemSize, NULL);
            printf("    CL_DEVICE_MAX_WORK_ITEM_SIZES: (%d, %d, %d)\n", maxWorkItemSize[0], maxWorkItemSize[1], maxWorkItemSize[2]);

            // print maximum work group size
            clGetDeviceInfo(devices[j], CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(size_t), &maxWorkGroupSize, NULL);
            printf("    CL_DEVICE_MAX_WORK_GROUP_SIZES: %d\n", maxWorkGroupSize);

            // print device address bits
            clGetDeviceInfo(devices[j], CL_DEVICE_ADDRESS_BITS, sizeof(cl_uint), &devAddressBit, NULL);
            printf("    CL_DEVICE_ADDRESS_BITS: %d\n", devAddressBit);

            // print device memory base address align
            clGetDeviceInfo(devices[j], CL_DEVICE_MEM_BASE_ADDR_ALIGN, sizeof(cl_uint), &memBaseAddressAlign, NULL);
            printf("    CL_DEVICE_MEM_BASE_ADDR_ALIGN: %d\n", memBaseAddressAlign);

            // print device max memory allocation size
            clGetDeviceInfo(devices[j], CL_DEVICE_MAX_MEM_ALLOC_SIZE, sizeof(cl_ulong), &maxMemAllocSize, NULL);
            printf("    CL_DEVICE_MAX_MEM_ALLOC_SIZE: %ld\n", maxMemAllocSize);

            // print device global memry size
            clGetDeviceInfo(devices[j], CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(cl_ulong), &gloMemSize, NULL);
            printf("    CL_DEVICE_GLOBAL_MEM_SIZE: %ld\n", gloMemSize);

            // print device maximum constant buffer size
            clGetDeviceInfo(devices[j], CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE, sizeof(cl_ulong), &maxConstBuffSize, NULL);
            printf("    CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE: %ld\n", maxConstBuffSize);

            // print device global memory cache size
            clGetDeviceInfo(devices[j], CL_DEVICE_GLOBAL_MEM_CACHE_SIZE, sizeof(cl_ulong), &gloMemCacheSize, NULL);
            printf("    CL_DEVICE_GLOBAL_MEM_CACHE_SIZE: %ld\n", gloMemCacheSize);

            // print device global memory cacheline size
            clGetDeviceInfo(devices[j], CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE, sizeof(cl_uint), &gloMemCacheLineSize, NULL);
            printf("    CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE: %d\n", gloMemCacheLineSize);

            // print device local memory size
            clGetDeviceInfo(devices[j], CL_DEVICE_LOCAL_MEM_SIZE, sizeof(cl_ulong), &localMemSize, NULL);
            printf("    CL_DEVICE_LOCAL_MEM_SIZE: %ld\n", localMemSize);

            // print device profiling timer resoultion
            clGetDeviceInfo(devices[j], CL_DEVICE_PROFILING_TIMER_RESOLUTION, sizeof(size_t), &profTimerRes, NULL);
            printf("    CL_DEVICE_PROFILING_TIMER_RESOLUTION: %d\n", profTimerRes);

            // print device image support
            clGetDeviceInfo(devices[j], CL_DEVICE_IMAGE_SUPPORT, sizeof(cl_bool), &imageSupport, NULL);
            printf("    CL_DEVICE_IMAGE_SUPPORT: %d\n", imageSupport);

            // print device error correction support
            clGetDeviceInfo(devices[j], CL_DEVICE_ERROR_CORRECTION_SUPPORT, sizeof(cl_bool), &errorCorrSupport, NULL);
            printf("    CL_DEVICE_ERROR_CORRECTION_SUPPORT: %d\n", errorCorrSupport);

            // print device error correction support
            clGetDeviceInfo(devices[j], CL_DEVICE_HOST_UNIFIED_MEMORY, sizeof(cl_bool), &hostUnifiedMem, NULL);
            printf("    CL_DEVICE_HOST_UNIFIED_MEMORY: %d\n", hostUnifiedMem);

            // print device entexsion
            clGetDeviceInfo(devices[j], CL_DEVICE_EXTENSIONS, 0, NULL, &valueSize);
            value = (char*)malloc(valueSize);
            clGetDeviceInfo(devices[j], CL_DEVICE_EXTENSIONS, valueSize, value, NULL);
            printf("    CL_DEVICE_EXTENSIONS: %s\n", value);
            free(value);

            // print device preferred vector width int
            clGetDeviceInfo(devices[j], CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT, sizeof(cl_uint), &preVecWidthInt, NULL);
            printf("    CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT: %d\n", preVecWidthInt);

            // print device preferred vector width long
            clGetDeviceInfo(devices[j], CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG, sizeof(cl_uint), &preVecWidthLong, NULL);
            printf("    CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG: %d\n", preVecWidthLong);

            // print device preferred vector width float
            clGetDeviceInfo(devices[j], CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT, sizeof(cl_uint), &preVecWidthFloat, NULL);
            printf("    CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT: %d\n", preVecWidthFloat);

            // print device preferred vector width double
            clGetDeviceInfo(devices[j], CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE, sizeof(cl_uint), &preVecWidthDouble, NULL);
            printf("    CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE: %d\n", preVecWidthDouble);

            // print device native vector width int
            clGetDeviceInfo(devices[j], CL_DEVICE_NATIVE_VECTOR_WIDTH_INT, sizeof(cl_uint), &nativeVecWidthInt, NULL);
            printf("    CL_DEVICE_NATIVE_VECTOR_WIDTH_INT: %d\n", nativeVecWidthInt);

            // print device native vector width long
            clGetDeviceInfo(devices[j], CL_DEVICE_NATIVE_VECTOR_WIDTH_LONG, sizeof(cl_uint), &nativeVecWidthLong, NULL);
            printf("    CL_DEVICE_NATIVE_VECTOR_WIDTH_LONG: %d\n", nativeVecWidthLong);

            // print device native vector width float
            clGetDeviceInfo(devices[j], CL_DEVICE_NATIVE_VECTOR_WIDTH_FLOAT, sizeof(cl_uint), &nativeVecWidthFloat, NULL);
            printf("    CL_DEVICE_NATIVE_VECTOR_WIDTH_FLOAT: %d\n", nativeVecWidthFloat);

            // print device native vector width double
            clGetDeviceInfo(devices[j], CL_DEVICE_NATIVE_VECTOR_WIDTH_DOUBLE, sizeof(cl_uint), &nativeVecWidthDouble, NULL);
            printf("    CL_DEVICE_NATIVE_VECTOR_WIDTH_DOUBLE: %d\n", nativeVecWidthDouble);

            printf("\n");
            printf("------------------------------------------------------------------------------\n\n");
        }

        // get all CPU devices
        clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_CPU, 0, NULL, &deviceCount);
        devices = (cl_device_id*)malloc(sizeof(cl_device_id)* deviceCount);
        clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_CPU, deviceCount, devices, NULL);

        // for each device print critical attributes
        for (j = 0; j < deviceCount; j++) {

            printf("   CPU Device [%d]: \n", j + 1);

            // print device name
            clGetDeviceInfo(devices[j], CL_DEVICE_NAME, 0, NULL, &valueSize);
            value = (char*)malloc(valueSize);
            clGetDeviceInfo(devices[j], CL_DEVICE_NAME, valueSize, value, NULL);
            printf("    CL_DEVICE_NAME: %s\n", value);
            free(value);

            // print device vendor
            clGetDeviceInfo(devices[j], CL_DEVICE_VENDOR, 0, NULL, &valueSize);
            value = (char*)malloc(valueSize);
            clGetDeviceInfo(devices[j], CL_DEVICE_VENDOR, valueSize, value, NULL);
            printf("    CL_DEVICE_VENDOR: %s\n", value);
            free(value);

            // print device profile
            clGetDeviceInfo(devices[j], CL_DEVICE_PROFILE, 0, NULL, &valueSize);
            value = (char*)malloc(valueSize);
            clGetDeviceInfo(devices[j], CL_DEVICE_PROFILE, valueSize, value, NULL);
            printf("    CL_DEVICE_PROFILE: %s\n", value);
            free(value);

            // print device version
            clGetDeviceInfo(devices[j], CL_DEVICE_VERSION, 0, NULL, &valueSize);
            value = (char*)malloc(valueSize);
            clGetDeviceInfo(devices[j], CL_DEVICE_VERSION, valueSize, value, NULL);
            printf("    CL_DEVICE_VERSION: %s\n", value);
            free(value);

            // print driver version
            clGetDeviceInfo(devices[j], CL_DRIVER_VERSION, 0, NULL, &valueSize);
            value = (char*)malloc(valueSize);
            clGetDeviceInfo(devices[j], CL_DRIVER_VERSION, valueSize, value, NULL);
            printf("    CL_DRIVER_VERSION: %s\n", value);
            free(value);

            // print c version supported by compiler for device
            clGetDeviceInfo(devices[j], CL_DEVICE_OPENCL_C_VERSION, 0, NULL, &valueSize);
            value = (char*)malloc(valueSize);
            clGetDeviceInfo(devices[j], CL_DEVICE_OPENCL_C_VERSION, valueSize, value, NULL);
            printf("    CL_DEVICE_OPENCL_C_VERSION: %s\n", value);
            free(value);

            // print parallel compute units
            clGetDeviceInfo(devices[j], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(cl_uint), &maxComputeUnits, NULL);
            printf("    CL_DEVICE_MAX_COMPUTE_UNITS: %d\n", maxComputeUnits);

            // print maximum clock frequency
            clGetDeviceInfo(devices[j], CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(cl_uint), &maxClockFreq, NULL);
            printf("    CL_DEVICE_MAX_CLOCK_FREQUENCY: %d MHz\n", maxClockFreq);

            // print maximum work item dimension
            clGetDeviceInfo(devices[j], CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(unsigned int), &maxWorkItemDimension, NULL);
            printf("    CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS: %d\n", maxWorkItemDimension);

            // print maximum work item size
            clGetDeviceInfo(devices[j], CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(size_t)*maxWorkItemDimension, &maxWorkItemSize, NULL);
            printf("    CL_DEVICE_MAX_WORK_ITEM_SIZES: (%d, %d, %d)\n", maxWorkItemSize[0], maxWorkItemSize[1], maxWorkItemSize[2]);

            // print maximum work group size
            clGetDeviceInfo(devices[j], CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(size_t), &maxWorkGroupSize, NULL);
            printf("    CL_DEVICE_MAX_WORK_GROUP_SIZES: %d\n", maxWorkGroupSize);

            // print device address bits
            clGetDeviceInfo(devices[j], CL_DEVICE_ADDRESS_BITS, sizeof(cl_uint), &devAddressBit, NULL);
            printf("    CL_DEVICE_ADDRESS_BITS: %d\n", devAddressBit);

            // print device memory base address align
            clGetDeviceInfo(devices[j], CL_DEVICE_MEM_BASE_ADDR_ALIGN, sizeof(cl_uint), &memBaseAddressAlign, NULL);
            printf("    CL_DEVICE_MEM_BASE_ADDR_ALIGN: %d\n", memBaseAddressAlign);

            // print device max memory allocation size
            clGetDeviceInfo(devices[j], CL_DEVICE_MAX_MEM_ALLOC_SIZE, sizeof(cl_ulong), &maxMemAllocSize, NULL);
            printf("    CL_DEVICE_MAX_MEM_ALLOC_SIZE: %ld\n", maxMemAllocSize);

            // print device global memry size
            clGetDeviceInfo(devices[j], CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(cl_ulong), &gloMemSize, NULL);
            printf("    CL_DEVICE_GLOBAL_MEM_SIZE: %ld\n", gloMemSize);

            // print device maximum constant buffer size
            clGetDeviceInfo(devices[j], CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE, sizeof(cl_ulong), &maxConstBuffSize, NULL);
            printf("    CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE: %ld\n", maxConstBuffSize);

            // print device global memory cache size
            clGetDeviceInfo(devices[j], CL_DEVICE_GLOBAL_MEM_CACHE_SIZE, sizeof(cl_ulong), &gloMemCacheSize, NULL);
            printf("    CL_DEVICE_GLOBAL_MEM_CACHE_SIZE: %ld\n", gloMemCacheSize);

            // print device global memory cacheline size
            clGetDeviceInfo(devices[j], CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE, sizeof(cl_uint), &gloMemCacheLineSize, NULL);
            printf("    CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE: %d\n", gloMemCacheLineSize);

            // print device local memory size
            clGetDeviceInfo(devices[j], CL_DEVICE_LOCAL_MEM_SIZE, sizeof(cl_ulong), &localMemSize, NULL);
            printf("    CL_DEVICE_LOCAL_MEM_SIZE: %ld\n", localMemSize);

            // print device profiling timer resoultion
            clGetDeviceInfo(devices[j], CL_DEVICE_PROFILING_TIMER_RESOLUTION, sizeof(size_t), &profTimerRes, NULL);
            printf("    CL_DEVICE_PROFILING_TIMER_RESOLUTION: %d\n", profTimerRes);

            // print device image support
            clGetDeviceInfo(devices[j], CL_DEVICE_IMAGE_SUPPORT, sizeof(cl_bool), &imageSupport, NULL);
            printf("    CL_DEVICE_IMAGE_SUPPORT: %d\n", imageSupport);

            // print device error correction support
            clGetDeviceInfo(devices[j], CL_DEVICE_ERROR_CORRECTION_SUPPORT, sizeof(cl_bool), &errorCorrSupport, NULL);
            printf("    CL_DEVICE_ERROR_CORRECTION_SUPPORT: %d\n", errorCorrSupport);

            // print device error correction support
            clGetDeviceInfo(devices[j], CL_DEVICE_HOST_UNIFIED_MEMORY, sizeof(cl_bool), &hostUnifiedMem, NULL);
            printf("    CL_DEVICE_HOST_UNIFIED_MEMORY: %d\n", hostUnifiedMem);

            // print device entexsion
            clGetDeviceInfo(devices[j], CL_DEVICE_EXTENSIONS, 0, NULL, &valueSize);
            value = (char*)malloc(valueSize);
            clGetDeviceInfo(devices[j], CL_DEVICE_EXTENSIONS, valueSize, value, NULL);
            printf("    CL_DEVICE_EXTENSIONS: %s\n", value);
            free(value);

            // print device preferred vector width int
            clGetDeviceInfo(devices[j], CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT, sizeof(cl_uint), &preVecWidthInt, NULL);
            printf("    CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT: %d\n", preVecWidthInt);

            // print device preferred vector width long
            clGetDeviceInfo(devices[j], CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG, sizeof(cl_uint), &preVecWidthLong, NULL);
            printf("    CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG: %d\n", preVecWidthLong);

            // print device preferred vector width float
            clGetDeviceInfo(devices[j], CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT, sizeof(cl_uint), &preVecWidthFloat, NULL);
            printf("    CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT: %d\n", preVecWidthFloat);

            // print device preferred vector width double
            clGetDeviceInfo(devices[j], CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE, sizeof(cl_uint), &preVecWidthDouble, NULL);
            printf("    CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE: %d\n", preVecWidthDouble);

            // print device native vector width int
            clGetDeviceInfo(devices[j], CL_DEVICE_NATIVE_VECTOR_WIDTH_INT, sizeof(cl_uint), &nativeVecWidthInt, NULL);
            printf("    CL_DEVICE_NATIVE_VECTOR_WIDTH_INT: %d\n", nativeVecWidthInt);

            // print device native vector width long
            clGetDeviceInfo(devices[j], CL_DEVICE_NATIVE_VECTOR_WIDTH_LONG, sizeof(cl_uint), &nativeVecWidthLong, NULL);
            printf("    CL_DEVICE_NATIVE_VECTOR_WIDTH_LONG: %d\n", nativeVecWidthLong);

            // print device native vector width float
            clGetDeviceInfo(devices[j], CL_DEVICE_NATIVE_VECTOR_WIDTH_FLOAT, sizeof(cl_uint), &nativeVecWidthFloat, NULL);
            printf("    CL_DEVICE_NATIVE_VECTOR_WIDTH_FLOAT: %d\n", nativeVecWidthFloat);

            // print device native vector width double
            clGetDeviceInfo(devices[j], CL_DEVICE_NATIVE_VECTOR_WIDTH_DOUBLE, sizeof(cl_uint), &nativeVecWidthDouble, NULL);
            printf("    CL_DEVICE_NATIVE_VECTOR_WIDTH_DOUBLE: %d\n", nativeVecWidthDouble);

            printf("\n");
            printf("------------------------------------------------------------------------------\n\n");
        }

        free(devices);

    }
    free(platforms);
}

jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    ALOGI("OpenCL4J JNI_OnLoad");
    printOpenCLInfo();
    return JNI_VERSION_1_6;
}

void JNI_OnUnload(JavaVM *vm, void *reserved) {
    ALOGI("OpenCL4J JNI_OnUnload");
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_peerless2012_demo_opencl_jni_OpenCL4J_nCreateTexture(JNIEnv *env, jobject thiz) {
    // https://stackoverflow.com/a/57069634/3769941
    EGLint clientBufferAttrs[] = { EGL_WIDTH, 3840,
                                   EGL_HEIGHT, 2160,
                                   EGL_RED_SIZE, 8,
                                   EGL_GREEN_SIZE, 8,
                                   EGL_BLUE_SIZE, 8,
                                   EGL_ALPHA_SIZE, 8,
                                   EGL_NATIVE_BUFFER_USAGE_ANDROID, EGL_NATIVE_BUFFER_USAGE_TEXTURE_BIT_ANDROID,
                                   EGL_NONE };
    // https://registry.khronos.org/EGL/extensions/ANDROID/EGL_ANDROID_create_native_client_buffer.txt
    EGLClientBuffer eglClientBuffer = eglCreateNativeClientBufferANDROID(clientBufferAttrs);
    if (eglClientBuffer == nullptr) {
        ALOGE("eglCreateNativeClientBufferANDROID error: %d", eglGetError());
        return 0;
    }

    EGLImageKHR eglImageKhr = eglCreateImageKHR(eglGetCurrentDisplay(),
                                                eglGetCurrentContext(),
                                                EGL_NATIVE_BUFFER_ANDROID,
                                                eglClientBuffer,
                                                0);
    if (eglImageKhr == nullptr) {
        ALOGE("eglCreateImageKHR error: %d", eglGetError());
        return 0;
    }

    GLuint texture;
    glGenTextures(1, &texture);
    if (texture < 0) {
        ALOGE("glGenTextures error: %d", eglGetError());
        return 0;
    }
    glBindTexture(GL_TEXTURE_EXTERNAL_OES, texture);
    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glEGLImageTargetTexture2DOES(GL_TEXTURE_EXTERNAL_OES, (GLeglImageOES) eglImageKhr);
    return texture;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_peerless2012_demo_opencl_jni_OpenCL4J_nColorfulToGray(JNIEnv *env, jobject thiz,
                                                               jint texture) {
//     https://gist.github.com/timweri/de34d94349a206f4396cb0b8724edfe6
//    cl_context clContext = clCreateContext();
//    clCreateFromGLTexture(clContext, )
//    clCreateFromEGLImageKHR()
}

