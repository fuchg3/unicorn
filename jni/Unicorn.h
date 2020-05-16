#include <jni.h>

#ifndef EMU_UNICORN_H
#define EMU_UNICORN_H

#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     com_fuchg_emu_unicorn_Unicorn
 * Method:    regWriteNum
 * Signature: (ILjava/lang/Number;)V
 */
JNIEXPORT void JNICALL
Java_com_fuchg_emu_unicorn_Unicorn_regWriteNum(JNIEnv *, jobject, jint, jobject);

/*
 * Class:     com_fuchg_emu_unicorn_Unicorn
 * Method:    regReadNum
 * Signature: (I)Ljava/lang/Number;
 */
JNIEXPORT jobject JNICALL
Java_com_fuchg_emu_unicorn_Unicorn_regReadNum(JNIEnv *, jobject, jint);

/*
 * Class:     com_fuchg_emu_unicorn_Unicorn
 * Method:    open
 */
JNIEXPORT jlong JNICALL
Java_com_fuchg_emu_unicorn_Unicorn_open(JNIEnv *, jobject, jint, jint);

/*
 * Class:     com_fuchg_emu_unicorn_Unicorn
 * Method:    version
 */
JNIEXPORT jint JNICALL
Java_com_fuchg_emu_unicorn_Unicorn_version(JNIEnv *, jclass);

/*
 * Class:     com_fuchg_emu_unicorn_Unicorn
 * Method:    archSupported
 */
JNIEXPORT jboolean JNICALL
Java_com_fuchg_emu_unicorn_Unicorn_archSupported(JNIEnv *, jclass, jint);

/*
 * Class:     com_fuchg_emu_unicorn_Unicorn
 * Method:    close
 */
JNIEXPORT void JNICALL
Java_com_fuchg_emu_unicorn_Unicorn_close(JNIEnv *, jobject);

/*
 * Class:     com_fuchg_emu_unicorn_Unicorn
 * Method:    query
 */
JNIEXPORT jint JNICALL
Java_com_fuchg_emu_unicorn_Unicorn_query(JNIEnv *, jobject, jint);

/*
 * Class:     com_fuchg_emu_unicorn_Unicorn
 * Method:    errno
 */
JNIEXPORT jint JNICALL
Java_com_fuchg_emu_unicorn_Unicorn_errno(JNIEnv *, jobject);

/*
 * Class:     com_fuchg_emu_unicorn_Unicorn
 * Method:    strerror
 */
JNIEXPORT jstring JNICALL
Java_com_fuchg_emu_unicorn_Unicorn_strerror(JNIEnv *, jclass, jint);

/*
 * Class:     com_fuchg_emu_unicorn_Unicorn
 * Method:    memWrite
 * Signature: (I[B)V
 */
JNIEXPORT void JNICALL
Java_com_fuchg_emu_unicorn_Unicorn_memWrite(JNIEnv *, jobject, jint, jbyteArray);

/*
 * Class:     com_fuchg_emu_unicorn_Unicorn
 * Method:    memRead
 * Signature: (II)[B
 */
JNIEXPORT jbyteArray JNICALL
Java_com_fuchg_emu_unicorn_Unicorn_memRead(JNIEnv *, jobject, jint, jint);

/*
 * Class:     com_fuchg_emu_unicorn_Unicorn
 * Method:    startEmulation
 * Signature: (IIJJ)V
 */
JNIEXPORT void JNICALL
Java_com_fuchg_emu_unicorn_Unicorn_startEmulation(JNIEnv *, jobject, jint, jint, jlong, jlong);

/*
 * Class:     com_fuchg_emu_unicorn_Unicorn
 * Method:    stopEmulation
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_com_fuchg_emu_unicorn_Unicorn_stopEmulation(JNIEnv *, jobject);

/*
 * Class:     com_fuchg_emu_unicorn_Unicorn
 * Method:    registerHook
 * Signature: (JI)J
 */
JNIEXPORT jlong JNICALL
Java_com_fuchg_emu_unicorn_Unicorn_registerHook__JI(JNIEnv *, jclass, jlong, jint);

/*
 * Class:     com_fuchg_emu_unicorn_Unicorn
 * Method:    registerHook
 * Signature: (JIII)J
 */
JNIEXPORT jlong JNICALL
Java_com_fuchg_emu_unicorn_Unicorn_registerHook__JIII(JNIEnv *, jclass, jlong, jint, jint, jint);

/*
 * Class:     com_fuchg_emu_unicorn_Unicorn
 * Method:    deleteHook
 * Signature: (J)V
 */
JNIEXPORT void JNICALL
Java_com_fuchg_emu_unicorn_Unicorn_deleteHook(JNIEnv *, jobject, jlong);

/*
 * Class:     com_fuchg_emu_unicorn_Unicorn
 * Method:    memMap
 * Signature: (III)V
 */
JNIEXPORT void JNICALL
Java_com_fuchg_emu_unicorn_Unicorn_memMap(JNIEnv *, jobject, jint, jint, jint);

/*
 * Class:     com_fuchg_emu_unicorn_Unicorn
 * Method:    memMapPtr
 * Signature: (III[B)V
 */
JNIEXPORT void JNICALL
Java_com_fuchg_emu_unicorn_Unicorn_memMapPtr__III_3B(JNIEnv *,
                                                     jobject,
                                                     jint,
                                                     jint,
                                                     jint,
                                                     jbyteArray);

/*
 * Class:     com_fuchg_emu_unicorn_Unicorn
 * Method:    memUnmap
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_com_fuchg_emu_unicorn_Unicorn_memUnmap(JNIEnv *, jobject, jint, jint);

/*
 * Class:     com_fuchg_emu_unicorn_Unicorn
 * Method:    memProtect
 * Signature: (III)V
 */
JNIEXPORT void JNICALL
Java_com_fuchg_emu_unicorn_Unicorn_memProtect(JNIEnv *, jobject, jint, jint, jint);

/*
 * Class:     com_fuchg_emu_unicorn_Unicorn
 * Method:    memRegions
 * Signature: ()[Lcom/fuchg/emu/unicorn/MemRegion;
 */
JNIEXPORT jobjectArray JNICALL
Java_com_fuchg_emu_unicorn_Unicorn_memRegions(JNIEnv *, jobject);

/*
 * Class:     com_fuchg_emu_unicorn_Unicorn
 * Method:    contextAlloc
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL
Java_com_fuchg_emu_unicorn_Unicorn_contextAlloc(JNIEnv *, jobject);

/*
 * Class:     com_fuchg_emu_unicorn_Unicorn
 * Method:    free
 */
JNIEXPORT void JNICALL
Java_com_fuchg_emu_unicorn_Unicorn_free(JNIEnv *, jobject, jlong);

/*
 * Class:     com_fuchg_emu_unicorn_Unicorn
 * Method:    contextSave
 */
JNIEXPORT void JNICALL
Java_com_fuchg_emu_unicorn_Unicorn_contextSave(JNIEnv *, jobject, jlong);

/*
 * Class:     com_fuchg_emu_unicorn_Unicorn
 * Method:    contextRestore
 */
JNIEXPORT void JNICALL
Java_com_fuchg_emu_unicorn_Unicorn_contextRestore(JNIEnv *, jobject, jlong);

JNIEXPORT void JNICALL
Java_com_fuchg_emu_unicorn_Unicorn_run(JNIEnv *, jobject, jlong, jlong);

JNIEXPORT jobject JNICALL
Java_com_fuchg_emu_unicorn_Unicorn_queryRegion(JNIEnv *env, jobject self, jint address);

JNIEXPORT void JNICALL
Java_com_fuchg_emu_unicorn_Unicorn_memMapMirror(JNIEnv *env, jobject, jint, jint, jint, jint);

JNIEXPORT void JNICALL
Java_com_fuchg_emu_unicorn_Unicorn_setContextRegister(JNIEnv *env, jobject, jlong, jint, jint);

#ifdef __cplusplus
}
#endif
#endif //EMU_UNICORN_H