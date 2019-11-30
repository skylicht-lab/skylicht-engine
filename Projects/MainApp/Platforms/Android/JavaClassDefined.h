#ifndef _JAVA_CLASS_DEFINED_

#define JNI_APPNAME	"libMainApp.so"
#define JNI_FUNCTION(function) Java_com_skylicht_client_##function
#define JNI_CLASSNAME(className) "com/skylicht/client/"#className

#endif