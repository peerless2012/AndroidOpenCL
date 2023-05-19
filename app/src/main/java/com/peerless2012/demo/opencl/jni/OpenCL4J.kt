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
        return nColorfulToGrayGLInGLOut(width, height, input, output)
    }

    fun colorfulToGray(width: Int, height: Int, tex: Int): Int {
        if (!OpenCLJNI.findOpenCL) return tex
        return nColorfulToGrayGLInCLOut(width, height, tex)
    }

    private external fun nCreateTexture(): Int

    private external fun nColorfulToGrayGLInGLOut(width: Int, height: Int, input: Int, output: Int): Boolean

    private external fun nColorfulToGrayGLInCLOut(width: Int, height: Int, tex: Int): Int

}