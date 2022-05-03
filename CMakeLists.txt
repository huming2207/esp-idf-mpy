set(MICROPY_LIB_DIR    ${CMAKE_CURRENT_LIST_DIR}) # This component's directory
set(MICROPY_DIR         ${MICROPY_LIB_DIR}/mpy) # The mpy submodule's directory

include("${MICROPY_DIR}/py/py.cmake")

message(${CMAKE_BINARY_DIR})

#if(NOT CMAKE_BUILD_EARLY_EXPANSION)
#    include(${MICROPY_DIR}/py/usermod.cmake)
#    include(${MICROPY_DIR}/extmod/extmod.cmake)
#endif()


idf_component_register(
        SRCS
            "include/mpconfigport.h" ${MICROPY_SOURCE_PY}
            "mpy/shared/runtime/pyexec.c" "mpy/shared/runtime/pyexec.h"
            "mpy_main.cpp" "include/mpy_main.hpp"

        INCLUDE_DIRS "include" ${MICROPY_INC_CORE} "mpy/shared/runtime" ${CMAKE_BINARY_DIR})

foreach(comp ${IDF_COMPONENTS})
    micropy_gather_target_properties(__idf_${comp})
endforeach()

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

include("${MICROPY_DIR}/py/mkrules.cmake")