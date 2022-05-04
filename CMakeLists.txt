set(MICROPY_LIB_DIR    ${COMPONENT_DIR}) # This component's directory
set(MICROPY_DIR         ${MICROPY_LIB_DIR}/mpy) # The mpy submodule's directory

get_filename_component(MICROPY_DIR ${MICROPY_LIB_DIR}/mpy ABSOLUTE)

message(${MICROPY_DIR})
include("${MICROPY_DIR}/py/py.cmake")

file(GLOB_RECURSE MPY_C_SRC ${MICROPY_DIR}/py/*.c ${MICROPY_DIR}/shared/*.c)
file(GLOB_RECURSE MPY_C_HEADERS ${MICROPY_DIR}/py/*.h ${MICROPY_DIR}/shared/*.h)

#set_source_files_properties(${MPY_C_SRCS} PROPERTIES LANGUAGE C)
#set_source_files_properties(${MPY_C_HEADERS} PROPERTIES LANGUAGE C HEADER_FILE_ONLY TRUE)


if(NOT CMAKE_BUILD_EARLY_EXPANSION)
    include(${MICROPY_DIR}/py/usermod.cmake)
 #   include(${MICROPY_DIR}/extmod/extmod.cmake)
endif()

set(MICROPY_SOURCE_PORT
        "${MICROPY_LIB_DIR}/mphalport.c" "${MICROPY_LIB_DIR}/include/mphalport.h"
        "${MICROPY_LIB_DIR}/mpthreadport.c" "${MICROPY_LIB_DIR}/include/mpthreadport.h"
        "${MICROPY_LIB_DIR}/gccollect.c" "${MICROPY_LIB_DIR}/include/gccollect.h"
)

set(MICROPY_SOURCE_QSTR
        ${MICROPY_SOURCE_PY}
        ${MICROPY_SOURCE_EXTMOD}
        ${MICROPY_SOURCE_USERMOD}
        ${MICROPY_SOURCE_SHARED}
        ${MICROPY_SOURCE_LIB}
        ${MICROPY_SOURCE_PORT}
        ${MICROPY_SOURCE_BOARD}
)

set(IDF_COMPONENTS
        app_update
        bootloader_support
        bt
        driver
        esp_adc_cal
        esp_common
        esp_eth
        esp_event
        esp_ringbuf
        esp_rom
        esp_wifi
        freertos
        heap
        log
        lwip
        mbedtls
        mdns
        newlib
        nvs_flash
        sdmmc
        soc
        spi_flash
        ulp
        vfs
        xtensa
)

if(IDF_VERSION_MINOR GREATER_EQUAL 1 OR IDF_VERSION_MAJOR GREATER_EQUAL 5)
    list(APPEND IDF_COMPONENTS esp_netif)
endif()

if(IDF_VERSION_MINOR GREATER_EQUAL 2 OR IDF_VERSION_MAJOR GREATER_EQUAL 5)
    list(APPEND IDF_COMPONENTS esp_system)
    list(APPEND IDF_COMPONENTS esp_timer)
endif()

if(IDF_VERSION_MINOR GREATER_EQUAL 3 OR IDF_VERSION_MAJOR GREATER_EQUAL 5)
    list(APPEND IDF_COMPONENTS esp_hw_support)
    list(APPEND IDF_COMPONENTS esp_pm)
    list(APPEND IDF_COMPONENTS hal)
endif()

if(IDF_TARGET STREQUAL "esp32")
    list(APPEND IDF_COMPONENTS esp32)
elseif(IDF_TARGET STREQUAL "esp32c3")
    list(APPEND IDF_COMPONENTS esp32c3)
    list(APPEND IDF_COMPONENTS riscv)
elseif(IDF_TARGET STREQUAL "esp32s2")
    list(APPEND IDF_COMPONENTS esp32s2)
    list(APPEND IDF_COMPONENTS tinyusb)
elseif(IDF_TARGET STREQUAL "esp32s3")
    list(APPEND IDF_COMPONENTS esp32s3)
    list(APPEND IDF_COMPONENTS tinyusb)
endif()

idf_component_register(
        SRCS
            ${MICROPY_SOURCE_PY}
            ${MICROPY_SOURCE_PORT}
            ${MPY_C_SRCS} ${MPY_C_HEADERS}
            "mpy_main.c" "include/mpy_main.h"
        INCLUDE_DIRS
            "${MICROPY_LIB_DIR}/include"
            ${MICROPY_INC_CORE}
            ${MICROPY_INC_USERMOD}
            ${MICROPY_PORT_DIR}
            ${MICROPY_BOARD_DIR}
            ${CMAKE_BINARY_DIR}
        REQUIRES
            ${IDF_COMPONENTS}
)


set(MICROPY_TARGET ${COMPONENT_TARGET})

# Define mpy-cross flags, for use with frozen code.
set(MICROPY_CROSS_FLAGS -march=xtensawin)

# Set compile options for this port.
target_compile_definitions(${MICROPY_TARGET} PUBLIC
        ${MICROPY_DEF_CORE}
        MICROPY_ESP_IDF_4=1
        MICROPY_VFS_FAT=1
        MICROPY_VFS_LFS2=1
        FFCONF_H=\"${MICROPY_OOFATFS_DIR}/ffconf.h\"
        LFS1_NO_MALLOC LFS1_NO_DEBUG LFS1_NO_WARN LFS1_NO_ERROR LFS1_NO_ASSERT
        LFS2_NO_MALLOC LFS2_NO_DEBUG LFS2_NO_WARN LFS2_NO_ERROR LFS2_NO_ASSERT
)

target_compile_options(${MICROPY_TARGET} PUBLIC
        -Wno-clobbered
        -Wno-deprecated-declarations
        -Wno-missing-field-initializer
)


foreach(comp ${IDF_COMPONENTS})
    micropy_gather_target_properties(__idf_${comp})
endforeach()

if(IDF_VERSION_MINOR GREATER_EQUAL 2 OR IDF_VERSION_MAJOR GREATER_EQUAL 5)
    # These paths cannot currently be found by the IDF_COMPONENTS search loop above,
    # so add them explicitly.
    list(APPEND MICROPY_CPP_INC_EXTRA ${IDF_PATH}/components/soc/soc/${IDF_TARGET}/include)
    list(APPEND MICROPY_CPP_INC_EXTRA ${IDF_PATH}/components/soc/soc/include)
    if(IDF_VERSION_MINOR GREATER_EQUAL 3)
        list(APPEND MICROPY_CPP_INC_EXTRA ${IDF_PATH}/components/tinyusb/additions/include)
        list(APPEND MICROPY_CPP_INC_EXTRA ${IDF_PATH}/components/tinyusb/tinyusb/src)
    endif()
endif()

foreach(comp ${IDF_COMPONENTS})
    micropy_gather_target_properties(__idf_${comp})
endforeach()

include("${MICROPY_DIR}/py/mkrules.cmake")