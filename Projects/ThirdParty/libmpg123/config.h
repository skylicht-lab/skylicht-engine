#define OPT_MULTI
#define OPT_GENERIC
#define OPT_I386

// disable ASM compile
#define OPT_NO_OPTIMIZE

#if defined(_WIN32)
#include "config_window.h"
#else
#include "config_linux.h"
#endif