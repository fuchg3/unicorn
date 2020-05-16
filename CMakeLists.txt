# Tested on window10(x64) with vs2019.
# Open the "x86 Native Tools Command Prompt for VS 2019",
#   cd ${UNICORN_SOURCE_DIR}
#   mkdir build
#   cd build
#   cmake -G "NMake Makefiles" ..
#   nmake
# Or Open "x64 Native Tools Command Prompt for VS 2019" for 64bit binary.
# Tested on Ubuntu-1804-amd64 with gcc.
#   $ cd ${UNICORN_SOURCE_DIR}
#   $ mkdir build
#   $ cd build
#   $ cmake ..
#   $ make
# By Huitao Chen, 2019

cmake_minimum_required(VERSION 3.1)
project(unicorn C)

set(UNICORN_VERSION_MAJOR 1)
set(UNICORN_VERSION_MINOR 0)
set(UNICORN_VERSION_PATCH 2)

# qemu uses assert(). It is not recommended to define NDEBUG if using assert()
#  to detect error conditions since the software may behave
#  non-deterministically. Remove the NDEBUG macro.
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    string(REPLACE "-DNDEBUG" "" CMAKE_C_FLAGS_RELEASE ${CMAKE_C_FLAGS_RELEASE})
endif()

set(UNICORN_GEN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/gen)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/qemu
    ${CMAKE_CURRENT_SOURCE_DIR}/qemu/include
    ${CMAKE_CURRENT_SOURCE_DIR}/qemu/tcg
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${UNICORN_GEN_DIR}
)

add_compile_options(
    -I${CMAKE_CURRENT_SOURCE_DIR}/qemu/tcg/aarch64
    -D_GNU_SOURCE
    -D_FILE_OFFSET_BITS=64
    -D_LARGEFILE_SOURCE
    -Wall -O2
    -fPIC -fpic -fvisibility=hidden
)

add_library(arm-softmmu
    ${CMAKE_CURRENT_SOURCE_DIR}/qemu/cpu-exec.c
    ${CMAKE_CURRENT_SOURCE_DIR}/qemu/cpus.c
    ${CMAKE_CURRENT_SOURCE_DIR}/qemu/cputlb.c
    ${CMAKE_CURRENT_SOURCE_DIR}/qemu/exec.c
    ${CMAKE_CURRENT_SOURCE_DIR}/qemu/fpu/softfloat.c
    ${CMAKE_CURRENT_SOURCE_DIR}/qemu/hw/arm/tosa.c
    ${CMAKE_CURRENT_SOURCE_DIR}/qemu/hw/arm/virt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/qemu/ioport.c
    ${CMAKE_CURRENT_SOURCE_DIR}/qemu/memory.c
    ${CMAKE_CURRENT_SOURCE_DIR}/qemu/memory_mapping.c
    ${CMAKE_CURRENT_SOURCE_DIR}/qemu/target-arm/cpu.c
    ${CMAKE_CURRENT_SOURCE_DIR}/qemu/target-arm/crypto_helper.c
    ${CMAKE_CURRENT_SOURCE_DIR}/qemu/target-arm/helper.c
    ${CMAKE_CURRENT_SOURCE_DIR}/qemu/target-arm/iwmmxt_helper.c
    ${CMAKE_CURRENT_SOURCE_DIR}/qemu/target-arm/neon_helper.c
    ${CMAKE_CURRENT_SOURCE_DIR}/qemu/target-arm/op_helper.c
    ${CMAKE_CURRENT_SOURCE_DIR}/qemu/target-arm/psci.c
    ${CMAKE_CURRENT_SOURCE_DIR}/qemu/target-arm/translate.c
    ${CMAKE_CURRENT_SOURCE_DIR}/qemu/target-arm/unicorn_arm.c
    ${CMAKE_CURRENT_SOURCE_DIR}/qemu/tcg/optimize.c
    ${CMAKE_CURRENT_SOURCE_DIR}/qemu/tcg/tcg.c
    ${CMAKE_CURRENT_SOURCE_DIR}/qemu/translate-all.c
)
target_compile_options(arm-softmmu PRIVATE
    -include arm.h
)
target_compile_definitions(arm-softmmu PRIVATE NEED_CPU_H)
target_include_directories(arm-softmmu PRIVATE
    ${UNICORN_GEN_DIR}/arm-softmmu
    ${CMAKE_CURRENT_SOURCE_DIR}/qemu/target-arm
)

add_library(armeb-softmmu
    ${CMAKE_CURRENT_SOURCE_DIR}/qemu/cpu-exec.c
    ${CMAKE_CURRENT_SOURCE_DIR}/qemu/cpus.c
    ${CMAKE_CURRENT_SOURCE_DIR}/qemu/cputlb.c
    ${CMAKE_CURRENT_SOURCE_DIR}/qemu/exec.c
    ${CMAKE_CURRENT_SOURCE_DIR}/qemu/fpu/softfloat.c
    ${CMAKE_CURRENT_SOURCE_DIR}/qemu/hw/arm/tosa.c
    ${CMAKE_CURRENT_SOURCE_DIR}/qemu/hw/arm/virt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/qemu/ioport.c
    ${CMAKE_CURRENT_SOURCE_DIR}/qemu/memory.c
    ${CMAKE_CURRENT_SOURCE_DIR}/qemu/memory_mapping.c
    ${CMAKE_CURRENT_SOURCE_DIR}/qemu/target-arm/cpu.c
    ${CMAKE_CURRENT_SOURCE_DIR}/qemu/target-arm/crypto_helper.c
    ${CMAKE_CURRENT_SOURCE_DIR}/qemu/target-arm/helper.c
    ${CMAKE_CURRENT_SOURCE_DIR}/qemu/target-arm/iwmmxt_helper.c
    ${CMAKE_CURRENT_SOURCE_DIR}/qemu/target-arm/neon_helper.c
    ${CMAKE_CURRENT_SOURCE_DIR}/qemu/target-arm/op_helper.c
    ${CMAKE_CURRENT_SOURCE_DIR}/qemu/target-arm/psci.c
    ${CMAKE_CURRENT_SOURCE_DIR}/qemu/target-arm/translate.c
    ${CMAKE_CURRENT_SOURCE_DIR}/qemu/target-arm/unicorn_arm.c
    ${CMAKE_CURRENT_SOURCE_DIR}/qemu/tcg/optimize.c
    ${CMAKE_CURRENT_SOURCE_DIR}/qemu/tcg/tcg.c
    ${CMAKE_CURRENT_SOURCE_DIR}/qemu/translate-all.c
)
target_compile_options(armeb-softmmu PRIVATE
    -include armeb.h
)
target_compile_definitions(armeb-softmmu PRIVATE NEED_CPU_H)
target_include_directories(armeb-softmmu PRIVATE
    ${UNICORN_GEN_DIR}/armeb-softmmu
    ${CMAKE_CURRENT_SOURCE_DIR}/qemu/target-arm
)

set(UNICORN_SRCS_COMMON
    ${CMAKE_CURRENT_SOURCE_DIR}/list.c
    ${CMAKE_CURRENT_SOURCE_DIR}/qemu/accel.c
    ${CMAKE_CURRENT_SOURCE_DIR}/qemu/glib_compat.c
    ${CMAKE_CURRENT_SOURCE_DIR}/qemu/hw/core/machine.c
    ${CMAKE_CURRENT_SOURCE_DIR}/qemu/hw/core/qdev.c
    ${CMAKE_CURRENT_SOURCE_DIR}/qemu/qapi/qapi-dealloc-visitor.c
    ${CMAKE_CURRENT_SOURCE_DIR}/qemu/qapi/qapi-visit-core.c
    ${CMAKE_CURRENT_SOURCE_DIR}/qemu/qapi/qmp-input-visitor.c
    ${CMAKE_CURRENT_SOURCE_DIR}/qemu/qapi/qmp-output-visitor.c
    ${CMAKE_CURRENT_SOURCE_DIR}/qemu/qapi/string-input-visitor.c
    ${CMAKE_CURRENT_SOURCE_DIR}/qemu/qemu-log.c
    ${CMAKE_CURRENT_SOURCE_DIR}/qemu/qemu-timer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/qemu/qobject/qbool.c
    ${CMAKE_CURRENT_SOURCE_DIR}/qemu/qobject/qdict.c
    ${CMAKE_CURRENT_SOURCE_DIR}/qemu/qobject/qerror.c
    ${CMAKE_CURRENT_SOURCE_DIR}/qemu/qobject/qfloat.c
    ${CMAKE_CURRENT_SOURCE_DIR}/qemu/qobject/qint.c
    ${CMAKE_CURRENT_SOURCE_DIR}/qemu/qobject/qlist.c
    ${CMAKE_CURRENT_SOURCE_DIR}/qemu/qobject/qstring.c
    ${CMAKE_CURRENT_SOURCE_DIR}/qemu/qom/container.c
    ${CMAKE_CURRENT_SOURCE_DIR}/qemu/qom/cpu.c
    ${CMAKE_CURRENT_SOURCE_DIR}/qemu/qom/object.c
    ${CMAKE_CURRENT_SOURCE_DIR}/qemu/qom/qom-qobject.c
    ${CMAKE_CURRENT_SOURCE_DIR}/qemu/tcg-runtime.c
    ${CMAKE_CURRENT_SOURCE_DIR}/qemu/util/aes.c
    ${CMAKE_CURRENT_SOURCE_DIR}/qemu/util/bitmap.c
    ${CMAKE_CURRENT_SOURCE_DIR}/qemu/util/bitops.c
    ${CMAKE_CURRENT_SOURCE_DIR}/qemu/util/crc32c.c
    ${CMAKE_CURRENT_SOURCE_DIR}/qemu/util/cutils.c
    ${CMAKE_CURRENT_SOURCE_DIR}/qemu/util/error.c
    ${CMAKE_CURRENT_SOURCE_DIR}/qemu/util/getauxval.c
    ${CMAKE_CURRENT_SOURCE_DIR}/qemu/util/host-utils.c
    ${CMAKE_CURRENT_SOURCE_DIR}/qemu/util/module.c
    ${CMAKE_CURRENT_SOURCE_DIR}/qemu/util/qemu-timer-common.c
    ${CMAKE_CURRENT_SOURCE_DIR}/qemu/vl.c
    ${CMAKE_CURRENT_SOURCE_DIR}/uc.c
)

set(UNICORN_SRCS
    ${UNICORN_SRCS_COMMON}
    ${CMAKE_CURRENT_SOURCE_DIR}/qemu/util/oslib-posix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/qemu/util/qemu-thread-posix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/qemu/qapi-types.c
    ${CMAKE_CURRENT_SOURCE_DIR}/qemu/qapi-visit.c
    ${CMAKE_CURRENT_SOURCE_DIR}/jni/Unicorn.c
)

add_library(unicorn SHARED
    ${UNICORN_SRCS}
)

set(UNICRON_COMPILE_OPTIONS ${UNICRON_COMPILE_OPTIONS} -DUNICORN_HAS_ARM)
set(UNICRON_LINK_LIBRARIES ${UNICRON_LINK_LIBRARIES} arm-softmmu armeb-softmmu)

find_library(ndk_log log)

target_compile_options(unicorn PRIVATE
    ${UNICRON_COMPILE_OPTIONS}
)
target_include_directories(unicorn PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/jni
)
target_link_libraries(unicorn
    ${UNICRON_LINK_LIBRARIES}
    ${ndk_log}
)
set_target_properties(unicorn PROPERTIES
    VERSION ${UNICORN_VERSION_MAJOR}
    SOVERSION ${UNICORN_VERSION_MAJOR}
)
