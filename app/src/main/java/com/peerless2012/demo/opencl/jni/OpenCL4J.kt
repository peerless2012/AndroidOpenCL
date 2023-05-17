package com.peerless2012.demo.opencl.jni

import com.peerless2012.opencl.jni.OpenCLJNI

/**
 * @Author peerless2012
 * @Email peerless2012@126.com
 * @DateTime 2023/3/5 0:05
 * @Version V1.0
 * @Description
 */
object OpenCL4J {

    init {

        if (OpenCLJNI.findOpenCL) {
            System.loadLibrary("OpenCL4J")
        }

    }

    fun load() {

    }

    fun createTexture(): Int {
        return nCreateTexture()
    }

    fun colorfulToGray(width: Int, height: Int, input: Int, output: Int): Boolean {
        if (!OpenCLJNI.findOpenCL) return false
        return nColorfulToGray(width, height, input, output)
    }

    private external fun nCreateTexture(): Int

    private external fun nColorfulToGray(width: Int, height: Int, input: Int, output: Int): Boolean

}