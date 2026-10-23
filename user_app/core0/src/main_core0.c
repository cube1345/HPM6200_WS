/*
 * Copyright (c) 2026
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>

#include "board.h"
#include "multicore_common.h"

int main(void)
{
    board_init();
    printf("Starting Core1 RGB PWM application\n");
    multicore_release_cpu(HPM_CORE1, SEC_CORE_IMG_START);

    while (1) {
    }
}


