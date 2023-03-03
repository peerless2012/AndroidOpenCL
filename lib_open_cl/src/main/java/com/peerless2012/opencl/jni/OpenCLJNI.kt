package com.peerless2012.opencl.jni

import android.util.Log

/**
 * @Author peerless2012
 * @Email peerless2012@126.com
 * @DateTime 2023/2/14 14:49
 * @Version V1.0
 * @Description:
 */
object OpenCLJNI {

    const val TAG = "OpenCL"

    init {
        var findOpenCL = false
        try {
            // /vendor/lib64/libOpenCL.so
            System.loadLibrary("OpenCL")
            Log.i(TAG, "OpenCL load success!!!")
            findOpenCL = true
        } catch (exception: UnsatisfiedLinkError) {
            exception.printStackTrace()
            try {
                System.load("/vendor/lib64/libOpenCL.so")
                Log.i(TAG, "OpenCL load success!!!")
                findOpenCL = true
            } catch (e2: UnsatisfiedLinkError) {
                Log.e(TAG, "OpenCL load Fail!!!")
                e2.printStackTrace()
            }
        }
        if (findOpenCL) {
            System.loadLibrary("OpenCL4J")
        }
    }

    fun load() {

    }

}