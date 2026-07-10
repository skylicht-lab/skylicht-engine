#ifdef ANDROID
#include <stdlib.h>
#include <string.h>
#include <jni.h>
#include <android/log.h>
#include "JavaClassDefined.h"

extern void playStore_onInitialized();
extern void playStore_onInitializeFailed(int error, const char* message);
extern void playStore_onFetchProductFailed(int error, const char* message);
extern void playStore_onRestorePurchaseFailed(int error, const char* message);
extern void playStore_onPurchaseSucceeded(const char* productId, const char* receipt);
extern void playStore_onPurchaseFailed(const char* productId, int error, const char* message);

extern const char *getJString(JNIEnv* env, jstring jstr);
extern JNIEnv* skylichtGetJniEnv();

jclass g_classPlayStoreController = NULL;
jmethodID g_restorePurchase;
jmethodID g_restart;
jmethodID g_initiatePurchase;
jmethodID g_fetchAdditionalProducts;

JNIEXPORT void JNICALL JNI_FUNCTION(PlayStoreController_init)(JNIEnv* env, jobject thiz)
{
	__android_log_print(ANDROID_LOG_INFO, JNI_APPNAME, "PlayStoreController Init");
	
	jclass local = (*env)->FindClass(env, JNI_CLASSNAME(PlayStoreController));
	g_classPlayStoreController = (jclass)(*env)->NewGlobalRef(env, local);

	g_restorePurchase = (*env)->GetStaticMethodID(env, g_classPlayStoreController, "restorePurchase", "()V");
	g_restart = (*env)->GetStaticMethodID(env, g_classPlayStoreController, "restart", "()V");
	g_initiatePurchase = (*env)->GetStaticMethodID(env, g_classPlayStoreController, "initiatePurchase", "(Ljava/lang/String;)V");
	g_fetchAdditionalProducts = (*env)->GetStaticMethodID(env, g_classPlayStoreController, "fetchAdditionalProducts", "([Ljava/lang/String;)V");
}

extern void playStore_onProductsReceived(const char** productIds,
									  const char** titles,
									  const char** descriptions,
									  const char** localizedPrices,
									  double* prices,
									  const char** currencyCodes,
									  int count);

JNIEXPORT void JNICALL JNI_FUNCTION(PlayStoreController_onProductsReceived)(JNIEnv* env, jobject thiz, jobjectArray ids, jobjectArray titles, jobjectArray descriptions, jobjectArray prices, jdoubleArray values, jobjectArray currencies)
{
	int count = (*env)->GetArrayLength(env, ids);
	const char **c_ids = (const char **)malloc(sizeof(char *) * count);
	const char **c_titles = (const char **)malloc(sizeof(char *) * count);
	const char **c_descriptions = (const char **)malloc(sizeof(char *) * count);
	const char **c_prices = (const char **)malloc(sizeof(char *) * count);
	const char **c_currencies = (const char **)malloc(sizeof(char *) * count);
	double *c_values = (*env)->GetDoubleArrayElements(env, values, NULL);

	for (int i = 0; i < count; i++)
	{
		jstring j_id = (jstring)(*env)->GetObjectArrayElement(env, ids, i);
		jstring j_title = (jstring)(*env)->GetObjectArrayElement(env, titles, i);
		jstring j_description = (jstring)(*env)->GetObjectArrayElement(env, descriptions, i);
		jstring j_price = (jstring)(*env)->GetObjectArrayElement(env, prices, i);
		jstring j_currency = (jstring)(*env)->GetObjectArrayElement(env, currencies, i);

		c_ids[i] = getJString(env, j_id);
		c_titles[i] = getJString(env, j_title);
		c_descriptions[i] = getJString(env, j_description);
		c_prices[i] = getJString(env, j_price);
		c_currencies[i] = getJString(env, j_currency);

		(*env)->DeleteLocalRef(env, j_id);
		(*env)->DeleteLocalRef(env, j_title);
		(*env)->DeleteLocalRef(env, j_description);
		(*env)->DeleteLocalRef(env, j_price);
		(*env)->DeleteLocalRef(env, j_currency);
	}

	playStore_onProductsReceived(c_ids, c_titles, c_descriptions, c_prices, c_values, c_currencies, count);

	for (int i = 0; i < count; i++)
	{
		jstring j_id = (jstring)(*env)->GetObjectArrayElement(env, ids, i);
		jstring j_title = (jstring)(*env)->GetObjectArrayElement(env, titles, i);
		jstring j_description = (jstring)(*env)->GetObjectArrayElement(env, descriptions, i);
		jstring j_price = (jstring)(*env)->GetObjectArrayElement(env, prices, i);
		jstring j_currency = (jstring)(*env)->GetObjectArrayElement(env, currencies, i);

		(*env)->ReleaseStringUTFChars(env, j_id, c_ids[i]);
		(*env)->ReleaseStringUTFChars(env, j_title, c_titles[i]);
		(*env)->ReleaseStringUTFChars(env, j_description, c_descriptions[i]);
		(*env)->ReleaseStringUTFChars(env, j_price, c_prices[i]);
		(*env)->ReleaseStringUTFChars(env, j_currency, c_currencies[i]);

		(*env)->DeleteLocalRef(env, j_id);
		(*env)->DeleteLocalRef(env, j_title);
		(*env)->DeleteLocalRef(env, j_description);
		(*env)->DeleteLocalRef(env, j_price);
		(*env)->DeleteLocalRef(env, j_currency);
	}

	free(c_ids);
	free(c_titles);
	free(c_descriptions);
	free(c_prices);
	free(c_currencies);
	(*env)->ReleaseDoubleArrayElements(env, values, c_values, 0);
}

JNIEXPORT void JNICALL JNI_FUNCTION(PlayStoreController_onInitialized)(JNIEnv* env, jobject thiz)
{
	playStore_onInitialized();
}

JNIEXPORT void JNICALL JNI_FUNCTION(PlayStoreController_onInitializeFailed)(JNIEnv* env, jobject thiz, jint error, jstring msg)
{
	const char *cmsg = getJString(env, msg);
	playStore_onInitializeFailed(error, cmsg);
	(*env)->ReleaseStringUTFChars(env, msg, cmsg);
}

JNIEXPORT void JNICALL JNI_FUNCTION(PlayStoreController_onFetchProductFailed)(JNIEnv* env, jobject thiz, jint error, jstring msg)
{
	const char *cmsg = getJString(env, msg);
	playStore_onFetchProductFailed(error, cmsg);
	(*env)->ReleaseStringUTFChars(env, msg, cmsg);
}

JNIEXPORT void JNICALL JNI_FUNCTION(PlayStoreController_onRestorePurchaseFailed)(JNIEnv* env, jobject thiz, jint error, jstring message)
{
	const char *cmsg = getJString(env, message);
	playStore_onRestorePurchaseFailed(error, cmsg);
	(*env)->ReleaseStringUTFChars(env, message, cmsg);
}

JNIEXPORT void JNICALL JNI_FUNCTION(PlayStoreController_onPurchaseSucceeded)(JNIEnv* env, jobject thiz, jstring productId, jstring receipt)
{
	const char *cid = getJString(env, productId);
	const char *creceipt = getJString(env, receipt);
	playStore_onPurchaseSucceeded(cid, creceipt);
	(*env)->ReleaseStringUTFChars(env, productId, cid);
	(*env)->ReleaseStringUTFChars(env, receipt, creceipt);
}

JNIEXPORT void JNICALL JNI_FUNCTION(PlayStoreController_onPurchaseFailed)(JNIEnv* env, jobject thiz, jstring productId, jint error, jstring message)
{
	const char *cid = getJString(env, productId);
	const char *cmsg = getJString(env, message);
	playStore_onPurchaseFailed(cid, error, cmsg);
	(*env)->ReleaseStringUTFChars(env, productId, cid);
	(*env)->ReleaseStringUTFChars(env, message, cmsg);
}

void playStore_restorePurchase()
{
	JNIEnv* env = skylichtGetJniEnv();
	if (env != NULL && g_restorePurchase != NULL && g_classPlayStoreController != NULL)
	{
		(*env)->CallStaticVoidMethod(env, g_classPlayStoreController, g_restorePurchase);
	}
}

void playStore_restart()
{
	JNIEnv* env = skylichtGetJniEnv();
	if (env != NULL && g_restart != NULL && g_classPlayStoreController != NULL)
	{
		(*env)->CallStaticVoidMethod(env, g_classPlayStoreController, g_restart);
	}
}

void playStore_initiatePurchase(const char* productId)
{
	JNIEnv* env = skylichtGetJniEnv();
	if (env != NULL && g_initiatePurchase != NULL && g_classPlayStoreController != NULL)
	{
		jstring jproductId = (*env)->NewStringUTF(env, productId);
		(*env)->CallStaticVoidMethod(env, g_classPlayStoreController, g_initiatePurchase, jproductId);
		(*env)->DeleteLocalRef(env, jproductId);
	}
}

void playStore_fetchAdditionalProducts(const char** productIds, int count)
{
	JNIEnv* env = skylichtGetJniEnv();
	if (env != NULL && g_fetchAdditionalProducts != NULL && g_classPlayStoreController != NULL)
	{
		jclass stringClass = (*env)->FindClass(env, "java/lang/String");
		jobjectArray stringArray = (*env)->NewObjectArray(env, count, stringClass, NULL);
		for (int i = 0; i < count; i++)
		{
			jstring string = (*env)->NewStringUTF(env, productIds[i]);
			(*env)->SetObjectArrayElement(env, stringArray, i, string);
			(*env)->DeleteLocalRef(env, string);
		}
		(*env)->CallStaticVoidMethod(env, g_classPlayStoreController, g_fetchAdditionalProducts, stringArray);
		(*env)->DeleteLocalRef(env, stringArray);
		(*env)->DeleteLocalRef(env, stringClass);
	}
}
#endif
