/*
 * Copyright 2025, Millpex
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

/*
 * Google Tensor G3 (Zuma) - L2/L3 Cache Configuration
 *
 * CPU Identification:
 * Source: dmesg.txt identifies the CPU ID as [0x411fd461].
 * Core: This ID corresponds definitively to an ARM Cortex-A72 core (ARMv8-A).
 *
 * Cache Hierarchy (Typical for Cortex-A72):
 * - L1 I/D caches per core
 * - L2 cache per cluster (or potentially shared L2)
 * - System-level L3 cache (if present, shared)
 *
 * Cache Coherency:
 * With ARMv8-A, cache coherency between cores and clusters is managed by the
 * hardware's interconnect (e.g., CCI or similar). Therefore, standard ARM
 * cache maintenance operations targeting the Point of Coherency (PoC) are
 * sufficient and correct for this platform.
 */

#include <config.h>
#include <types.h>
#include <plat/machine.h> // Include master platform header
#include <arch/machine/cache.h>
#include <object/tcb.h>
#include <utils/fence.h>

void plat_cleanL2Range(paddr_t start, paddr_t end)
{
    /*
     * Use the generic ARMv8 cache clean operation.
     * This function (typically using DC CVAC instructions) operates to the
     * Point of Coherency, ensuring data is visible to other masters.
     */
    cleanCacheRange_RAM(start, end);
}

void plat_invalidateL2Range(paddr_t start, paddr_t end)
{
    /*
     * Use the generic ARMv8 cache invalidate operation.
     * This function (typically using DC IVAC instructions) operates to the
     * Point of Coherency, ensuring stale data is removed.
     */
    invalidateCacheRange_RAM(start, end);
}

void plat_cleanInvalidateL2Range(paddr_t start, paddr_t end)
{
    /*
     * Use the generic ARMv8 cache clean and invalidate operation.
     * This function (typically using DC CIVAC instructions) operates to the
     * Point of Coherency.
     */
    cleanInvalidateCacheRange_RAM(start, end);
}
