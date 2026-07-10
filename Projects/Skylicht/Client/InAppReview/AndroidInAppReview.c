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

extern JNIEnv* skylichtGetJniEnv();

jclass g_classInAppReview = NULL;
jmethodID g_showInAppReview = NULL;

JNIEXPORT void JNICALL JNI_FUNCTION(InAppReview_init)(JNIEnv* env, jobject thiz)
{
	__android_log_print(ANDROID_LOG_INFO, JNI_APPNAME, "InAppReview Init");
	
	jclass local = (*env)->FindClass(env, JNI_CLASSNAME(InAppReview));
	g_classInAppReview = (jclass)(*env)->NewGlobalRef(env, local);

	g_showInAppReview = (*env)->GetStaticMethodID(env, g_classInAppReview, "showInAppReview", "(Z)V");
}

void androidInAppReview_show(int isTesting)
{
	JNIEnv* env = skylichtGetJniEnv();

	if (env != NULL && g_showInAppReview != NULL && g_classInAppReview != NULL)
	{
		(*env)->CallStaticVoidMethod(env, g_classInAppReview, g_showInAppReview, isTesting);
	}
}
#endif
