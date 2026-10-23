/*
 * Copyright (c) 2022 HPMicro
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>

#include "board.h"
#include "hpm_ipc_event_mgr.h"
#include "erpc_matrix_multiply.h"
#include "erpc_matrix_multiply_server.h"
#include "erpc_server_setup.h"
#include "rpmsg_lite.h"
#include "FreeRTOS.h"
#include "task.h"

#define ERPC_TRANSPORT_RPMSG_LITE_LINK_ID RL_PLATFORM_HPM6XXX_DUAL_CORE_LINK_ID
#define APP_ERPC_READY_EVENT_DATA (1U)
#define SH_MEM_TOTAL_SIZE (6144U)

ATTR_SHARE_MEM static char rpmsg_lite_base[SH_MEM_TOTAL_SIZE];

void erpcMatrixMultiply(Matrix matrix_a, Matrix matrix_b, Matrix result)
{
    for (uint32_t row = 0; row < matrix_size; row++) {
        for (uint32_t col = 0; col < matrix_size; col++) {
            result[row][col] = 0;
            for (uint32_t inner = 0; inner < matrix_size; inner++) {
                result[row][col] += matrix_a[row][inner] * matrix_b[inner][col];
            }
        }
    }
}

void erpcSwitchLightLed(void)
{
    board_led_toggle();
}

static void signal_erpc_ready(void)
{
    (void)ipc_tigger_event(ipc_remote_start_event, APP_ERPC_READY_EVENT_DATA);
}

static void server_task(void *param)
{
    (void)param;

    erpc_transport_t transport = erpc_transport_rpmsg_lite_rtos_remote_init(
        101U, 100U, rpmsg_lite_base, ERPC_TRANSPORT_RPMSG_LITE_LINK_ID,
        signal_erpc_ready, NULL);
    erpc_mbf_t message_buffer_factory = erpc_mbf_rpmsg_init(transport);
    erpc_server_t server = erpc_server_init(transport, message_buffer_factory);
    erpc_service_t service = create_MatrixMultiplyService_service();
    erpc_add_service_to_server(server, service);

    printf("Core1: RPMsg remote ready; serving eRPC\r\n");
    erpc_status_t status = erpc_server_run(server);
    printf("Core1: eRPC server stopped with status %d\r\n", (int)status);

    erpc_remove_service_from_server(server, service);
    destroy_MatrixMultiplyService_service(service);
    erpc_server_stop(server);
    erpc_server_deinit(server);
    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

int main(void)
{
    board_init_core1();
    board_init_led_pins();
    ipc_init();
    ipc_enable_event_interrupt(2U);

    if (xTaskCreate(server_task, "ERPC_SERVER", configMINIMAL_STACK_SIZE + 256U,
                    NULL, tskIDLE_PRIORITY + 1U, NULL) != pdPASS) {
        printf("Core1: failed to create server task\r\n");
        return -1;
    }

    vTaskStartScheduler();
    for (;;) {
    }
}
