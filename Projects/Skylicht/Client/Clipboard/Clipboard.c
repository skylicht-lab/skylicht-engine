/*
!@
MIT License

Copyright (c) 2024 Skylicht Technology CO., LTD

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

extern const char *getJString(JNIEnv* env, jstring jstr);

extern JavaVM *g_javaVM;
extern JNIEnv* g_jniEnv;

jclass g_classClipboard = NULL;
jmethodID g_copyToClipboard = NULL;
jmethodID g_getTextFromClipboard = NULL;

JNIEXPORT void JNICALL JNI_FUNCTION(Clipboard_init)(JNIEnv* env, jobject thiz)
{
	__android_log_print(ANDROID_LOG_INFO, JNI_APPNAME, "Clipboard Init");

	jclass local = (*env)->FindClass(env, JNI_CLASSNAME(Clipboard));
	g_classClipboard = (jclass)(*env)->NewGlobalRef(env, local);

	g_copyToClipboard = (*env)->GetStaticMethodID(env, g_classClipboard, "copyToClipboard", "(Ljava/lang/String;)V");
	g_getTextFromClipboard = (*env)->GetStaticMethodID(env, g_classClipboard, "getTextFromClipboard", "()Ljava/lang/String;");
}

void clipboard_copy(const char* text)
{
	if (g_copyToClipboard != NULL && g_classClipboard != NULL)
	{
		(*g_javaVM)->AttachCurrentThread(g_javaVM, &g_jniEnv, NULL);
		jstring jniText = (*g_jniEnv)->NewStringUTF(g_jniEnv, text);
		
		(*g_jniEnv)->CallStaticVoidMethod(g_jniEnv, 
			g_classClipboard, 
			g_copyToClipboard,
			jniText);
		
		(*g_jniEnv)->DeleteLocalRef(g_jniEnv, jniText);
	}
}

const char* clipboard_paste()
{
	static char s_clipboardBuffer[4096];
	s_clipboardBuffer[0] = 0;

	if (g_getTextFromClipboard != NULL && g_classClipboard != NULL)
	{
		(*g_javaVM)->AttachCurrentThread(g_javaVM, &g_jniEnv, NULL);
		jstring jstr = (jstring)(*g_jniEnv)->CallStaticObjectMethod(g_jniEnv, 
			g_classClipboard, 
			g_getTextFromClipboard);

		if (jstr != NULL)
		{
			const char *ctext = getJString(g_jniEnv, jstr);
			strncpy(s_clipboardBuffer, ctext, 4095);
			s_clipboardBuffer[4095] = 0;
			(*g_jniEnv)->ReleaseStringUTFChars(g_jniEnv, jstr, ctext);
			(*g_jniEnv)->DeleteLocalRef(g_jniEnv, jstr);
		}
	}
	return s_clipboardBuffer;
}
#endif
