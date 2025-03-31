#pragma once

#include <stdint.h>

#define FP_OFF(x) (intptr_t)(x)

#ifdef _MSC_VER
#define open _open
#define read _read
#define close _close
#define stricmp _stricmp
#endif

