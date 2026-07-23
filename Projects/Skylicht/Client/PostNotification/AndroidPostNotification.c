#ifdef ANDROID
#include <jni.h>
#include <android/log.h>
#include "JavaClassDefined.h"

extern JNIEnv* skylichtGetJniEnv();
extern void android_onPostNotificationPermissionUpdate(int permission);

jclass g_classPostNotification = NULL;
jmethodID g_checkPostNotificationPermission = NULL;
jmethodID g_requestPostNotificationPermission = NULL;
jmethodID g_schedulePostNotification = NULL;
jmethodID g_removePostNotification = NULL;
jmethodID g_clearPostNotification = NULL;

JNIEXPORT void JNICALL JNI_FUNCTION(PostNotification_initNative)(JNIEnv* env, jobject thiz)
{
	__android_log_print(ANDROID_LOG_INFO, JNI_APPNAME, "PostNotification Init Native");

	jclass local = (*env)->FindClass(env, JNI_CLASSNAME(PostNotification));
	g_classPostNotification = (jclass)(*env)->NewGlobalRef(env, local);

	g_checkPostNotificationPermission = (*env)->GetStaticMethodID(env, g_classPostNotification, "nativeCheckPermission", "()I");
	g_requestPostNotificationPermission = (*env)->GetStaticMethodID(env, g_classPostNotification, "nativeRequestPermission", "()V");
	g_schedulePostNotification = (*env)->GetStaticMethodID(env, g_classPostNotification, "nativeScheduleNotification", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;J)V");
	g_removePostNotification = (*env)->GetStaticMethodID(env, g_classPostNotification, "nativeRemoveNotification", "(Ljava/lang/String;)V");
	g_clearPostNotification = (*env)->GetStaticMethodID(env, g_classPostNotification, "nativeClearNotification", "()V");
}

JNIEXPORT void JNICALL JNI_FUNCTION(PostNotification_onPermissionUpdate)(JNIEnv* env, jobject thiz, jint permission)
{
	android_onPostNotificationPermissionUpdate(permission);
}

int android_checkPostNotificationPermission()
{
	JNIEnv* env = skylichtGetJniEnv();
	if (env != NULL && g_checkPostNotificationPermission != NULL && g_classPostNotification != NULL)
		return (*env)->CallStaticIntMethod(env, g_classPostNotification, g_checkPostNotificationPermission);
	return 1;
}

void android_requestPostNotificationPermission()
{
	JNIEnv* env = skylichtGetJniEnv();
	if (env != NULL && g_requestPostNotificationPermission != NULL && g_classPostNotification != NULL)
		(*env)->CallStaticVoidMethod(env, g_classPostNotification, g_requestPostNotificationPermission);
}

void android_schedulePostNotification(const char* id, const char* title, const char* content, unsigned long delayFromNow)
{
	JNIEnv* env = skylichtGetJniEnv();
	if (env != NULL && g_schedulePostNotification != NULL && g_classPostNotification != NULL)
	{
		jstring jid = (*env)->NewStringUTF(env, id ? id : "");
		jstring jtitle = (*env)->NewStringUTF(env, title ? title : "");
		jstring jcontent = (*env)->NewStringUTF(env, content ? content : "");
		(*env)->CallStaticVoidMethod(env, g_classPostNotification, g_schedulePostNotification, jid, jtitle, jcontent, (jlong)delayFromNow);
		(*env)->DeleteLocalRef(env, jid);
		(*env)->DeleteLocalRef(env, jtitle);
		(*env)->DeleteLocalRef(env, jcontent);
	}
}

void android_removePostNotification(const char* id)
{
	JNIEnv* env = skylichtGetJniEnv();
	if (env != NULL && g_removePostNotification != NULL && g_classPostNotification != NULL)
	{
		jstring jid = (*env)->NewStringUTF(env, id ? id : "");
		(*env)->CallStaticVoidMethod(env, g_classPostNotification, g_removePostNotification, jid);
		(*env)->DeleteLocalRef(env, jid);
	}
}

void android_clearPostNotification()
{
	JNIEnv* env = skylichtGetJniEnv();
	if (env != NULL && g_clearPostNotification != NULL && g_classPostNotification != NULL)
		(*env)->CallStaticVoidMethod(env, g_classPostNotification, g_clearPostNotification);
}

#endif
