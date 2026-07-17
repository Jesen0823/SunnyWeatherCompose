#include "util/LogUtil.h"
#include "GLRenderContext.h"
#include "jni.h"

#define NATIVE_RENDER_CLASS_NAME "com/jesen/dev/gllib/MineNativeRender"

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT void JNICALL
native_OnInit(JNIEnv *env, jobject thiz) {
    GLRenderContext::GetInstance();
}

extern "C"
JNIEXPORT void JNICALL
native_OnUnInit(JNIEnv *env, jobject thiz) {
    GLRenderContext::DestroyInstance();
}

extern "C"
JNIEXPORT void JNICALL
native_SetImageData(JNIEnv *env, jobject thiz,
                    jint format, jint width,
                    jint height, jbyteArray imageData) {
    int len = env->GetArrayLength(imageData);
    uint8_t *buf = new uint8_t[len];
    env->GetByteArrayRegion(imageData, 0, len, reinterpret_cast<jbyte *>(buf));
    GLRenderContext::GetInstance()->SetImageData(format, width, height, buf);
    delete[] buf;
    env->DeleteLocalRef(imageData);
}

extern "C"
JNIEXPORT void JNICALL
native_SetParamsInt(JNIEnv *env, jobject thiz,
                    jint param_type, jint value0, jint value1) {
    GLRenderContext::GetInstance()->SetParamsInt(param_type, value0, value1);
}

extern "C"
JNIEXPORT void JNICALL native_UpdateTransformMatrix
        (JNIEnv *env, jobject instance, jfloat rotateX, jfloat rotateY, jfloat scaleX,
         jfloat scaleY) {
    GLRenderContext::GetInstance()->UpdateTransformMatrix(rotateX, rotateY, scaleX, scaleY);
}

extern "C"
JNIEXPORT void JNICALL
native_OnSurfaceCreated(JNIEnv *env, jobject thiz) {
    GLRenderContext::GetInstance()->OnSurfaceCreated();
}

extern "C"
JNIEXPORT void JNICALL
native_OnSurfaceChanged(JNIEnv *env, jobject thiz,
                        jint width, jint height) {
    GLRenderContext::GetInstance()->OnSurfaceChanged(width, height);
}

extern "C"
JNIEXPORT void JNICALL
native_OnDrawFrame(JNIEnv *env, jobject thiz) {
    GLRenderContext::GetInstance()->OnDrawFrame();
}

extern "C"
JNIEXPORT void JNICALL
native_SetSkycon(JNIEnv *env, jobject thiz, jstring skycon) {
    LOGCATE("native_SetSkycon: JNI bridge called");
    if (skycon == nullptr) {
        LOGCATE("native_SetSkycon: skycon is null, returning");
        return;
    }
    const char *skyconStr = env->GetStringUTFChars(skycon, nullptr);
    LOGCATI("native_SetSkycon: skycon = %s", skyconStr);
    GLRenderContext::GetInstance()->SetSkycon(skyconStr);
    env->ReleaseStringUTFChars(skycon, skyconStr);
    LOGCATI("native_SetSkycon: completed");
}

#ifdef __cplusplus
}
#endif

static JNINativeMethod g_NativeMethods[] = {
        {"native_OnInit",                "()V",       (void *) (native_OnInit)},
        {"native_OnUnInit",              "()V",       (void *) (native_OnUnInit)},
        {"native_SetImageData",          "(III[B)V",  (void *) (native_SetImageData)},
        {"native_SetParamsInt",          "(III)V",    (void *) (native_SetParamsInt)},
        {"native_UpdateTransformMatrix", "(FFFF)V",   (void *) (native_UpdateTransformMatrix)},
        {"native_OnSurfaceCreated",      "()V",       (void *) (native_OnSurfaceCreated)},
        {"native_OnSurfaceChanged",      "(II)V",     (void *) (native_OnSurfaceChanged)},
        {"native_OnDrawFrame",           "()V",       (void *) (native_OnDrawFrame)},
        {"native_SetSkycon",             "(Ljava/lang/String;)V", (void *) (native_SetSkycon)}
};

static int
RegisterNativeMethods(JNIEnv *env, const char *className, JNINativeMethod *methods, int methodNum) {
    LOGCATE("====RegisterNativeMethods===");
    jclass clazz = env->FindClass(className);
    if (clazz == nullptr) {
        LOGCATE("RegisterNativeMethods fail. clazz == NULL");
        return JNI_FALSE;
    }
    if (env->RegisterNatives(clazz, methods, methodNum) < 0) {
        LOGCATE("RegisterNativeMethods fail");
        return JNI_FALSE;
    }
    return JNI_TRUE;
}

extern "C" jint JNI_OnLoad(JavaVM *jvm, void *p) {
    LOGCATE("======= JNI_OnLoad===");
    jint jniRet = JNI_ERR;
    JNIEnv *env = nullptr;
    if (jvm->GetEnv((void **) (&env), JNI_VERSION_1_6) != JNI_OK) {
        return jniRet;
    }
    jint regRet = RegisterNativeMethods(env, NATIVE_RENDER_CLASS_NAME, g_NativeMethods,
                                        sizeof(g_NativeMethods) /
                                        sizeof(g_NativeMethods[0]));
    if (regRet != JNI_TRUE) {
        return JNI_ERR;
    }

    return JNI_VERSION_1_6;
}