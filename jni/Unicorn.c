/*

Java bindings for the Unicorn Emulator Engine

Copyright(c) 2015 Chris Eagle

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
version 2 as published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

*/

#include <sys/types.h>
#include "unicorn/platform.h"
#include <stdlib.h>
#include <string.h>

#include <unicorn/unicorn.h>
#include <unicorn/arm64.h>
#include "Unicorn.h"

// cache jmethodID values as we look them up
static jmethodID invokeBlockCallbacks = 0;
static jmethodID invokeInterruptCallbacks = 0;
static jmethodID invokeCodeCallbacks = 0;

static jmethodID invokeEventMemCallbacks = 0;
static jmethodID invokeReadCallbacks = 0;
static jmethodID invokeWriteCallbacks = 0;

static JavaVM *cachedJVM;

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *jvm, void *reserved) {
  cachedJVM = jvm;
  return JNI_VERSION_1_6;
}

// Callback function for tracing code (UC_HOOK_CODE & UC_HOOK_BLOCK)
// @address: address where the code is being executed
// @size: size of machine instruction being executed
// @user_data: user data passed to tracing APIs.
static void cb_hook_code(uc_engine *eng, uint64_t address, uint32_t size, void *user_data) {
  JNIEnv *env;
  int stat = (*cachedJVM)->GetEnv(cachedJVM, (void **) &env, JNI_VERSION_1_6);
  bool isAttached = false;
  if (stat != JNI_OK) {
    if ((*cachedJVM)->AttachCurrentThread(cachedJVM, &env, NULL) == JNI_OK) {
      isAttached = true;
    }
  }
  jclass clz = (*env)->FindClass(env, "com/fuchg/emu/unicorn/Unicorn");
  if ((*env)->ExceptionCheck(env)) {
    (*env)->ExceptionDescribe(env);
    return;
  }
  (*env)->CallStaticVoidMethod(env, clz, invokeCodeCallbacks,
                               (jlong) eng, (jint) address, (jint) size);
  if (isAttached) {
    (*cachedJVM)->DetachCurrentThread(cachedJVM);
  }
}

// Callback function for tracing code (UC_HOOK_CODE & UC_HOOK_BLOCK)
// @address: address where the code is being executed
// @size: size of machine instruction being executed
// @user_data: user data passed to tracing APIs.
static void cb_hook_block(uc_engine *eng, uint64_t address, uint32_t size, void *user_data) {
  JNIEnv *env;
  int stat = (*cachedJVM)->GetEnv(cachedJVM, (void **) &env, JNI_VERSION_1_6);
  bool isAttached = false;
  if (stat != JNI_OK) {
    if ((*cachedJVM)->AttachCurrentThread(cachedJVM, &env, NULL) == JNI_OK) {
      isAttached = true;
    }
  }
  jclass clz = (*env)->FindClass(env, "com/fuchg/emu/unicorn/Unicorn");
  if ((*env)->ExceptionCheck(env)) {
    (*env)->ExceptionDescribe(env);
    return;
  }
  (*env)->CallStaticVoidMethod(env, clz, invokeBlockCallbacks,
                               (jlong) eng, (jint) address, (jint) size);
  if (isAttached) {
    (*cachedJVM)->DetachCurrentThread(cachedJVM);
  }
}

// Callback function for tracing interrupts (for uc_hook_intr())
// @intno: interrupt number
// @user_data: user data passed to tracing APIs.
static void cb_hook_interrupt(uc_engine *eng, uint32_t intno, void *user_data) {
  JNIEnv *env;
  int stat = (*cachedJVM)->GetEnv(cachedJVM, (void **) &env, JNI_VERSION_1_6);
  bool isAttached = false;
  if (stat != JNI_OK) {
    if ((*cachedJVM)->AttachCurrentThread(cachedJVM, &env, NULL) == JNI_OK) {
      isAttached = true;
    }
  }
  jclass clz = (*env)->FindClass(env, "com/fuchg/emu/unicorn/Unicorn");
  if ((*env)->ExceptionCheck(env)) {
    (*env)->ExceptionDescribe(env);
    return;
  }
  (*env)->CallStaticVoidMethod(env, clz, invokeInterruptCallbacks, (jlong) eng, (jint) intno);
  if (isAttached) {
    (*cachedJVM)->DetachCurrentThread(cachedJVM);
  }
}

// Callback function for hooking memory (UC_HOOK_MEM_*)
// @type: this memory is being READ, or WRITE
// @address: address where the code is being executed
// @size: size of data being read or written
// @value: value of data being written to memory, or irrelevant if type = READ.
// @user_data: user data passed to tracing APIs
static void cb_hook_mem(uc_engine *eng, uc_mem_type type, uint64_t address, int size,
                        int64_t value, void *user_data) {
  JNIEnv *env;
  int stat = (*cachedJVM)->GetEnv(cachedJVM, (void **) &env, JNI_VERSION_1_6);
  bool isAttached = false;
  if (stat != JNI_OK) {
    if ((*cachedJVM)->AttachCurrentThread(cachedJVM, &env, NULL) == JNI_OK) {
      isAttached = true;
    }
  }
  jclass clz = (*env)->FindClass(env, "com/fuchg/emu/unicorn/Unicorn");
  if ((*env)->ExceptionCheck(env)) {
    (*env)->ExceptionDescribe(env);
    return;
  }
  switch (type) {
    case UC_MEM_READ:
      (*env)->CallStaticVoidMethod(env, clz, invokeReadCallbacks,
                                   (jlong) eng, (jint) address, (jint) size);
      break;
    case UC_MEM_WRITE:
      (*env)->CallStaticVoidMethod(env, clz, invokeWriteCallbacks,
                                   (jlong) eng, (jint) address, (jint) size, (jint) value);
      break;
    default:break;
  }
  if (isAttached) {
    (*cachedJVM)->DetachCurrentThread(cachedJVM);
  }
}

// Callback function for handling memory events (for UC_HOOK_MEM_UNMAPPED)
// @type: this memory is being READ, or WRITE
// @address: address where the code is being executed
// @size: size of data being read or written
// @value: value of data being written to memory, or irrelevant if type = READ.
// @user_data: user data passed to tracing APIs
// @return: return true to continue, or false to stop program (due to invalid memory).
static bool cb_event_mem(uc_engine *eng, uc_mem_type type, uint64_t address, int size,
                         int64_t value, void *user_data) {
  JNIEnv *env;
  int stat = (*cachedJVM)->GetEnv(cachedJVM, (void **) &env, JNI_VERSION_1_6);
  bool isAttached = false;
  if (stat != JNI_OK) {
    if ((*cachedJVM)->AttachCurrentThread(cachedJVM, &env, NULL) == JNI_OK) {
      isAttached = true;
    }
  }
  jclass clz = (*env)->FindClass(env, "com/fuchg/emu/unicorn/Unicorn");
  if ((*env)->ExceptionCheck(env)) {
    (*env)->ExceptionDescribe(env);
    return false;
  }
  jboolean res = (*env)->CallStaticBooleanMethod(env, clz, invokeEventMemCallbacks, (jlong) eng,
                                                 (jint) type, (jint) address, (jint) size,
                                                 (jint) value);
  if (isAttached) {
    (*cachedJVM)->DetachCurrentThread(cachedJVM);
  }
  return res;
}

static void throwException(JNIEnv *env, uc_err err) {
  // throw exception
  jclass clazz = (*env)->FindClass(env, "com/fuchg/emu/unicorn/UnicornException");
  if (err != UC_ERR_OK) {
    const char *msg = uc_strerror(err);
    (*env)->ThrowNew(env, clazz, msg);
  }
}

static uc_engine *getEngine(JNIEnv *env, jobject self) {
  static int haveFid = 0;
  static jfieldID fid;
  if (haveFid == 0) {
    // cache the field id
    jclass clazz = (*env)->GetObjectClass(env, self);
    fid = (*env)->GetFieldID(env, clazz, "eng", "J");
    haveFid = 1;
  }
  return (uc_engine *) (*env)->GetLongField(env, self, fid);
}

/*
 * Class:     com_fuchg_emu_unicorn_Unicorn
 * Method:    regWriteNum
 * Signature: (ILjava/lang/Number;)V
 */
JNIEXPORT void JNICALL
Java_com_fuchg_emu_unicorn_Unicorn_regWriteNum(JNIEnv *env, jobject self,
                                               jint regid, jobject value) {
  uc_engine *eng = getEngine(env, self);

  jclass clz = (*env)->FindClass(env, "java/lang/Number");
  if ((*env)->ExceptionCheck(env)) {
    return;
  }

  jmethodID longValue = (*env)->GetMethodID(env, clz, "longValue", "()J");
  jlong longVal = (*env)->CallLongMethod(env, value, longValue);
  uc_err err = uc_reg_write(eng, regid, &longVal);
  if (err != UC_ERR_OK) {
    throwException(env, err);
  }
}

/*
 * Class:     com_fuchg_emu_unicorn_Unicorn
 * Method:    regReadNum
 * Signature: (I)Ljava/lang/Number;
 */
JNIEXPORT jobject JNICALL
Java_com_fuchg_emu_unicorn_Unicorn_regReadNum(JNIEnv *env, jobject self, jint regid) {
  uc_engine *eng = getEngine(env, self);

  jclass clz = (*env)->FindClass(env, "java/lang/Integer");
  if ((*env)->ExceptionCheck(env)) {
    return NULL;
  }

  jint intVal;
  uc_err err = uc_reg_read(eng, regid, &intVal);
  if (err != UC_ERR_OK) {
    throwException(env, err);
  }

  jmethodID cons = (*env)->GetMethodID(env, clz, "<init>", "(I)V");
  jobject result = (*env)->NewObject(env, clz, cons, intVal);
  if ((*env)->ExceptionCheck(env)) {
    return NULL;
  }
  return result;
}

/*
 * Class:     com_fuchg_emu_unicorn_Unicorn
 * Method:    open
 * Signature: (II)J
 */
JNIEXPORT jlong JNICALL
Java_com_fuchg_emu_unicorn_Unicorn_open(JNIEnv *env, jobject self, jint arch, jint mode) {
  uc_engine *eng = NULL;
  uc_err err = uc_open((uc_arch) arch, (uc_mode) mode, &eng);
  if (err != UC_ERR_OK) {
    throwException(env, err);
  }
  return (jlong) eng;
}

/*
 * Class:     com_fuchg_emu_unicorn_Unicorn
 * Method:    version
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_com_fuchg_emu_unicorn_Unicorn_version(JNIEnv *env, jclass clz) {
  return (jint) uc_version(NULL, NULL);
}

/*
 * Class:     com_fuchg_emu_unicorn_Unicorn
 * Method:    archSupported
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_com_fuchg_emu_unicorn_Unicorn_archSupported(JNIEnv *env, jclass clz, jint arch) {
  return (jboolean) (uc_arch_supported((uc_arch) arch) != 0);
}

/*
 * Class:     com_fuchg_emu_unicorn_Unicorn
 * Method:    close
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_com_fuchg_emu_unicorn_Unicorn_close(JNIEnv *env, jobject self) {
  uc_engine *eng = getEngine(env, self);
  uc_err err = uc_close(eng);
  if (err != UC_ERR_OK) {
    throwException(env, err);
  }
  // We also need to ReleaseByteArrayElements for any regions that were mapped with uc_mem_map_ptr
}

/*
 * Class:     com_fuchg_emu_unicorn_Unicorn
 * Method:    query
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_com_fuchg_emu_unicorn_Unicorn_query(JNIEnv *env, jobject self, jint type) {
  uc_engine *eng = getEngine(env, self);
  size_t result;
  uc_err err = uc_query(eng, (uc_query_type) type, &result);
  if (err != UC_ERR_OK) {
    throwException(env, err);
  }
  return (jint) result;
}

/*
 * Class:     com_fuchg_emu_unicorn_Unicorn
 * Method:    errno
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_com_fuchg_emu_unicorn_Unicorn_errno(JNIEnv *env, jobject self) {
  uc_engine *eng = getEngine(env, self);
  return (jint) uc_errno(eng);
}

/*
 * Class:     com_fuchg_emu_unicorn_Unicorn
 * Method:    strerror
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL
Java_com_fuchg_emu_unicorn_Unicorn_strerror(JNIEnv *env, jclass clz, jint code) {
  const char *err = uc_strerror((uc_err) code);
  jstring s = (*env)->NewStringUTF(env, err);
  return s;
}

/*
 * Class:     com_fuchg_emu_unicorn_Unicorn
 * Method:    memWrite
 * Signature: (I[B)V
 */
JNIEXPORT void JNICALL
Java_com_fuchg_emu_unicorn_Unicorn_memWrite(JNIEnv *env, jobject self,
                                            jint address, jbyteArray bytes) {
  uc_engine *eng = getEngine(env, self);
  jbyte *array = (*env)->GetByteArrayElements(env, bytes, NULL);
  jsize size = (*env)->GetArrayLength(env, bytes);
  uc_err err = uc_mem_write(eng, (uint64_t) address, array, (size_t) size);

  if (err != UC_ERR_OK) {
    throwException(env, err);
  }

  (*env)->ReleaseByteArrayElements(env, bytes, array, JNI_ABORT);
}

/*
 * Class:     com_fuchg_emu_unicorn_Unicorn
 * Method:    memRead
 * Signature: (II)[B
 */
JNIEXPORT jbyteArray JNICALL
Java_com_fuchg_emu_unicorn_Unicorn_memRead(JNIEnv *env, jobject self, jint address, jint size) {
  uc_engine *eng = getEngine(env, self);

  jbyteArray bytes = (*env)->NewByteArray(env, (jsize) size);
  jbyte *array = (*env)->GetByteArrayElements(env, bytes, NULL);
  uc_err err = uc_mem_read(eng, (uint64_t) address, array, (size_t) size);
  if (err != UC_ERR_OK) {
    throwException(env, err);
  }
  (*env)->ReleaseByteArrayElements(env, bytes, array, 0);
  return bytes;
}

/*
 * Class:     com_fuchg_emu_unicorn_Unicorn
 * Method:    startEmulation
 * Signature: (IIJJ)V
 */
JNIEXPORT void JNICALL
Java_com_fuchg_emu_unicorn_Unicorn_startEmulation(JNIEnv *env, jobject self,
                                                  jint begin, jint until,
                                                  jlong timeout, jlong count) {
  uc_engine *eng = getEngine(env, self);
  uc_err err = uc_emu_start(eng, (uint64_t) begin, (uint64_t) until,
                            (uint64_t) timeout, (size_t) count);
  if (err != UC_ERR_OK) {
    throwException(env, err);
  }
}

/*
 * Class:     com_fuchg_emu_unicorn_Unicorn
 * Method:    stopEmulation
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_com_fuchg_emu_unicorn_Unicorn_stopEmulation(JNIEnv *env, jobject self) {
  uc_engine *eng = getEngine(env, self);
  uc_err err = uc_emu_stop(eng);
  if (err != UC_ERR_OK) {
    throwException(env, err);
  }
}

/*
 * Class:     com_fuchg_emu_unicorn_Unicorn
 * Method:    registerHook
 * Signature: (JI)J
 */
JNIEXPORT jlong JNICALL
Java_com_fuchg_emu_unicorn_Unicorn_registerHook__JI(JNIEnv *env, jclass clz, jlong eng, jint type) {
  uc_hook hh = 0;
  uc_err err = UC_ERR_OK;
  switch (type) {
    case UC_HOOK_INTR:                // Hook all interrupt events
      if (invokeInterruptCallbacks == 0) {
        invokeInterruptCallbacks = (*env)->GetStaticMethodID(env, clz,
                                                             "invokeInterruptCallbacks", "(JI)V");
      }
      err = uc_hook_add((uc_engine *) eng, &hh, (uc_hook_type) type, cb_hook_interrupt, env, 1, 0);
      break;
    case UC_HOOK_MEM_FETCH_UNMAPPED:  // Hook for all invalid memory access events
    case UC_HOOK_MEM_READ_UNMAPPED:   // Hook for all invalid memory access events
    case UC_HOOK_MEM_WRITE_UNMAPPED:  // Hook for all invalid memory access events
    case UC_HOOK_MEM_FETCH_PROT:      // Hook for all invalid memory access events
    case UC_HOOK_MEM_READ_PROT:       // Hook for all invalid memory access events
    case UC_HOOK_MEM_WRITE_PROT:      // Hook for all invalid memory access events
      if (invokeEventMemCallbacks == 0) {
        invokeEventMemCallbacks = (*env)->GetStaticMethodID(env, clz,
                                                            "invokeEventMemCallbacks", "(JIIII)Z");
      }
      err = uc_hook_add((uc_engine *) eng, &hh, (uc_hook_type) type, cb_event_mem, env, 1, 0);
      break;
    default:break;
  }
  return (jlong) hh;
}

/*
 * Class:     com_fuchg_emu_unicorn_Unicorn
 * Method:    registerHook
 * Signature: (JIII)J
 */
JNIEXPORT jlong JNICALL
Java_com_fuchg_emu_unicorn_Unicorn_registerHook__JIII(JNIEnv *env, jclass clz, jlong eng,
                                                      jint type, jint arg1, jint arg2) {
  uc_hook hh = 0;
  uc_err err = UC_ERR_OK;
  switch (type) {
    case UC_HOOK_CODE: // Hook a range of code
      if (invokeCodeCallbacks == 0) {
        invokeCodeCallbacks = (*env)->GetStaticMethodID(env, clz, "invokeCodeCallbacks", "(JII)V");
      }
      err = uc_hook_add((uc_engine *) eng, &hh, (uc_hook_type) type, cb_hook_code, env, 1, 0,
                        arg1, arg2);
      break;
    case UC_HOOK_BLOCK: // Hook basic blocks
      if (invokeBlockCallbacks == 0) {
        invokeBlockCallbacks = (*env)->GetStaticMethodID(env, clz,
                                                         "invokeBlockCallbacks", "(JII)V");
      }
      err = uc_hook_add((uc_engine *) eng, &hh, (uc_hook_type) type, cb_hook_block, env, 1, 0,
                        arg1, arg2);
      break;
    case UC_HOOK_MEM_READ: // Hook all memory read events.
      if (invokeReadCallbacks == 0) {
        invokeReadCallbacks = (*env)->GetStaticMethodID(env, clz, "invokeReadCallbacks", "(JII)V");
      }
      err = uc_hook_add((uc_engine *) eng, &hh, (uc_hook_type) type, cb_hook_mem, env, 1, 0,
                        arg1, arg2);
      break;
    case UC_HOOK_MEM_WRITE: // Hook all memory write events.
      if (invokeWriteCallbacks == 0) {
        invokeWriteCallbacks = (*env)->GetStaticMethodID(env, clz,
                                                         "invokeWriteCallbacks", "(JIII)V");
      }
      err = uc_hook_add((uc_engine *) eng, &hh, (uc_hook_type) type, cb_hook_mem, env, 1, 0,
                        arg1, arg2);
      break;
    default:break;
  }
  return (jlong) hh;
}

/*
 * Class:     com_fuchg_emu_unicorn_Unicorn
 * Method:    deleteHook
 * Signature: (J)V
 */
JNIEXPORT void JNICALL
Java_com_fuchg_emu_unicorn_Unicorn_deleteHook(JNIEnv *env, jobject self, jlong hh) {
  uc_engine *eng = getEngine(env, self);

  //**** TODO remove hook from any internal hook tables as well

  uc_err err = uc_hook_del(eng, (uc_hook) hh);
  if (err != UC_ERR_OK) {
    throwException(env, err);
  }
}

/*
 * Class:     com_fuchg_emu_unicorn_Unicorn
 * Method:    memMap
 * Signature: (III)V
 */
JNIEXPORT void JNICALL
Java_com_fuchg_emu_unicorn_Unicorn_memMap(JNIEnv *env, jobject self,
                                          jint address, jint size, jint perms) {
  uc_engine *eng = getEngine(env, self);

  uc_err err = uc_mem_map(eng, (uint64_t) address, (size_t) size, (uint32_t) perms);
  if (err != UC_ERR_OK) {
    throwException(env, err);
  }
}

/*
 * Class:     com_fuchg_emu_unicorn_Unicorn
 * Method:    memMapPtr
 * Signature: (III[B)V
 */
JNIEXPORT void JNICALL
Java_com_fuchg_emu_unicorn_Unicorn_memMapPtr__III_3B(JNIEnv *env, jobject self,
                                                     jint address, jint size, jint perms,
                                                     jbyteArray block) {
  uc_engine *eng = getEngine(env, self);
  jbyte *array = (*env)->GetByteArrayElements(env, block, NULL);
  uc_err err = uc_mem_map_ptr(eng, (uint64_t) address, (size_t) size, (uint32_t) perms,
                              (void *) array);
  if (err != UC_ERR_OK) {
    throwException(env, err);
  }
  // Need to track address/block/array so that we can ReleaseByteArrayElements when the
  // block gets unmapped or when uc_close gets called
  // (*env)->ReleaseByteArrayElements(env, block, array, JNI_ABORT);
}

/*
 * Class:     com_fuchg_emu_unicorn_Unicorn
 * Method:    memUnmap
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_com_fuchg_emu_unicorn_Unicorn_memUnmap(JNIEnv *env, jobject self, jint address, jint size) {
  uc_engine *eng = getEngine(env, self);
  uc_err err = uc_mem_unmap(eng, (uint64_t) address, (size_t) size);
  if (err != UC_ERR_OK) {
    throwException(env, err);
  }
  // If a region was mapped using uc_mem_map_ptr, we also need to
  // ReleaseByteArrayElements for that region
}

/*
 * Class:     com_fuchg_emu_unicorn_Unicorn
 * Method:    memProtect
 * Signature: (III)V
 */
JNIEXPORT void JNICALL
Java_com_fuchg_emu_unicorn_Unicorn_memProtect(JNIEnv *env, jobject self,
                                              jint address, jint size, jint perms) {
  uc_engine *eng = getEngine(env, self);
  uc_err err = uc_mem_protect(eng, (uint64_t) address, (size_t) size, (uint32_t) perms);
  if (err != UC_ERR_OK) {
    throwException(env, err);
  }
}

/*
 * Class:     com_fuchg_emu_unicorn_Unicorn
 * Method:    memRegions
 * Signature: ()[Lcom/fuchg/emu/unicorn/MemRegion;
 */
JNIEXPORT jobjectArray JNICALL
Java_com_fuchg_emu_unicorn_Unicorn_memRegions(JNIEnv *env, jobject self) {
  uc_engine *eng = getEngine(env, self);

  uc_mem_region *regions = NULL;
  uint32_t count = 0;
  uint32_t i;

  uc_err err = uc_mem_regions(eng, &regions, &count);
  if (err != UC_ERR_OK) {
    throwException(env, err);
  }
  jclass clz = (*env)->FindClass(env, "com/fuchg/emu/unicorn/MemRegion");
  if ((*env)->ExceptionCheck(env)) {
    return NULL;
  }
  jobjectArray result = (*env)->NewObjectArray(env, (jsize) count, clz, NULL);
  jmethodID cons = (*env)->GetMethodID(env, clz, "<init>", "(III)V");
  for (i = 0; i < count; i++) {
    jobject mr = (*env)->NewObject(env, clz, cons, (jint) regions[i].begin, (jint) regions[i].end,
                                   (jint) regions[i].perms);
    (*env)->SetObjectArrayElement(env, result, (jsize) i, mr);
  }
  uc_free(regions);

  return result;
}

/*
 * Class:     com_fuchg_emu_unicorn_Unicorn
 * Method:    contextAlloc
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL
Java_com_fuchg_emu_unicorn_Unicorn_contextAlloc(JNIEnv *env, jobject self) {
  uc_engine *eng = getEngine(env, self);
  uc_context *ctx;
  uc_err err = uc_context_alloc(eng, &ctx);
  if (err != UC_ERR_OK) {
    throwException(env, err);
  }
  return (jlong) (uint64_t) ctx;
}

/*
 * Class:     com_fuchg_emu_unicorn_Unicorn
 * Method:    free
 * Signature: (J)V
 */
JNIEXPORT void JNICALL
Java_com_fuchg_emu_unicorn_Unicorn_free(JNIEnv *env, jobject self, jlong ctx) {
  uc_err err = uc_free((void *) ctx);
  if (err != UC_ERR_OK) {
    throwException(env, err);
  }
}

/*
 * Class:     com_fuchg_emu_unicorn_Unicorn
 * Method:    contextSave
 * Signature: (J)V
 */
JNIEXPORT void JNICALL
Java_com_fuchg_emu_unicorn_Unicorn_contextSave(JNIEnv *env, jobject self, jlong ctx) {
  uc_engine *eng = getEngine(env, self);
  uc_err err = uc_context_save(eng, (uc_context *) ctx);
  if (err != UC_ERR_OK) {
    throwException(env, err);
  }
}

/*
 * Class:     com_fuchg_emu_unicorn_Unicorn
 * Method:    contextRestore
 * Signature: (J)V
 */
JNIEXPORT void JNICALL
Java_com_fuchg_emu_unicorn_Unicorn_contextRestore(JNIEnv *env, jobject self, jlong ctx) {
  uc_engine *eng = getEngine(env, self);
  uc_err err = uc_context_restore(eng, (uc_context *) ctx);
  if (err != UC_ERR_OK) {
    throwException(env, err);
  }
}

JNIEXPORT void JNICALL
Java_com_fuchg_emu_unicorn_Unicorn_run(JNIEnv *env, jobject self, jlong timeout, jlong count) {
  uc_engine *eng = getEngine(env, self);
  uc_err err = uc_emu_run(eng, (uint64_t) timeout, (size_t) count);
  if (err != UC_ERR_OK) {
    throwException(env, err);
  }
}

JNIEXPORT jobject JNICALL
Java_com_fuchg_emu_unicorn_Unicorn_queryRegion(JNIEnv *env, jobject self, jint address) {
  uc_engine *eng = getEngine(env, self);

  uc_mem_region *region;
  uc_err err = uc_query_region(eng, (uint64_t) address, &region);
  if (err != UC_ERR_OK) {
    throwException(env, err);
  }
  if (region == NULL) {
    return NULL;
  }

  jclass clz = (*env)->FindClass(env, "com/fuchg/emu/unicorn/MemRegion");
  if ((*env)->ExceptionCheck(env)) {
    return NULL;
  }
  jmethodID constructor = (*env)->GetMethodID(env, clz, "<init>", "(III)V");
  jobject result = (*env)->NewObject(env, clz, constructor, (jint) region->begin,
                                     (jint) region->end, (jint) region->perms);
  uc_free(region);
  return result;
}

JNIEXPORT void JNICALL
Java_com_fuchg_emu_unicorn_Unicorn_memMapMirror(JNIEnv *env, jobject self,
                                                jint source, jint target, jint size, jint perms) {
  uc_engine *eng = getEngine(env, self);

  uc_err err = uc_mem_map_mirror(eng, (uint64_t) source, (uint64_t) target,
                                 (size_t) size, (uint32_t) perms);
  if (err != UC_ERR_OK) {
    throwException(env, err);
  }
}

JNIEXPORT void JNICALL
Java_com_fuchg_emu_unicorn_Unicorn_setContextRegister(JNIEnv *env, jobject self, jlong context, jint regid, jint value) {
  uc_engine *eng = getEngine(env, self);

  uc_err err = uc_set_context_reg(eng, (uc_context *) context, regid, &value);

  if (err != UC_ERR_OK) {
    throwException(env, err);
  }
}