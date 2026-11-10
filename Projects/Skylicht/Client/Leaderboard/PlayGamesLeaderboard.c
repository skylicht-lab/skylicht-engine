#ifdef ANDROID
#include <stdint.h>
#include <jni.h>
#include "JavaClassDefined.h"

extern JNIEnv* skylichtGetJniEnv();

static jclass s_classPlayGamesLeaderboard = NULL;
static jmethodID s_postScore = NULL;
static jmethodID s_showDefaultLeaderboardUI = NULL;

JNIEXPORT void JNICALL JNI_FUNCTION(PlayGamesLeaderboard_init)(JNIEnv* env, jobject thiz)
{
	if (s_classPlayGamesLeaderboard != NULL)
		return;

	jclass local = (*env)->FindClass(env, JNI_CLASSNAME(PlayGamesLeaderboard));
	if (local == NULL)
		return;
	s_classPlayGamesLeaderboard = (jclass)(*env)->NewGlobalRef(env, local);
	(*env)->DeleteLocalRef(env, local);
	if (s_classPlayGamesLeaderboard == NULL)
		return;

	s_postScore = (*env)->GetStaticMethodID(env, s_classPlayGamesLeaderboard, "postScore", "(Ljava/lang/String;J)V");
	s_showDefaultLeaderboardUI = (*env)->GetStaticMethodID(env, s_classPlayGamesLeaderboard, "showDefaultLeaderboardUI", "(Ljava/lang/String;)V");
}

void playGamesLeaderboard_postScore(const char* id, int64_t score)
{
	if (id == NULL || id[0] == '\0')
		return;

	JNIEnv* env = skylichtGetJniEnv();
	if (env == NULL || s_classPlayGamesLeaderboard == NULL || s_postScore == NULL)
		return;

	jstring jid = (*env)->NewStringUTF(env, id);
	if (jid == NULL)
		return;
	(*env)->CallStaticVoidMethod(env, s_classPlayGamesLeaderboard, s_postScore, jid, (jlong)score);
	(*env)->DeleteLocalRef(env, jid);
}

void playGamesLeaderboard_showDefaultLeaderboardUI(const char* id)
{
	JNIEnv* env = skylichtGetJniEnv();
	if (env == NULL || s_classPlayGamesLeaderboard == NULL || s_showDefaultLeaderboardUI == NULL)
		return;

	jstring jid = (*env)->NewStringUTF(env, id != NULL ? id : "");
	if (jid == NULL)
		return;
	(*env)->CallStaticVoidMethod(env, s_classPlayGamesLeaderboard, s_showDefaultLeaderboardUI, jid);
	(*env)->DeleteLocalRef(env, jid);
}
#endif
