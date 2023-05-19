package com.peerless2012.demo.opencl.render

import android.content.Context
import android.graphics.Bitmap
import android.opengl.GLES20
import android.opengl.GLUtils
import android.util.Log
import com.peerless2012.demo.opencl.jni.OpenCL4J

/**
 * @Author peerless2012
 * @Email peerless2012@126.com
 * @DateTime 2023/5/18 17:08
 * @Version V1.0
 * @Description:
 */
class GLInCLOutRender(context: Context) : TextureRender(context) {

    override fun onCreateTexture(bitmap: Bitmap): Int {
        // texture
        val txtArr = IntArray(1)
        GLES20.glGenTextures(txtArr.size, txtArr, 0)
        if (txtArr[0] <= 0) {
            Log.w(TAG, "Generate texture fail.")
            return 0
        }
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, txtArr[0])
        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_S, GLES20.GL_CLAMP_TO_EDGE)
        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_T, GLES20.GL_CLAMP_TO_EDGE)
        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MAG_FILTER, GLES20.GL_LINEAR)
        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MIN_FILTER, GLES20.GL_LINEAR)
        GLUtils.texImage2D(GLES20.GL_TEXTURE_2D, 0, bitmap, 0)

        val outTex = OpenCL4J.colorfulToGray(bitmap.width, bitmap.height, txtArr[0])
        return if (outTex > 0) {
            outTex
        } else {
            txtArr[0]
        }
    }
}