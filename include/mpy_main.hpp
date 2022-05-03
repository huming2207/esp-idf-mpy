#pragma once

#include <esp_err.h>

class mpy_main
{
public:
    esp_err_t init(size_t _heap_size);
    void start_repl();

private:
    static const constexpr char *TAG = "mpy_main";
    size_t heap_size = 32768;
    char *py_stack_top = nullptr;
    uint8_t *heap_ptr = nullptr;
};
