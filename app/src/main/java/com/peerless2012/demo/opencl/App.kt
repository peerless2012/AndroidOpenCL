package com.peerless2012.demo.opencl

import android.app.Application
import com.peerless2012.demo.opencl.jni.OpenCL4J

/**
 * @Author peerless2012
 * @Email peerless2012@126.com
 * @DateTime 2023/2/14 14:49
 * @Version V1.0
 * @Description:
 */
class App: Application() {

    override fun onCreate() {
        super.onCreate()
        OpenCL4J.load()
    }

}