/*
 * Copyright 2025, Millpex
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include <config.h>
#include <types.h>
#include <plat/machine.h> // Include master platform header
#include <plat/gic.h>
#include <arch/machine/gic_v3.h>
#include <utils/io.h>

/*
 * Initializes the GICv3 Distributor. This is done once by the primary core.
 * Source (Hardware): Tensor-G3.dts.txt & dmesg.txt confirm a GICv3.
 * Source (Address): Tensor-G3.dts.txt & /proc/iomem confirm GICD_BASE.
 */
void plat_gic_init(void)
{
    /*
     * The GIC driver in seL4 (`gic_v3.c`) takes the distributor base address
     * (defined in plat/gic.h) and handles the full initialization sequence.
     */
    gic_v3_dist_init(GICD_BASE);
}

/*
 * Initializes the per-core GIC components (Redistributor and CPU Interface).
 * Source (Address): Tensor-G3.dts.txt & /proc/iomem confirm GICR_BASE.
 */
void plat_interrupt_controller_init(void)
{
    /*
     * The seL4 driver handles initializing the per-CPU interfaces. It needs
     * the base of the redistributor region (defined in plat/gic.h) for this core.
     */
    gic_v3_cpu_interface_init(GICR_BASE);
}
