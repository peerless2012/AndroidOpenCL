/**
 * @Author peerless2012
 * @Email peerless2012@126.com
 * @DateTime 2023/3/3 16:57
 * @Version V1.0
 * @Description 
 */
#include "OpenCL4J.h"
#include "CL/cl_gl.h"
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
#include "string"
#define printf ALOGV

struct CLEnvironment {

    bool ready = false;

    cl_context clContext = nullptr;

    cl_command_queue clCommandQueue = nullptr;

    cl_program clProgram = nullptr;

    cl_kernel clKernel = nullptr;

    CLEnvironment(const char** source, const char* name) {
        ALOGI("CLEnvironment ctor");
        char* value;
        size_t valueSize;
        cl_uint platformCount;
        cl_platform_id* platforms;
        cl_platform_id platformId = nullptr;
        cl_uint deviceCount;
        cl_device_id* devices;
        cl_device_id deviceId = nullptr;
        clGetPlatformIDs(0, nullptr, &platformCount);
        platforms = (cl_platform_id*)malloc(sizeof(cl_platform_id)* platformCount);
        clGetPlatformIDs(platformCount, platforms, NULL);
        for (cl_uint i = 0; i < platformCount; i++) {
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
            for (cl_uint j = 0; j < deviceCount; j++) {
                // print device entexsion
                clGetDeviceInfo(devices[j], CL_DEVICE_EXTENSIONS, 0, NULL, &valueSize);
                value = (char*)malloc(valueSize);
                clGetDeviceInfo(devices[j], CL_DEVICE_EXTENSIONS, valueSize, value, NULL);
                        printf("    CL_DEVICE_EXTENSIONS: %s\n", value);
                if (strpbrk(value, "cl_khr_gl_sharing") != nullptr
                    && strpbrk(value, "cl_khr_egl_event") != nullptr) {
                    platformId = platforms[i];
                    deviceId = devices[j];
                    break;
                }
                free(value);
            }

        }
        if (deviceId == nullptr) {
            return;
        }

        EGLContext eglContext = eglGetCurrentContext();
        EGLDisplay eglDisplay = eglGetCurrentDisplay();
        cl_context_properties props[] = {CL_GL_CONTEXT_KHR,(cl_context_properties) eglContext,
                                         CL_EGL_DISPLAY_KHR,(cl_context_properties) eglDisplay,
                                         CL_CONTEXT_PLATFORM,(cl_context_properties) platformId,
                                         CL_NONE};
        // https://man.opencl.org/clCreateContext.html
        int error_code;
        clContext = clCreateContext(props
                , 1
                , &deviceId
                , nullptr
                , nullptr
                , &error_code);
        if (!clContext) {
            ALOGE("Create cl_context error: %d", error_code);
            return;
        }

        clCommandQueue = clCreateCommandQueue(clContext
                , deviceId
                , CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE
                , &error_code);
        if (!clCommandQueue) {
            ALOGE("Create cl_command_queue error: %d", error_code);
            return;
        }

        clProgram = clCreateProgramWithSource(clContext
                , 1
                , source
                , nullptr
                , &error_code);
        if (!clProgram) {
            ALOGE("Create cl_program error: %d", error_code);
            return;
        }
        cl_program_info clBuildProgramResult = clBuildProgram(clProgram, 0, NULL, NULL, NULL, NULL);
        if (clBuildProgramResult != CL_SUCCESS) {
            clGetProgramBuildInfo(clProgram
                    , deviceId
                    , CL_PROGRAM_BUILD_LOG
                    , 0
                    , nullptr
                    , &valueSize);
            value = (char*) malloc(valueSize);
            clGetProgramBuildInfo(clProgram
                    , deviceId
                    , CL_PROGRAM_BUILD_LOG
                    , valueSize
                    , value
                    , nullptr);
            ALOGI("Build cl_program info: %s", value);
            free(value);
            return;
        }

        clKernel = clCreateKernel(clProgram, name, &error_code);
        if (!clKernel) {
            ALOGE("Create cl_kernel error: %d", error_code);
            return;
        }

        ready = true;
    }

    ~CLEnvironment() {
        ALOGI("CLEnvironment ~ctor");
        if (clProgram != nullptr) {
            clReleaseProgram(clProgram);
        }
        if (clCommandQueue != nullptr) {
            clReleaseCommandQueue(clCommandQueue);
        }
        if (clKernel != nullptr) {
            clReleaseKernel(clKernel);
        }
        if (clContext != nullptr) {
            clReleaseContext(clContext);
        }
    }
};

static const char * source =
         "__kernel void rgba_to_gray(__read_only image2d_t input, __write_only image2d_t output)\n"
         "{\n"
         "    int2 coord = (int2)(get_global_id(0), get_global_id(1));\n"
         "    float4 pixel = read_imagef(input, CLK_NORMALIZED_COORDS_FALSE|CLK_ADDRESS_CLAMP|CLK_FILTER_NEAREST, coord);\n"
         "    float gray = dot(pixel.xyz, (float3)(0.299, 0.587, 0.114));\n"
         "    write_imagef(output, coord, (float4)(gray, gray, gray, pixel.w));\n"
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

/**
 * Texture2D to EGLImageKHR
 * Make sure call this func after glFinish() the Texture2D.
 * @param textureId
 * @return
 */
static EGLImageKHR textureToEGLImageKHR(GLint textureId) {
    //KHRImage
    EGLint eglImageAttributes[] = {
            EGL_GL_TEXTURE_LEVEL_KHR, 0
            , EGL_IMAGE_PRESERVED_KHR, EGL_TRUE
            , EGL_NONE
    };
    EGLClientBuffer egl_buffer = (EGLClientBuffer) (textureId);
    EGLImageKHR eglImageKHR = eglCreateImageKHR(eglGetCurrentDisplay()
            , eglGetCurrentContext()
            , EGL_GL_TEXTURE_2D_KHR
            , egl_buffer
            , eglImageAttributes);
    ALOGD("eglImageHandle = %x", eglImageKHR);
    return eglImageKHR;
}

/**
 * EGLImageKHR to Texture2D
 * @param eglImageKhr
 * @return
 */
static GLuint eglImageKHRToTexture(EGLImageKHR eglImageKhr) {
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glEGLImageTargetTexture2DOES(GL_TEXTURE_2D, eglImageKhr);
    return texture;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_peerless2012_demo_opencl_jni_OpenCL4J_nCreateTexture(JNIEnv *env, jobject thiz) {
    // https://stackoverflow.com/a/57069634/3769941
//    EGLint clientBufferAttrs[] = { EGL_WIDTH, 3840,
//                                   EGL_HEIGHT, 2160,
//                                   EGL_RED_SIZE, 8,
//                                   EGL_GREEN_SIZE, 8,
//                                   EGL_BLUE_SIZE, 8,
//                                   EGL_ALPHA_SIZE, 8,
//                                   EGL_NATIVE_BUFFER_USAGE_ANDROID, EGL_NATIVE_BUFFER_USAGE_TEXTURE_BIT_ANDROID,
//                                   EGL_NONE };
//    // https://registry.khronos.org/EGL/extensions/ANDROID/EGL_ANDROID_create_native_client_buffer.txt
//    EGLClientBuffer eglClientBuffer = eglCreateNativeClientBufferANDROID(clientBufferAttrs);
//    if (eglClientBuffer == nullptr) {
//        ALOGE("eglCreateNativeClientBufferANDROID error: %d", eglGetError());
//        return 0;
//    }
//
//    EGLImageKHR eglImageKhr = eglCreateImageKHR(eglGetCurrentDisplay(),
//                                                eglGetCurrentContext(),
//                                                EGL_NATIVE_BUFFER_ANDROID,
//                                                eglClientBuffer,
//                                                0);
//    if (eglImageKhr == nullptr) {
//        ALOGE("eglCreateImageKHR error: %d", eglGetError());
//        return 0;
//    }
//
//    GLuint texture;
//    glGenTextures(1, &texture);
//    if (texture < 0) {
//        ALOGE("glGenTextures error: %d", eglGetError());
//        return 0;
//    }
//    glBindTexture(GL_TEXTURE_EXTERNAL_OES, texture);
//    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//    glEGLImageTargetTexture2DOES(GL_TEXTURE_EXTERNAL_OES, (GLeglImageOES) eglImageKhr);
//    return texture;
    return 0;
}

/**
 * https://deepinout.com/opencl/opencl-interoperates-with-opengl/opencl-clcreatefromgltexture.html
 */
extern "C"
JNIEXPORT jboolean JNICALL
Java_com_peerless2012_demo_opencl_jni_OpenCL4J_nColorfulToGrayGLInGLOut(JNIEnv *env, jobject thiz,
                                                                        jint width, jint height,
                                                                        jint input, jint output) {
    CLEnvironment clEnvironment(&source, "rgba_to_gray");
    if (!clEnvironment.ready) {
        return false;
    }
    int error_code;
    // https://man.opencl.org/clCreateFromGLTexture.html
    cl_mem inputMem = clCreateFromGLTexture(clEnvironment.clContext
            , CL_MEM_READ_ONLY
            , GL_TEXTURE_2D
            , 0
            , input
            , &error_code);
    if (!inputMem) {
        ALOGE("Create cl_mem input error: %d", error_code);
        clReleaseMemObject(inputMem);
        return false;
    }
    cl_mem outputMem = clCreateFromGLTexture(clEnvironment.clContext
            , CL_MEM_WRITE_ONLY
            , GL_TEXTURE_2D
            , 0
            , output
            , &error_code);
    if (!outputMem) {
        ALOGE("Create cl_mem output error: %d", error_code);
        return false;
    }

    clSetKernelArg(clEnvironment.clKernel, 0, sizeof(cl_mem), &inputMem);
    clSetKernelArg(clEnvironment.clKernel, 1, sizeof(cl_mem), &outputMem);

    size_t global_size[2] = {(size_t) width, (size_t) height};
    clEnqueueNDRangeKernel(clEnvironment.clCommandQueue
            , clEnvironment.clKernel
            , 2
            , NULL
            , global_size
            , NULL
            , 0
            , NULL
            , NULL);

    // Make sure the command is finish.
    clFinish(clEnvironment.clCommandQueue);
    return true;
}

extern "C"
JNIEXPORT jint JNICALL
        Java_com_peerless2012_demo_opencl_jni_OpenCL4J_nColorfulToGrayGLInCLOut(JNIEnv * env,
                                                                                jobject thiz,
                                                                                jint width,
                                                                                jint height,
                                                                                jint inTex
) {
    CLEnvironment clEnvironment(&source, "rgba_to_gray");
    if (!clEnvironment.ready) {
        return false;
    }
    int error_code;
    // https://man.opencl.org/clCreateFromGLTexture.html
    cl_mem inputMem = clCreateFromGLTexture(clEnvironment.clContext
            , CL_MEM_READ_ONLY
            , GL_TEXTURE_2D
            , 0
            , inTex
            , &error_code);
    if (inputMem == nullptr) {
        ALOGE("Create cl_mem input error: %d", error_code);
        return inTex;
    }

    // https://registry.khronos.org/OpenCL/specs/3.0-unified/html/OpenCL_API.html#image-format-descriptor
    cl_image_format clImageFormat = {CL_RGBA
                                     , CL_UNORM_INT8};
    // https://registry.khronos.org/OpenCL/specs/3.0-unified/html/OpenCL_API.html#image-descriptor
    cl_image_desc clImageDesc = {CL_MEM_OBJECT_IMAGE2D
                                 , (size_t) width
                                 , (size_t) height
                                 , 0
                                 , 1
                                 , 0
                                 , 0
                                 , 0
                                 , 0};
    // https://registry.khronos.org/OpenCL/sdk/3.0/docs/man/html/clCreateImage.html
    cl_mem outputMem = clCreateImage(clEnvironment.clContext
                  , CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR
                  , &clImageFormat
                  , &clImageDesc
                  , nullptr
                  , &error_code);
    if (outputMem == nullptr) {
        ALOGE("Create cl_mem out error: %d", error_code);
        return inTex;
    }

    clSetKernelArg(clEnvironment.clKernel, 0, sizeof(cl_mem), &inputMem);
    clSetKernelArg(clEnvironment.clKernel, 1, sizeof(cl_mem), &outputMem);

    size_t global_size[2] = {(size_t) width, (size_t) height};
    clEnqueueNDRangeKernel(clEnvironment.clCommandQueue
            , clEnvironment.clKernel
            , 2
            , NULL
            , global_size
            , NULL
            , 0
            , NULL
            , NULL);


    clFinish(clEnvironment.clCommandQueue);

    size_t region[] = {0,0, 0};
    size_t dimen[] = {(size_t)width, (size_t)height, 1};
    size_t image_row_pitch = width;
    size_t image_slice_pitch = width * height;

    // https://deepinout.com/opencl/opencl-memory-object/opencl-image-object-mapping.html
    void *outBuffer = nullptr;
    void *mapBuffer = clEnqueueMapImage(clEnvironment.clCommandQueue
                      , outputMem
                      , CL_TRUE
                      , CL_MAP_READ
                      , region
                      , dimen
                      , &image_row_pitch
                      , &image_slice_pitch
                      , 0
                      , nullptr
                      , nullptr
                      , &error_code);

    if (mapBuffer == nullptr) {
        ALOGW("Map cl_mem error: %d", error_code);
        // 为pDeviceBuffer申请空间
        outBuffer = malloc(width * height * 4);
        int ret = clEnqueueReadImage(clEnvironment.clCommandQueue
                           , outputMem
                           , CL_TRUE
                           , region
                           , dimen
                           , 0
                           , 0
                           , outBuffer
                           , 0
                           , nullptr
                           , nullptr);
        if(ret != CL_SUCCESS) {
            ALOGE("cl_read out error: %d", ret);
            free(outBuffer);
            return inTex;
        }
    } else {
        ALOGI("Map cl_mem success");
    }

    // Make sure the command is finish.
    clFinish(clEnvironment.clCommandQueue);

    GLuint outTex = 0;
    glGenTextures(1, &outTex);
    glBindTexture(GL_TEXTURE_2D, outTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D
                 , 0
                 , GL_RGBA
                 , (GLsizei) width
                 , (GLsizei) height
                 , 0
                 , GL_RGBA
                 , GL_UNSIGNED_BYTE
                 , mapBuffer != nullptr ? mapBuffer : outBuffer);
    glFinish();

    GLenum error = glGetError();
    if (error != 0) {
        ALOGE("glTexImage2D error: %d", error);
    }

    if (outBuffer != nullptr) {
        free(outBuffer);
    }

    return outTex;
}
