# Copyright (c) 2021 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CUSTOM_GCC_LINKER_FILE ${CMAKE_CURRENT_SOURCE_DIR}/linkers/gcc/user_linker.ld)
set(CUSTOM_SES_LINKER_FILE ${CMAKE_CURRENT_SOURCE_DIR}/linkers/segger/user_linker.icf)
set(CUSTOM_IAR_LINKER_FILE ${CMAKE_CURRENT_SOURCE_DIR}/linkers/iar/user_linker.icf)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(user_app)

sdk_app_inc(inc)
sdk_app_inc(core0/src)
sdk_app_inc($ENV{HPM_SDK_BASE}/samples/multicore/common)
sdk_app_src($ENV{HPM_SDK_BASE}/samples/multicore/common/multicore_common.c)
sdk_app_src(core0/src/main_core0.c core0/src/sec_core_img.c)

generate_ide_projects()
