#ifdef ANDROID
#include <string.h>
#include <jni.h>
#include <android/log.h>
#include "JavaClassDefined.h"

// applicationGetEditFieldValue
// todo current text value
const char* applicationGetEditFieldValue();
void applicationSetEditFieldValue(const char *value);

jclass g_classNativeInterfaceEditTextController = NULL;

jmethodID g_showEditText = NULL;
jmethodID g_hideEditText = NULL;

const char *getJString(JNIEnv* env, jstring jstr);

extern JavaVM *g_javaVM;
JNIEnv* g_jniEnvEditText = NULL;

JNIEXPORT void JNICALL JNI_FUNCTION(NativeInterfaceEditTextController_initNative)(JNIEnv* env, jobject thiz)
{
	g_jniEnvEditText = env;

	jclass local = (*env)->FindClass(env, JNI_CLASSNAME(NativeInterfaceEditTextController));
	g_classNativeInterfaceEditTextController = (jclass)(*env)->NewGlobalRef(env, local);

	if (g_classNativeInterfaceEditTextController != NULL)
	{
		/*
		JNI signatures
		"(params)return"

		Z	boolean
		B	byte
		C	char
		S	short
		I	int
		J	long
		F	float
		D	double
		Lfully-qualified-class;		fully-qualified-class
		[ type	type[]

		For example, the Java method:
		long f (int n, String s, int[] arr);

		(ILjava/lang/String;[I)J

		With:
			I	-> int
			Ljava/lang/String;	-> string
			[I	-> array int

			J	-> return long
		*/
		(*g_javaVM)->AttachCurrentThread(g_javaVM, &g_jniEnvEditText, NULL);

		g_showEditText = (*g_jniEnvEditText)->GetStaticMethodID(g_jniEnvEditText, g_classNativeInterfaceEditTextController, "showEditText", "(Ljava/lang/String;IIII)V");
		g_hideEditText = (*g_jniEnvEditText)->GetStaticMethodID(g_jniEnvEditText, g_classNativeInterfaceEditTextController, "hideEditText", "()V");
	}
}

JNIEXPORT void JNICALL JNI_FUNCTION(NativeInterfaceEditTextController_setEditText)(JNIEnv* env, jobject thiz, jstring value)
{
	const char *valueString = getJString(env, value);

	applicationSetEditFieldValue(valueString);

	(*env)->ReleaseStringUTFChars(env, value, valueString);
}

// applicationHideEditFieldControl
// todo hide edit text
void applicationHideEditFieldControl()
{
	(*g_javaVM)->AttachCurrentThread(g_javaVM, &g_jniEnvEditText, NULL);

	if (g_hideEditText != NULL && g_classNativeInterfaceEditTextController != NULL)
	{
		__android_log_print(ANDROID_LOG_INFO, JNI_APPNAME, "call NativeInterfaceEditTextController::g_hideEditText");
		(*g_jniEnvEditText)->CallStaticVoidMethod(g_jniEnvEditText, g_classNativeInterfaceEditTextController, g_hideEditText);
	}
	else
	{
		__android_log_print(ANDROID_LOG_INFO, JNI_APPNAME, "can not call NativeInterfaceEditTextController::g_hideEditText");
	}
}

// applicationShowEditFieldControl
// todo show edit text
void applicationShowEditFieldControl(int x, int y, int w, int h)
{
	(*g_javaVM)->AttachCurrentThread(g_javaVM, &g_jniEnvEditText, NULL);

	if (g_showEditText != NULL && g_classNativeInterfaceEditTextController != NULL)
	{
		const char* editValue = applicationGetEditFieldValue();
		jstring jstringValue = (*g_jniEnvEditText)->NewStringUTF(g_jniEnvEditText, editValue);

		__android_log_print(ANDROID_LOG_INFO, JNI_APPNAME, "call NativeInterfaceEditTextController::showEditText");
		(*g_jniEnvEditText)->CallStaticVoidMethod(g_jniEnvEditText, g_classNativeInterfaceEditTextController, g_showEditText, jstringValue, x, y, w, h);

		(*g_jniEnvEditText)->DeleteLocalRef(g_jniEnvEditText, jstringValue);
	}
	else
	{
		__android_log_print(ANDROID_LOG_INFO, JNI_APPNAME, "can not call NativeInterfaceEditTextController::showEditText");
	}
}
#endif