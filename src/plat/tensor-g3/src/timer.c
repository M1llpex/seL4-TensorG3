/*
 * Copyright 2025, Millpex
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include <config.h>
#include <types.h>
#include <plat/machine.h> // Include master platform header
#include <plat/timer.h>
#include <arch/machine/generic_timer.h>

/*
 * Initialize the timer for Google Tensor G3 (Zuma)
 *
 * The Tensor G3 uses the ARM Generic Timer.
 * Source (Frequency): Tensor-G3.dts.txt & test_clk.txt confirm 24.576MHz.
 * Source (IRQ): Tensor-G3.dts.txt & /proc/interrupts confirm IRQ 30.
 */
void initTimer(void)
{
    /*
     * TODO: Verify if the 24.576MHz clock source for the generic timer
     * needs to be manually un-gated via the SoC's clock controller registers.
     * This implementation ASSUMES the bootloader has already enabled it.
     */

    /*
     * Initialize the ARM Generic Timer. This function reads the frequency
     * from CNTFRQ_EL0 (set by bootloader) and sets up the per-core timer
     * using KERNEL_TIMER_IRQ (defined in plat/timer.h).
     */
    initGenericTimer();
}
