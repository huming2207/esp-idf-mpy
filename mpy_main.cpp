#include <esp_log.h>

#include "py/gc.h"
#include "py/runtime.h"

#include "mpy_main.hpp"
#include "pyexec.h"
#include "py/lexer.h"
#include "py/compile.h"

void do_str(const char *src, mp_parse_input_kind_t input_kind) {
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        mp_lexer_t *lex = mp_lexer_new_from_str_len(MP_QSTR__lt_stdin_gt_, src, strlen(src), 0);
        qstr source_name = lex->source_name;
        mp_parse_tree_t parse_tree = mp_parse(lex, input_kind);
        mp_obj_t module_fun = mp_compile(&parse_tree, source_name, true);
        mp_call_function_0(module_fun);
        nlr_pop();
    } else {
        // uncaught exception
        mp_obj_print_exception(&mp_plat_print, (mp_obj_t)nlr.ret_val);
    }
}

esp_err_t mpy_main::init(size_t _heap_size)
{
    if (_heap_size < 1) {
        ESP_LOGE(TAG, "Heap too small: %zu", _heap_size);
        return ESP_ERR_INVALID_ARG;
    }

    heap_ptr = (uint8_t *)malloc(_heap_size);
    if (heap_ptr == nullptr) {
        ESP_LOGE(TAG, "Failed to allocate heap");
        return ESP_ERR_NO_MEM;
    }

    gc_init(heap_ptr, heap_ptr + _heap_size);
    mp_init();

    return ESP_OK;
}
