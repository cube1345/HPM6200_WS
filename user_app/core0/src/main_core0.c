/*
 * Copyright (c) 2022 HPMicro
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdbool.h>
#include <stdio.h>

#include "board.h"
#include "hpm_ipc_event_mgr.h"
#include "erpc_client_setup.h"
#include "erpc_matrix_multiply.h"
#include "multicore_common.h"
#include "rpmsg_lite.h"
#include "FreeRTOS.h"
#include "task.h"

#define ERPC_TRANSPORT_RPMSG_LITE_LINK_ID RL_PLATFORM_HPM6XXX_DUAL_CORE_LINK_ID
#define APP_ERPC_READY_EVENT_DATA (1U)

static volatile uint16_t erpc_ready_event_data;

static void erpc_ready_event_handler(uint16_t event_data, void *context)
{
    (void)context;
    erpc_ready_event_data = event_data;
}

static void client_task(void *param)
{
    (void)param;

    Matrix matrix_a = {
        { 1,  2,  3,  4,  5},
        { 6,  7,  8,  9, 10},
        {11, 12, 13, 14, 15},
        {16, 17, 18, 19, 20},
        {21, 22, 23, 24, 25},
    };
    Matrix identity = {
        {1, 0, 0, 0, 0},
        {0, 1, 0, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 0, 1, 0},
        {0, 0, 0, 0, 1},
    };
    Matrix result = { 0 };

    (void)ipc_register_event(ipc_remote_start_event, erpc_ready_event_handler, NULL);
    printf("Core0: releasing Core1\r\n");
    multicore_release_cpu(HPM_CORE1, SEC_CORE_IMG_START);

    while (erpc_ready_event_data != APP_ERPC_READY_EVENT_DATA) {
        vTaskDelay(pdMS_TO_TICKS(1));
    }
    printf("Core0: Core1 RPMsg endpoint ready\r\n");

    erpc_transport_t transport = erpc_transport_rpmsg_lite_rtos_master_init(
        100U, 101U, ERPC_TRANSPORT_RPMSG_LITE_LINK_ID);
    erpc_mbf_t message_buffer_factory = erpc_mbf_rpmsg_init(transport);
    erpc_client_init(transport, message_buffer_factory);

    printf("Core0: sending matrix multiply RPC\r\n");
    erpcMatrixMultiply(matrix_a, identity, result);

    bool result_matches = true;
    for (uint32_t row = 0; row < matrix_size; row++) {
        for (uint32_t col = 0; col < matrix_size; col++) {
            if (result[row][col] != matrix_a[row][col]) {
                result_matches = false;
            }
        }
    }

    printf("Core0: eRPC result %s\r\n", result_matches ? "PASS" : "FAIL");
    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

int main(void)
{
    board_init();
    ipc_init();
    ipc_enable_event_interrupt(2U);

    if (xTaskCreate(client_task, "ERPC_CLIENT", configMINIMAL_STACK_SIZE + 256U,
                    NULL, tskIDLE_PRIORITY + 2U, NULL) != pdPASS) {
        printf("Core0: failed to create client task\r\n");
        return -1;
    }

    vTaskStartScheduler();
    for (;;) {
    }
}
