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

extern void textfield_on_change(const char* text);
extern void textfield_on_done(const char* text);

extern const char *getJString(JNIEnv* env, jstring jstr);

extern JavaVM *g_javaVM;
extern JNIEnv* g_jniEnv;

jclass g_classTextField = NULL;
jmethodID g_show;

JNIEXPORT void JNICALL JNI_FUNCTION(TextField_init)(JNIEnv* env, jobject thiz)
{
	__android_log_print(ANDROID_LOG_INFO, JNI_APPNAME, "TextField Init");
	
	jclass local = (*env)->FindClass(env, JNI_CLASSNAME(TextField));
	g_classTextField = (jclass)(*env)->NewGlobalRef(env, local);

	g_show = (*env)->GetStaticMethodID(env, g_classTextField, "show", "(Ljava/lang/String;II)V");
}

JNIEXPORT void JNICALL JNI_FUNCTION(TextField_onChange)(JNIEnv* env, jobject thiz, jstring text)
{
	const char *ctext = getJString(env, text);
	textfield_on_change(ctext);
	(*env)->ReleaseStringUTFChars(env, text, ctext);
}

JNIEXPORT void JNICALL JNI_FUNCTION(TextField_onDone)(JNIEnv* env, jobject thiz, jstring text)
{
	const char *ctext = getJString(env, text);
	textfield_on_done(ctext);
	(*env)->ReleaseStringUTFChars(env, text, ctext);
}

void textfield_show(const char* text, int maxLength, int height)
{
	if (g_show != NULL && g_classTextField != NULL)
	{
		jstring jniText = (*g_jniEnv)->NewStringUTF(g_jniEnv, text);
		
		(*g_javaVM)->AttachCurrentThread(g_javaVM, &g_jniEnv, NULL);
		(*g_jniEnv)->CallStaticVoidMethod(g_jniEnv, 
			g_classTextField, 
			g_show,
			jniText,
			maxLength,
			height);
		
		(*g_jniEnv)->DeleteLocalRef(g_jniEnv, jniText);
	}
}
#endif