/**
 * @Author peerless2012
 * @Email peerless2012@126.com
 * @DateTime 2023/3/3 16:57
 * @Version V1.0
 * @Description 
 */

#ifndef ANDROIDOPENCL_OPENCL4J_H
#define ANDROIDOPENCL_OPENCL4J_H

#include "jni.h"
#include "CL/cl.h"
#include "AndroidLog.h"

#ifdef __cplusplus
extern "C"
{
#endif

    extern JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *reserved);

    extern JNIEXPORT void JNI_OnUnload(JavaVM *vm, void *reserved);

#ifdef __cplusplus
}
#endif

#endif //ANDROIDOPENCL_OPENCL4J_H
