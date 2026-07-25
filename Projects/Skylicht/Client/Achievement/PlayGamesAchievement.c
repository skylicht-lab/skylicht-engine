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
#include <stdlib.h>
#include <string.h>
#include <jni.h>
#include <android/log.h>
#include "JavaClassDefined.h"

extern JNIEnv* skylichtGetJniEnv();
extern const char *getJString(JNIEnv* env, jstring jstr);

extern void playGamesAchievement_onFetchData(const char** ids, int* unlocks, float* percents, int* currentSteps, int* totalSteps, int count);

jclass g_classPlayGamesAchievement = NULL;
jmethodID g_updateAchievement = NULL;
jmethodID g_fetchAchievement = NULL;
jmethodID g_showDefaultAchievementsUI = NULL;

JNIEXPORT void JNICALL JNI_FUNCTION(PlayGamesAchievement_init)(JNIEnv* env, jobject thiz)
{
	__android_log_print(ANDROID_LOG_INFO, JNI_APPNAME, "PlayGamesAchievement Init");
	
	jclass local = (*env)->FindClass(env, JNI_CLASSNAME(PlayGamesAchievement));
	g_classPlayGamesAchievement = (jclass)(*env)->NewGlobalRef(env, local);

	g_updateAchievement = (*env)->GetStaticMethodID(env, g_classPlayGamesAchievement, "updateAchievement", "(Ljava/lang/String;IF)V");
	g_fetchAchievement = (*env)->GetStaticMethodID(env, g_classPlayGamesAchievement, "fetch", "()V");
	g_showDefaultAchievementsUI = (*env)->GetStaticMethodID(env, g_classPlayGamesAchievement, "showDefaultAchievementsUI", "()V");
}

JNIEXPORT void JNICALL JNI_FUNCTION(PlayGamesAchievement_onFetchData)(JNIEnv* env, jobject thiz, jobjectArray ids, jintArray unlocks, jfloatArray percents, jintArray currentSteps, jintArray totalSteps)
{
	int count = (*env)->GetArrayLength(env, ids);
	const char** c_ids = (const char**)malloc(sizeof(char*) * count);
	int* c_unlocks = (*env)->GetIntArrayElements(env, unlocks, NULL);
	float* c_percents = (*env)->GetFloatArrayElements(env, percents, NULL);
	int* c_currentSteps = (*env)->GetIntArrayElements(env, currentSteps, NULL);
	int* c_totalSteps = (*env)->GetIntArrayElements(env, totalSteps, NULL);

	for (int i = 0; i < count; i++)
	{
		jstring j_id = (jstring)(*env)->GetObjectArrayElement(env, ids, i);
		c_ids[i] = getJString(env, j_id);
		(*env)->DeleteLocalRef(env, j_id);
	}

	playGamesAchievement_onFetchData(c_ids, c_unlocks, c_percents, c_currentSteps, c_totalSteps, count);

	for (int i = 0; i < count; i++)
	{
		jstring j_id = (jstring)(*env)->GetObjectArrayElement(env, ids, i);
		(*env)->ReleaseStringUTFChars(env, j_id, c_ids[i]);
		(*env)->DeleteLocalRef(env, j_id);
	}

	free(c_ids);
	(*env)->ReleaseIntArrayElements(env, unlocks, c_unlocks, 0);
	(*env)->ReleaseFloatArrayElements(env, percents, c_percents, 0);
	(*env)->ReleaseIntArrayElements(env, currentSteps, c_currentSteps, 0);
	(*env)->ReleaseIntArrayElements(env, totalSteps, c_totalSteps, 0);
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

void playGamesAchievement_fetch()
{
	JNIEnv* env = skylichtGetJniEnv();

	if (env != NULL && g_fetchAchievement != NULL && g_classPlayGamesAchievement != NULL)
	{
		(*env)->CallStaticVoidMethod(env, g_classPlayGamesAchievement, g_fetchAchievement);
	}
}
#endif
