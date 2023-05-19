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
 * @DateTime 2023/5/18 16:18
 * @Version V1.0
 * @Description:
 */
class GLInGLOutRender(context: Context) : TextureRender(context) {

    override fun onCreateTexture(bitmap: Bitmap): Int {
        // texture
        val txtArr = IntArray(2)
        GLES20.glGenTextures(txtArr.size, txtArr, 0)
        if (txtArr[0] <= 0 || txtArr[1] <= 0) {
            Log.w(TAG, "Generate texture fail.")
            return 0
        }
        txtArr.forEachIndexed { index, value ->
            GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, value)
            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_S, GLES20.GL_CLAMP_TO_EDGE)
            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_T, GLES20.GL_CLAMP_TO_EDGE)
            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MAG_FILTER, GLES20.GL_LINEAR)
            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MIN_FILTER, GLES20.GL_LINEAR)
            if (index == 0) {
                GLUtils.texImage2D(GLES20.GL_TEXTURE_2D, 0, bitmap, 0)
            } else {
                GLES20.glTexImage2D(GLES20.GL_TEXTURE_2D, 0, GLES20.GL_RGBA, bitmap.width, bitmap.height, 0, GLES20.GL_RGBA, GLES20.GL_UNSIGNED_BYTE, null)
            }
        }

        val texture = if (OpenCL4J.colorfulToGray(bitmap.width, bitmap.height, txtArr[0], txtArr[1])) {
            txtArr[1]
        } else {
            txtArr[0]
        }
        return texture
    }
}