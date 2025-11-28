/*
!@
MIT License

Copyright (c) 2025 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify,
merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

This file is part of the "Skylicht Engine".
https://github.com/skylicht-lab/skylicht-engine
!#
*/

#ifdef ANDROID
#include <string.h>
#include <jni.h>
#include <android/log.h>
#include "JavaClassDefined.h"

extern void playGamesSignIn_onSignInSuccess(const char* id, const char* name, const char* code);
extern void playGamesSignIn_onSignInFailed(const char* message);

extern const char *getJString(JNIEnv* env, jstring jstr);

extern JavaVM *g_javaVM;
extern JNIEnv* g_jniEnv;

jclass g_classPlayGamesSignIn = NULL;
jmethodID g_signIn;

JNIEXPORT void JNICALL JNI_FUNCTION(PlayGamesSignIn_init)(JNIEnv* env, jobject thiz)
{
	__android_log_print(ANDROID_LOG_INFO, JNI_APPNAME, "PlayGamesSignIn Init");
	
	jclass local = (*env)->FindClass(env, JNI_CLASSNAME(PlayGamesSignIn));
	g_classPlayGamesSignIn = (jclass)(*env)->NewGlobalRef(env, local);

	g_signIn = (*env)->GetStaticMethodID(env, g_classPlayGamesSignIn, "signIn", "()V");
}

JNIEXPORT void JNICALL JNI_FUNCTION(PlayGamesSignIn_onSignInSuccess)(JNIEnv* env, jobject thiz, jstring id, jstring name, jstring authCode)
{
	const char *cid = getJString(env, id);
	const char *cname = getJString(env, name);
	const char *cauthcode = getJString(env, authCode);
	playGamesSignIn_onSignInSuccess(cid, cname, cauthcode);
	(*env)->ReleaseStringUTFChars(env, id, cid);
	(*env)->ReleaseStringUTFChars(env, name, cname);
	(*env)->ReleaseStringUTFChars(env, authCode, cauthcode);
}

JNIEXPORT void JNICALL JNI_FUNCTION(PlayGamesSignIn_onSignInFailed)(JNIEnv* env, jobject thiz, jstring msg)
{
	const char *cmsg = getJString(env, msg);
	playGamesSignIn_onSignInFailed(cmsg);
	(*env)->ReleaseStringUTFChars(env, msg, cmsg);
}

void playGamesSignIn_signIn()
{
	(*g_javaVM)->AttachCurrentThread(g_javaVM, &g_jniEnv, NULL);

	if (g_signIn != NULL && g_classPlayGamesSignIn != NULL)
	{
		(*g_jniEnv)->CallStaticVoidMethod(g_jniEnv, g_classPlayGamesSignIn, g_signIn);
	}
}
#endif