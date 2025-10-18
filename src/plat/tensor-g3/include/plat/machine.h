/*
 * Copyright 2025, Millpex
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#pragma once

/*================================================================================
 * Core SoC Infrastructure Addresses
 * Source: Tensor-G3.dts and /proc/iomem
 *==============================================================================*/

/*
 * Pin Controller (pinctrl_0)
 * Manages GPIO and peripheral pin multiplexing.
 */
#define G3_PINCTRL_BASE 0x10840000

/*
 * Top-level Clock Controller (cmu_top)
 * Manages the main clock gates and PLLs for the SoC.
 */
#define G3_CLK_BASE 0x133C0000

/*================================================================================
 * Include all other platform-specific hardware definitions
 *==============================================================================*/

#include <plat/serial.h>
#include <plat/gic.h>
#include <plat/timer.h>
#include <plat/cache.h>

/*
 * Performs early platform-specific initialization.
 * This includes bringing up the console for debugging.
 */
void plat_serial_init(void);

/*
 * Initializes the platform-specific timer.
 */
void initTimer(void);

