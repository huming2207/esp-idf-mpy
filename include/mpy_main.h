#pragma once

#include <esp_err.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "mpy_bridge.h"


esp_err_t mpy_init(size_t _heap_size);
void mpy_start_repl();

#ifdef __cplusplus
}
#endif
