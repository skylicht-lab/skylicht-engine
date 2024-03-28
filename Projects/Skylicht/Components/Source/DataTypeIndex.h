#pragma once

#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64) || defined(CYGWIN)

// WINDOW
#ifdef _SKYLICHT_STATIC_LIB_

#define COMPONENT_API

#else

#ifdef COMPONENTS_EXPORTS
#define COMPONENT_API __declspec(dllexport)
#else
#define COMPONENT_API __declspec(dllimport)
#endif

#endif

#else

// GCC or OTHER
// Force symbol export in shared libraries built with gcc.
#if (__GNUC__ >= 4) && !defined(_SKYLICHT_STATIC_LIB_) && defined(SKYLICHT_EXPORTS)
#define COMPONENT_API __attribute__ ((visibility("default")))
#else
#define COMPONENT_API
#endif

#endif

#define DECLARE_COMPONENT_DATA_TYPE_INDEX(type) COMPONENT_API extern u32 type##_DataTypeIndex