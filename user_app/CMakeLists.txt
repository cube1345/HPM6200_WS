# Copyright (c) 2021 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CUSTOM_GCC_LINKER_FILE ${CMAKE_CURRENT_SOURCE_DIR}/linkers/gcc/user_linker.ld)
set(CUSTOM_SES_LINKER_FILE ${CMAKE_CURRENT_SOURCE_DIR}/linkers/segger/user_linker.icf)
set(CUSTOM_IAR_LINKER_FILE ${CMAKE_CURRENT_SOURCE_DIR}/linkers/iar/user_linker.icf)

set(CONFIG_ERPC 1)
set(CONFIG_ERPC_CLIENT 1)
set(CONFIG_ERPC_RPMSG_MASTER 1)
set(CONFIG_IPC_EVENT_MGR 1)
set(CONFIG_IPC_EVENT_MGR_MBX 1)
set(CONFIG_FREERTOS 1)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(user_app)

sdk_app_inc(inc)
sdk_app_inc(core0/src)
sdk_app_inc($ENV{HPM_SDK_BASE}/samples/multicore/common)
sdk_inc(config)
sdk_inc($ENV{HPM_SDK_BASE}/samples/multicore/erpc/erpc_matrix_multiply_rpmsg_rtos/config)
sdk_inc($ENV{HPM_SDK_BASE}/samples/multicore/erpc/erpc_matrix_multiply_rpmsg_rtos/generated)
sdk_app_src($ENV{HPM_SDK_BASE}/samples/multicore/common/multicore_common.c)
sdk_app_src(core0/src/main_core0.c core0/src/sec_core_img.c)
sdk_src($ENV{HPM_SDK_BASE}/samples/multicore/erpc/erpc_matrix_multiply_rpmsg_rtos/generated/erpc_matrix_multiply_client.cpp)

sdk_compile_definitions(-DHPM_FEATURE_MBX_SIDE_A)
sdk_compile_definitions(-DUSE_NONVECTOR_MODE=1)
sdk_compile_definitions(-DDISABLE_IRQ_PREEMPTIVE=1)
sdk_compile_options(-fno-exceptions)
sdk_ld_options("--std=c++11")

generate_ide_projects()
