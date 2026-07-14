package com.jesen.dev.gllib

class NativeLib {

    /**
     * A native method that is implemented by the 'gllib' native library,
     * which is packaged with this application.
     */
    external fun stringFromJNI(): String

    companion object {
        // Used to load the 'gllib' library on application startup.
        init {
            System.loadLibrary("gllib")
        }
    }
}