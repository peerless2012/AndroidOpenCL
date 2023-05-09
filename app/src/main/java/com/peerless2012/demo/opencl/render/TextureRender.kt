package com.peerless2012.demo.opencl.render

import android.content.Context
import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.opengl.GLES20
import android.opengl.GLSurfaceView
import android.opengl.GLUtils
import android.opengl.Matrix
import android.util.Log
import java.io.InputStream
import java.nio.ByteBuffer
import java.nio.ByteOrder
import java.nio.FloatBuffer
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.opengles.GL10

/**
 * @Author peerless2012
 * @Email peerless2012@126.com
 * @DateTime 2023/5/9 21:38
 * @Version V1.0
 * @Description
 */
class TextureRender(private val context: Context): GLSurfaceView.Renderer {

    companion object {

        private const val TAG = "TextureRender"

        val MVP_MATRIX = FloatArray(16).also {
            Matrix.setIdentityM(it, 0)
        }

        val TEXTURE_MATRIX = FloatArray(16).also {
            Matrix.setIdentityM(it, 0)
            Matrix.translateM(it, 0, 0f, 1f, 0f)
            Matrix.scaleM(it, 0, 1f, -1f, 1f)
        }

        private val GLES_VERTEX_DATA = floatArrayOf(
            -1f, -1f,
            1f, -1f,
            -1f, 1f,
            1f, 1f
        )

        private val GLES_TEXTURE_DATA = floatArrayOf(
            0f, 0f,
            1f, 0f,
            0f, 1f,
            1f, 1f
        )

        private val TEXTURE_BUFFER_ARRAY = arrayOf(
            GLES_VERTEX_DATA,
            GLES_TEXTURE_DATA
        )

        private const val GLES_COORDS_PER_VERTEX_COUNT = 2

        private val GLES_VERTEX_COUNT: Int = GLES_VERTEX_DATA.size / GLES_COORDS_PER_VERTEX_COUNT

        private const val GLES_PROGRAM_VERTEX_SOURCE =
                "uniform mat4 uMVPMatrix;\n" +
                "uniform mat4 uTexMatrix;\n" +
                "attribute vec4 av_Position;\n" +
                "attribute vec4 af_Position;\n" +
                "varying vec2 v_texPo;\n" +
                "void main() {\n" +
                "    v_texPo = (uTexMatrix * af_Position).xy;\n" +
                "    gl_Position = uMVPMatrix * av_Position;\n" +
                "}"

        private const val GLES_PROGRAM_FRAGMENT_SOURCE = 
                "precision mediump float; " +
                "varying vec2 v_texPo; " +
                "uniform sampler2D s_Texture; " +
                "void main() { " +
                "   gl_FragColor = texture2D(s_Texture, v_texPo); " +
                "} "

        private fun checkError(method: String) {
            var error: Int
            while (GLES20.glGetError().also { error = it } != GLES20.GL_NO_ERROR) {
                Log.e(TAG, "GL_ERROR " + method + ": glError 0x" + Integer.toHexString(error))
            }
        }

        private fun deleteProgram(program: Int) {
            GLES20.glDeleteProgram(program)
            checkError("glDeleteProgram")
        }

        private fun loadShader(shaderType: Int, shaderSource: String?): Int {
            val shader = GLES20.glCreateShader(shaderType)
            GLES20.glShaderSource(shader, shaderSource)
            GLES20.glCompileShader(shader)
            val status = IntArray(1)
            GLES20.glGetShaderiv(shader, GLES20.GL_COMPILE_STATUS, status, 0)
            if (status[0] == 0) {
                Log.e(TAG, "loadShader: compiler error = " + GLES20.glGetShaderInfoLog(shader))
                GLES20.glDeleteShader(shader)
                return 0
            }
            return shader
        }

        private fun createProgram(vertexSource: String, fragmentSource: String): Int {
            val mVertexShader: Int = loadShader(
                GLES20.GL_VERTEX_SHADER,
                vertexSource
            )
            val mFragmentShader: Int = loadShader(
                GLES20.GL_FRAGMENT_SHADER,
                fragmentSource
            )
            val program = GLES20.glCreateProgram()
            GLES20.glAttachShader(program, mVertexShader)
            GLES20.glAttachShader(program, mFragmentShader)
            GLES20.glLinkProgram(program)
            val status = IntArray(1)
            GLES20.glGetProgramiv(program, GLES20.GL_LINK_STATUS, status, 0)
            if (status[0] != GLES20.GL_TRUE) {
                Log.e(TAG, "createProgram: link error = " + GLES20.glGetProgramInfoLog(program))
                GLES20.glDeleteProgram(program)
                return 0
            }
            GLES20.glDeleteShader(mVertexShader)
            GLES20.glDeleteShader(mFragmentShader)
            return program
        }

        fun createProgram2D(): Int {
            return createProgram(
                GLES_PROGRAM_VERTEX_SOURCE,
                GLES_PROGRAM_FRAGMENT_SOURCE
            )
        }
    }

    private var texture = 0

    private var program = 0

    private var avPosition = 0

    private var afPosition = 0

    private var uMVPMatrix = 0

    private var uTexMatrix = 0

    private val bufferArray = IntArray(2)

    private fun loadBitmap(): Bitmap? {
        var inputStream: InputStream? = null
        try {
            inputStream = context.assets.open("forza.jpg")
            val bytes = inputStream.readBytes()
            return BitmapFactory.decodeByteArray(bytes, 0, bytes.size)
        } catch (e: Exception) {
            e.printStackTrace()
        } finally {
            try {
                inputStream?.close()
            } catch (ce: Exception) {
                ce.printStackTrace()
            }
        }
        return null
    }

    override fun onSurfaceCreated(gl: GL10, config: EGLConfig) {
        // program
        program = createProgram2D()
        if (program <= 0) {
            Log.w(TAG, "Create program fail.")
            return
        }
        uMVPMatrix = GLES20.glGetUniformLocation(program, "uMVPMatrix")
        checkError("uMVPMatrix")
        uTexMatrix = GLES20.glGetUniformLocation(program, "uTexMatrix")
        checkError("uTexMatrix")
        avPosition = GLES20.glGetAttribLocation(program, "av_Position")
        checkError("av_Position")
        afPosition = GLES20.glGetAttribLocation(program, "af_Position")
        checkError("af_Position")


        // buffer
        GLES20.glGenBuffers(bufferArray.size, bufferArray, 0)
        for (i in bufferArray.indices) {
            val buffer = TEXTURE_BUFFER_ARRAY[i]
            //顶点坐标 Buffer
            val vertexBuffer: FloatBuffer = ByteBuffer.allocateDirect(buffer.size * 4)
                .order(ByteOrder.nativeOrder())
                .asFloatBuffer()
                .put(buffer)
            vertexBuffer.position(0)
            GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER, bufferArray[i])
            GLES20.glBufferData(
                GLES20.GL_ARRAY_BUFFER,
                buffer.size * 4,
                vertexBuffer,
                GLES20.GL_STATIC_DRAW
            )
            GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER, 0)
        }

        val bitmap = loadBitmap()
        if (bitmap == null) {
            Log.w(TAG, "Load bitmap fail.")
            return
        }

        // texture
        val txtArr = IntArray(1)
        GLES20.glGenTextures(1, txtArr, 0)
        if (txtArr[0] <= 0) {
            Log.w(TAG, "Generate texture fail.")
            return
        }
        texture = txtArr[0]
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, texture)
        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_S, GLES20.GL_CLAMP_TO_EDGE)
        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_T, GLES20.GL_CLAMP_TO_EDGE)
        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MAG_FILTER, GLES20.GL_LINEAR)
        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MIN_FILTER, GLES20.GL_LINEAR)
        GLUtils.texImage2D(GLES20.GL_TEXTURE_2D, 0, bitmap, 0)
    }

    override fun onSurfaceChanged(gl: GL10, width: Int, height: Int) {
        GLES20.glViewport(0, 0, width, height)
    }

    override fun onDrawFrame(gl: GL10) {
        // clear
        GLES20.glColorMask(true, true, true, true)
        GLES20.glClearColor(1.0f, 1.0f, 1.0f, 1.0f)
        GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT or GLES20.GL_DEPTH_BUFFER_BIT)

        if(texture <= 0) return

        // use program
        GLES20.glUseProgram(program)

        GLES20.glEnableVertexAttribArray(avPosition)
        GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER, bufferArray[0])
        GLES20.glVertexAttribPointer(
            avPosition,
            GLES_COORDS_PER_VERTEX_COUNT,
            GLES20.GL_FLOAT,
            false,
            0,
            0
        )
        GLES20.glEnableVertexAttribArray(afPosition)
        GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER, bufferArray[1])
        GLES20.glVertexAttribPointer(
            afPosition,
            GLES_COORDS_PER_VERTEX_COUNT,
            GLES20.GL_FLOAT,
            false,
            0,
            0
        )
        GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER, 0)

        // bind texture
        GLES20.glActiveTexture(GLES20.GL_TEXTURE0)
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, texture)

        // Copy the model / view / projection matrix over.
        GLES20.glUniformMatrix4fv(uMVPMatrix, 1, false, MVP_MATRIX, 0)
        checkError("glUniformMatrix4fv")

        // Copy the texture transformation matrix over.
        GLES20.glUniformMatrix4fv(uTexMatrix, 1, false, TEXTURE_MATRIX, 0)
        checkError("glUniformMatrix4fv")

        // draw
        GLES20.glDrawArrays(GLES20.GL_TRIANGLE_STRIP, 0, GLES_VERTEX_COUNT)

        // unbind texture
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, 0)

        GLES20.glUseProgram(0)
    }

}