/*
!@
MIT License

Copyright (c) 2026 Skylicht Technology CO., LTD

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

jclass g_classPlayGamesAchievement = NULL;
jmethodID g_updateAchievement = NULL;
jmethodID g_showDefaultAchievementsUI = NULL;

JNIEXPORT void JNICALL JNI_FUNCTION(PlayGamesAchievement_init)(JNIEnv* env, jobject thiz)
{
	__android_log_print(ANDROID_LOG_INFO, JNI_APPNAME, "PlayGamesAchievement Init");
	
	jclass local = (*env)->FindClass(env, JNI_CLASSNAME(PlayGamesAchievement));
	g_classPlayGamesAchievement = (jclass)(*env)->NewGlobalRef(env, local);

	g_updateAchievement = (*env)->GetStaticMethodID(env, g_classPlayGamesAchievement, "updateAchievement", "(Ljava/lang/String;IF)V");
	g_showDefaultAchievementsUI = (*env)->GetStaticMethodID(env, g_classPlayGamesAchievement, "showDefaultAchievementsUI", "()V");
}

void playGamesAchievement_updateAchievement(const char* id, int step, float percent)
{
	if (id == NULL)
		return;

	JNIEnv* env = skylichtGetJniEnv();

	if (env != NULL && g_updateAchievement != NULL && g_classPlayGamesAchievement != NULL)
	{
		jstring jid = (*env)->NewStringUTF(env, id);
		(*env)->CallStaticVoidMethod(env, g_classPlayGamesAchievement, g_updateAchievement, jid, step, percent);
		(*env)->DeleteLocalRef(env, jid);
	}
}

void playGamesAchievement_showDefaultAchievementsUI()
{
	JNIEnv* env = skylichtGetJniEnv();

	if (env != NULL && g_showDefaultAchievementsUI != NULL && g_classPlayGamesAchievement != NULL)
	{
		(*env)->CallStaticVoidMethod(env, g_classPlayGamesAchievement, g_showDefaultAchievementsUI);
	}
}
#endif
