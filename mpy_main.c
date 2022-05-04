#include <esp_log.h>
#include <string.h>

#include <mpy_main.h>

#define TAG "mpy_main"

static uint8_t *heap_ptr = NULL;

static void do_str(const char *src, mp_parse_input_kind_t input_kind)
{
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

esp_err_t mpy_init(size_t _heap_size)
{
    volatile uint32_t sp = (uint32_t)esp_cpu_get_sp();
    if (_heap_size < 1) {
        ESP_LOGE(TAG, "Heap too small: %zu", _heap_size);
        return ESP_ERR_INVALID_ARG;
    }

    heap_ptr = (uint8_t *)malloc(_heap_size);
    if (heap_ptr == NULL) {
        ESP_LOGE(TAG, "Failed to allocate heap");
        return ESP_ERR_NO_MEM;
    }

    mp_stack_set_top((void *)sp);
    mp_stack_set_limit(8192);

    gc_init(heap_ptr, heap_ptr + _heap_size);
    mp_init();

    return ESP_OK;
}

static int vprintf_null(const char *format, va_list ap)
{
    // do nothing: this is used as a log target during raw repl mode
    return 0;
}

void mpy_start_repl()
{
    for (;;) {
        if (pyexec_mode_kind == PYEXEC_MODE_RAW_REPL) {
            vprintf_like_t vprintf_log = esp_log_set_vprintf(vprintf_null);
            if (pyexec_raw_repl() != 0) {
                break;
            }
            esp_log_set_vprintf(vprintf_log);
        } else {
            if (pyexec_friendly_repl() != 0) {
                break;
            }
        }
    }
}
