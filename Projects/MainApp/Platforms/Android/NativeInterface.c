#ifdef ANDROID
#include <string.h>
#include <jni.h>
#include <android/log.h>
#include "JavaClassDefined.h"

void applicationInitApp(int w, int h);
void applicationLoop();
void applicationExitApp();
void applicationRelease();
void applicationPauseApp();
void applicationResumeApp(int showConnecting);
void applicationResizeWindow(int w, int h);
void applicationTouchDown(int touchID, int x, int y);
void applicationTouchMove(int touchID, int x, int y);
void applicationTouchUp(int touchID, int x, int y);
void applicationUpdateAccelerometer(float x, float y, float z);
void applicationSetAccelerometer(int b);
void applicationSetAPK(const char *path);
void applicationSetObbPath(const char *obbPath);
void applicationSetBundleID(const char *id);
void applicationSetSaveFolder(const char *path);
void applicationSetDownloadFolder(const char *path);
void applicationSetDeviceID(const char *id);
void applicationSetCPID(const char *cpid);
void applicationSetAndroidDeviceInfo(const char *manu, const char *model, const char *os);
int applicationOnBack();

JavaVM *g_javaVM = NULL;
JNIEnv* g_jniEnv = NULL;

jclass g_classNativeInterface = NULL;

jmethodID g_quitApplication = NULL;
jmethodID g_openURL = NULL;
jmethodID g_isNetworkAvailable = NULL;
jmethodID g_systemGC = NULL;

const char *getJString(JNIEnv* env, jstring jstr)
{
	if (jstr == NULL)
		return "";

	const char* chars = (*env)->GetStringUTFChars(env, jstr, NULL);
	return chars;
}

jint JNI_OnLoad(JavaVM *vm, void *reserved) {
	g_javaVM = vm;
	return JNI_VERSION_1_4;
}

void JNICALL JNI_OnUnload(JavaVM *vm, void *reserved)
{
}

// native member function mainInitApp
JNIEXPORT void JNICALL JNI_FUNCTION(NativeInterface_mainInitApp)(JNIEnv* env, jobject thiz, int width, int height)
{
	// todo nothing
	__android_log_print(ANDROID_LOG_INFO, JNI_APPNAME, "Native Application Init");

	g_jniEnv = env;
		
	jclass local = (*env)->FindClass(env, JNI_CLASSNAME(NativeInterface));
	g_classNativeInterface = (jclass)(*env)->NewGlobalRef(env, local);

	(*g_javaVM)->AttachCurrentThread(g_javaVM, &g_jniEnv, NULL);

	g_quitApplication = (*g_jniEnv)->GetStaticMethodID(g_jniEnv, g_classNativeInterface, "quitApplication", "()V");	
	g_openURL = (*g_jniEnv)->GetStaticMethodID(g_jniEnv, g_classNativeInterface, "openURL", "(Ljava/lang/String;)V");
	g_isNetworkAvailable = (*g_jniEnv)->GetStaticMethodID(g_jniEnv, g_classNativeInterface, "isNetworkAvailable", "()Z");
	g_systemGC = (*g_jniEnv)->GetStaticMethodID(g_jniEnv, g_classNativeInterface, "systemGC", "()V");

	__android_log_print(ANDROID_LOG_INFO, JNI_APPNAME, "Init jni OK");
	applicationInitApp(width, height);
}

// native member function mainLoop
JNIEXPORT void JNICALL JNI_FUNCTION(NativeInterface_mainLoop)(JNIEnv* env, jobject thiz)
{
	g_jniEnv = env;
	applicationLoop();
}

// native member function mainExitApp
JNIEXPORT void JNICALL JNI_FUNCTION(NativeInterface_mainExitApp)(JNIEnv* env, jobject thiz)
{
	__android_log_print(ANDROID_LOG_INFO, JNI_APPNAME, "Native Application Exit");
	g_jniEnv = env;
	applicationExitApp();
}


// native member function mainPauseApp
JNIEXPORT void JNICALL JNI_FUNCTION(NativeInterface_mainPauseApp)(JNIEnv* env, jobject thiz)
{
	__android_log_print(ANDROID_LOG_INFO, JNI_APPNAME, "Native Application Pause");
	g_jniEnv = env;
	applicationPauseApp();
}


// native member function mainResumeApp
JNIEXPORT void JNICALL JNI_FUNCTION(NativeInterface_mainResumeApp)(JNIEnv* env, jobject thiz, int showConnecting)
{
	__android_log_print(ANDROID_LOG_INFO, JNI_APPNAME, "Native Application Resume");
	g_jniEnv = env;
	applicationResumeApp(showConnecting);
}


// native member function mainReleaseDevice
JNIEXPORT void JNICALL JNI_FUNCTION(NativeInterface_mainReleaseDevice)(JNIEnv* env, jobject thiz)
{
	__android_log_print(ANDROID_LOG_INFO, JNI_APPNAME, "Native Application release device");
	g_jniEnv = env;
	applicationRelease();
}


// native member function mainResizeWindow
JNIEXPORT void JNICALL JNI_FUNCTION(NativeInterface_mainResizeWindow)(JNIEnv* env, jobject thiz, int w, int h)
{
	__android_log_print(ANDROID_LOG_INFO, JNI_APPNAME, "Native Application resize: %d %d", w, h);
	g_jniEnv = env;
	applicationResizeWindow(w, h);
}


// native member function mainTouchDown
JNIEXPORT void JNICALL JNI_FUNCTION(NativeInterface_mainTouchDown)(JNIEnv* env, jobject thiz, int touchID, int x, int y)
{
	//__android_log_print(ANDROID_LOG_INFO, JNI_APPNAME, "Native Application Touchdown: %d %d %d", touchID, x,y );
	applicationTouchDown(touchID, x, y);
}


// native member function mainTouchMove
JNIEXPORT void JNICALL JNI_FUNCTION(NativeInterface_mainTouchMove)(JNIEnv* env, jobject thiz, int touchID, int x, int y)
{
	applicationTouchMove(touchID, x, y);
}


// native member function mainTouchUp
JNIEXPORT void JNICALL JNI_FUNCTION(NativeInterface_mainTouchUp)(JNIEnv* env, jobject thiz, int touchID, int x, int y)
{
	//__android_log_print(ANDROID_LOG_INFO, JNI_APPNAME, "Native Application Touchup: %d %d %d", touchID, x,y );
	applicationTouchUp(touchID, x, y);
}


// native member function updateAccelerometer
JNIEXPORT void JNICALL JNI_FUNCTION(NativeInterface_updateAccelerometer)(JNIEnv* env, jobject thiz, float x, float y, float z)
{
	applicationUpdateAccelerometer(x, y, z);
}


// native member function setAccelerometerSupport
JNIEXPORT void JNICALL JNI_FUNCTION(NativeInterface_setAccelerometerSupport)(JNIEnv* env, jobject thiz, int enable)
{
	applicationSetAccelerometer(enable);
}


// native member function setApkPath
JNIEXPORT void JNICALL JNI_FUNCTION(NativeInterface_setApkPath)(JNIEnv* env, jobject thiz, jstring apkPath)
{
	const char *path = getJString(env, apkPath);

	// set apkPath
	__android_log_print(ANDROID_LOG_INFO, JNI_APPNAME, "Set APK path: %s", path);
	applicationSetAPK(path);

	(*env)->ReleaseStringUTFChars(env, apkPath, path);
}


// native member function setApkPath
JNIEXPORT void JNICALL JNI_FUNCTION(NativeInterface_setObbPath)(JNIEnv* env, jobject thiz, jstring apkPath)
{
	const char *path = getJString(env, apkPath);

	// set apkPath
	__android_log_print(ANDROID_LOG_INFO, JNI_APPNAME, "Set Obb path: %s", path);
	applicationSetObbPath(path);

	(*env)->ReleaseStringUTFChars(env, apkPath, path);
}


// native member function setApkPath
JNIEXPORT void JNICALL JNI_FUNCTION(NativeInterface_setBundleID)(JNIEnv* env, jobject thiz, jstring bundleID)
{
	const char *id = getJString(env, bundleID);

	// set bundle id
	applicationSetBundleID(id);

	(*env)->ReleaseStringUTFChars(env, bundleID, id);
}


// native member function setSaveFolder
JNIEXPORT void JNICALL JNI_FUNCTION(NativeInterface_setSaveFolder)(JNIEnv* env, jobject thiz, jstring saveFolder)
{
	const char *path = getJString(env, saveFolder);

	// set apkPath
	__android_log_print(ANDROID_LOG_INFO, JNI_APPNAME, "Set save folder path: %s", path);
	applicationSetSaveFolder(path);

	(*env)->ReleaseStringUTFChars(env, saveFolder, path);
}


// native member function setDownloadFolder
JNIEXPORT void JNICALL JNI_FUNCTION(NativeInterface_setDownloadFolder)(JNIEnv* env, jobject thiz, jstring saveFolder)
{
	const char *path = getJString(env, saveFolder);

	// set apkPath
	__android_log_print(ANDROID_LOG_INFO, JNI_APPNAME, "Set download folder path: %s", path);
	applicationSetDownloadFolder(path);

	(*env)->ReleaseStringUTFChars(env, saveFolder, path);
}


// native member function setMacAddress
JNIEXPORT void JNICALL JNI_FUNCTION(NativeInterface_setDeviceID)(JNIEnv* env, jobject thiz, jstring deviceID)
{
	const char *id = getJString(env, deviceID);

	__android_log_print(ANDROID_LOG_INFO, JNI_APPNAME, "Set device id: %s", id);
	applicationSetDeviceID(id);

	(*env)->ReleaseStringUTFChars(env, deviceID, id);
}


// native member function setCPID
JNIEXPORT void JNICALL JNI_FUNCTION(NativeInterface_setCPID)(JNIEnv* env, jobject thiz, jstring cpid)
{
	const char *pCPID = getJString(env, cpid);

	__android_log_print(ANDROID_LOG_INFO, JNI_APPNAME, "Set CPID: %s", pCPID);
	applicationSetCPID(pCPID);

	(*env)->ReleaseStringUTFChars(env, cpid, pCPID);
}


// native member function setAndroidDeviceInfo
JNIEXPORT void JNICALL JNI_FUNCTION(NativeInterface_setAndroidDeviceInfo)(JNIEnv* env, jobject thiz, jstring deviceManufacturer, jstring deviceModel, jstring osName)
{
	const char *facturer = getJString(env, deviceManufacturer);
	const char *model = getJString(env, deviceModel);
	const char *os = getJString(env, osName);

	applicationSetAndroidDeviceInfo(facturer, model, os);

	__android_log_print(ANDROID_LOG_INFO, JNI_APPNAME, "Device info: %s %s %s", facturer, model, os);

	(*env)->ReleaseStringUTFChars(env, deviceManufacturer, facturer);
	(*env)->ReleaseStringUTFChars(env, deviceModel, model);
	(*env)->ReleaseStringUTFChars(env, osName, os);
}


// native member function obBack
JNIEXPORT jboolean JNICALL JNI_FUNCTION(NativeInterface_onBack)(JNIEnv* env, jobject thiz)
{
	if (applicationOnBack() == 1)
		return JNI_TRUE;
	return JNI_FALSE;
}

void nativeInterface_quitApplication()
{
	(*g_javaVM)->AttachCurrentThread(g_javaVM, &g_jniEnv, NULL);

	if (g_quitApplication != NULL && g_classNativeInterface != NULL)
	{
		__android_log_print(ANDROID_LOG_INFO, JNI_APPNAME, "call NativeInterface::quitApplication");
		(*g_jniEnv)->CallStaticVoidMethod(g_jniEnv, g_classNativeInterface, g_quitApplication);
	}
	else
	{
		__android_log_print(ANDROID_LOG_INFO, JNI_APPNAME, "can not call NativeInterface::quitApplication");
	}
}

void nativeInterface_systemGC()
{
	(*g_javaVM)->AttachCurrentThread(g_javaVM, &g_jniEnv, NULL);

	if (g_systemGC != NULL && g_classNativeInterface != NULL)
	{
		__android_log_print(ANDROID_LOG_INFO, JNI_APPNAME, "call NativeInterface::systemGC");
		(*g_jniEnv)->CallStaticVoidMethod(g_jniEnv, g_classNativeInterface, g_systemGC);
	}
	else
	{
		__android_log_print(ANDROID_LOG_INFO, JNI_APPNAME, "can not call NativeInterface::systemGC");
	}
}

void nativeInterface_openURL(const char *url)
{
	(*g_javaVM)->AttachCurrentThread(g_javaVM, &g_jniEnv, NULL);

	if (g_openURL != NULL && g_classNativeInterface != NULL)
	{
		jstring jstringValue = (*g_jniEnv)->NewStringUTF(g_jniEnv, url);

		__android_log_print(ANDROID_LOG_INFO, JNI_APPNAME, "call NativeInterface::openURL");
		(*g_jniEnv)->CallStaticVoidMethod(g_jniEnv, g_classNativeInterface, g_openURL, jstringValue);

		(*g_jniEnv)->DeleteLocalRef(g_jniEnv, jstringValue);
	}
	else
	{
		__android_log_print(ANDROID_LOG_INFO, JNI_APPNAME, "can not call NativeInterface::openURL");
	}
}

int nativeInterface_applicationIsNetworkAvailable()
{
	(*g_javaVM)->AttachCurrentThread(g_javaVM, &g_jniEnv, NULL);

	if (g_isNetworkAvailable != NULL && g_classNativeInterface != NULL)
	{
		jboolean ret = (*g_jniEnv)->CallStaticBooleanMethod(g_jniEnv, g_classNativeInterface, g_isNetworkAvailable);
		return ret == JNI_TRUE ? 1 : 0;
	}

	return 1;
}
#endif