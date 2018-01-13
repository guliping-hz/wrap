﻿/****************************************************************************
Copyright (c) 2010-2012 Wrap-x.org
Copyright (c) 2013-2017 Chukong Technologies Inc.

http://www.Wrap-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/

#include "../wrap/wrap_config.h"

#ifndef COCOS_PROJECT

#ifndef __CUSTOM_JNI_HELPER_H__
#define __CUSTOM_JNI_HELPER_H__

#include <jni.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <map>

typedef std::map<unsigned int, JNIEnv *> MAPINTJNIENV;

namespace Wrap {

    typedef struct JniMethodInfo_ {
        JNIEnv *env;
        jclass classID;
        jmethodID methodID;
    } JniMethodInfo;

    class JniHelper {
    public:
        static std::string getStringUTFCharsJNI(JNIEnv *env, jstring srcjStr, bool *ret = nullptr);

        static jstring
        newStringUTFJNI(JNIEnv *env, const std::string &utf8Str, bool *ret = nullptr);

        static void setJavaVM(JavaVM *javaVM);

        static JavaVM *getJavaVM();

        static bool attachCurThread(unsigned int threadid);

        static bool detachCurThread(unsigned int threadid);

        static bool getJniByThreadID(unsigned int threadid, JNIEnv **penv);

        static JNIEnv *getEnv();

        static jobject getAndroidContext();

        static bool setClassLoaderFrom(JNIEnv *env, jobject activityinstance);

        static bool getStaticMethodInfo(JniMethodInfo &methodinfo, unsigned int threadId,
                                        const char *className,
                                        const char *methodName,
                                        const char *paramCode);

        static bool getMethodInfo(JniMethodInfo &methodinfo,
                                  const char *className,
                                  const char *methodName,
                                  const char *paramCode);

        static std::string jstring2string(jstring jstr);

        static std::string jstring2string(JNIEnv *env, jstring jstr);

        static jmethodID loadclassMethod_methodID;
        static jobject classloader;
        static std::function<void()> classloaderCallback;

        template<typename... Ts>
        static void callStaticVoidMethod(unsigned int threadId, const std::string &className,
                                         const std::string &methodName,
                                         Ts... xs) {
            Wrap::JniMethodInfo t;
            std::string signature = "(" + std::string(getJNISignature(xs...)) + ")V";
            if (Wrap::JniHelper::getStaticMethodInfo(t, threadId, className.c_str(),
                                                        methodName.c_str(),
                                                        signature.c_str())) {
                //LOGD("callStaticVoidMethod 1 env = %x", t.env);
                t.env->CallStaticVoidMethod(t.classID, t.methodID, convert(t, xs)...);
                //LOGD("callStaticVoidMethod 1 env = %x", t.env);
                t.env->DeleteLocalRef(t.classID);
                deleteLocalRefs(t.env);
            } else {
                reportError(className, methodName, signature);
            }
        }
//
//        template<typename... Ts>
//        static bool callStaticBooleanMethod(const std::string &className,
//                                            const std::string &methodName,
//                                            Ts... xs) {
//            jboolean jret = JNI_FALSE;
//            Wrap::JniMethodInfo t;
//            std::string signature = "(" + std::string(getJNISignature(xs...)) + ")Z";
//            if (Wrap::JniHelper::getStaticMethodInfo(t, className.c_str(), methodName.c_str(),
//                                                        signature.c_str())) {
//                jret = t.env->CallStaticBooleanMethod(t.classID, t.methodID, convert(t, xs)...);
//                t.env->DeleteLocalRef(t.classID);
//                deleteLocalRefs(t.env);
//            } else {
//                reportError(className, methodName, signature);
//            }
//            return (jret == JNI_TRUE);
//        }
//
//        template<typename... Ts>
//        static int callStaticIntMethod(const std::string &className,
//                                       const std::string &methodName,
//                                       Ts... xs) {
//            jint ret = 0;
//            Wrap::JniMethodInfo t;
//            std::string signature = "(" + std::string(getJNISignature(xs...)) + ")I";
//            if (Wrap::JniHelper::getStaticMethodInfo(t, className.c_str(), methodName.c_str(),
//                                                        signature.c_str())) {
//                ret = t.env->CallStaticIntMethod(t.classID, t.methodID, convert(t, xs)...);
//                t.env->DeleteLocalRef(t.classID);
//                deleteLocalRefs(t.env);
//            } else {
//                reportError(className, methodName, signature);
//            }
//            return ret;
//        }
//
//        template<typename... Ts>
//        static float callStaticFloatMethod(const std::string &className,
//                                           const std::string &methodName,
//                                           Ts... xs) {
//            jfloat ret = 0.0;
//            Wrap::JniMethodInfo t;
//            std::string signature = "(" + std::string(getJNISignature(xs...)) + ")F";
//            if (Wrap::JniHelper::getStaticMethodInfo(t, className.c_str(), methodName.c_str(),
//                                                        signature.c_str())) {
//                ret = t.env->CallStaticFloatMethod(t.classID, t.methodID, convert(t, xs)...);
//                t.env->DeleteLocalRef(t.classID);
//                deleteLocalRefs(t.env);
//            } else {
//                reportError(className, methodName, signature);
//            }
//            return ret;
//        }
//
//        template<typename... Ts>
//        static float *callStaticFloatArrayMethod(const std::string &className,
//                                                 const std::string &methodName,
//                                                 Ts... xs) {
//            static float ret[32];
//            Wrap::JniMethodInfo t;
//            std::string signature = "(" + std::string(getJNISignature(xs...)) + ")[F";
//            if (Wrap::JniHelper::getStaticMethodInfo(t, className.c_str(), methodName.c_str(),
//                                                        signature.c_str())) {
//                jfloatArray array = (jfloatArray) t.env->CallStaticObjectMethod(t.classID,
//                                                                                t.methodID,
//                                                                                convert(t, xs)...);
//                jsize len = t.env->GetArrayLength(array);
//                if (len <= 32) {
//                    jfloat *elems = t.env->GetFloatArrayElements(array, 0);
//                    if (elems) {
//                        memcpy(ret, elems, sizeof(float) * len);
//                        t.env->ReleaseFloatArrayElements(array, elems, 0);
//                    };
//                }
//                t.env->DeleteLocalRef(t.classID);
//                deleteLocalRefs(t.env);
//                return &ret[0];
//            } else {
//                reportError(className, methodName, signature);
//            }
//            return nullptr;
//        }
//
//        template<typename... Ts>
//        static double callStaticDoubleMethod(const std::string &className,
//                                             const std::string &methodName,
//                                             Ts... xs) {
//            jdouble ret = 0.0;
//            Wrap::JniMethodInfo t;
//            std::string signature = "(" + std::string(getJNISignature(xs...)) + ")D";
//            if (Wrap::JniHelper::getStaticMethodInfo(t, className.c_str(), methodName.c_str(),
//                                                        signature.c_str())) {
//                ret = t.env->CallStaticDoubleMethod(t.classID, t.methodID, convert(t, xs)...);
//                t.env->DeleteLocalRef(t.classID);
//                deleteLocalRefs(t.env);
//            } else {
//                reportError(className, methodName, signature);
//            }
//            return ret;
//        }

        template<typename... Ts>
        static std::string
        callStaticStringMethod(unsigned int threadId, const std::string &className,
                               const std::string &methodName,
                               Ts... xs) {

            std::string ret;

            Wrap::JniMethodInfo t;
            std::string signature =
                    "(" + std::string(getJNISignature(xs...)) + ")Ljava/lang/String;";
            if (Wrap::JniHelper::getStaticMethodInfo(t, threadId, className.c_str(),
                                                        methodName.c_str(),
                                                        signature.c_str())) {
                //LOGD("callStaticStringMethod 1 env = %x", t.env);
                jstring jret = (jstring) t.env->CallStaticObjectMethod(t.classID, t.methodID,
                                                                       convert(t, xs)...);

                //LOGD("callStaticStringMethod 2 env = %x", t.env);
                ret = Wrap::JniHelper::jstring2string(t.env, jret);
                t.env->DeleteLocalRef(t.classID);
                t.env->DeleteLocalRef(jret);
                deleteLocalRefs(t.env);
            } else {
                reportError(className, methodName, signature);
            }
            return ret;
        }

    private:
        static bool getMethodInfo_DefaultClassLoader(JNIEnv *env, JniMethodInfo &methodinfo,
                                                     const char *className,
                                                     const char *methodName,
                                                     const char *paramCode);

        static JavaVM *_psJavaVM;

        static jobject _context;

        static jstring convert(JniMethodInfo &t, const char *x);

        static jstring convert(JniMethodInfo &t, const std::string &x);

        template<typename T>
        static T convert(JniMethodInfo &, T x) {
            return x;
        }

        static std::unordered_map<JNIEnv *, std::vector<jobject>> localRefs;

        static void deleteLocalRefs(JNIEnv *env);

        static std::string getJNISignature() {
            return "";
        }

        static std::string getJNISignature(bool) {
            return "Z";
        }

        static std::string getJNISignature(char) {
            return "C";
        }

        static std::string getJNISignature(short) {
            return "S";
        }

        static std::string getJNISignature(int) {
            return "I";
        }

        static std::string getJNISignature(long) {
            return "J";
        }

        static std::string getJNISignature(float) {
            return "F";
        }

        static std::string getJNISignature(double) {
            return "D";
        }

        static std::string getJNISignature(const char *) {
            return "Ljava/lang/String;";
        }

        static std::string getJNISignature(const std::string &) {
            return "Ljava/lang/String;";
        }

        template<typename T>
        static std::string getJNISignature(T x) {
            // This template should never be instantiated
            static_assert(sizeof(x) == 0, "Unsupported argument type");
            return "";
        }

        template<typename T, typename... Ts>
        static std::string getJNISignature(T x, Ts... xs) {
            return getJNISignature(x) + getJNISignature(xs...);
        }

        static void reportError(const std::string &className, const std::string &methodName,
                                const std::string &signature);

    private:
        static MAPINTJNIENV m_mapEnv;
    };

}

#endif // __CUSTOM_JNI_HELPER_H__

#endif//COCOS_PROJECT
