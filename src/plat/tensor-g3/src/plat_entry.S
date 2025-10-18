/*
 * platforms/plat/tensor-g3/src/plat_entry.S
 *
 * Copyright 2025, Millpex
 *
 * Initial entry point for the seL4 kernel on the tensor-g3 platform.
 */

#include <kernel/arch/asm_generic.S>
#include <kernel/platform_consts.S>

.global _start
.global plat_cpu_init_early

.text

/*
 * _start: Primary entry point for the kernel.
 * Source (dmesg.txt): Confirms bootloader starts CPUs in EL2.
 * Bootloader is expected to pass the physical address of the
 * Device Tree Blob (DTB) in register x0.
 */
_start:
    /* Only the primary core (core 0) performs the main initialization. */
    /* Read MPIDR - Multiprocessor Affinity Register */
    mrs     x1, mpidr_el1
    and     x1, x1, #MPIDR_CPU_MASK // Mask to get CPU ID (Aff0)
    cbnz    x1, hang_secondary_cores // If not CPU 0, branch to wait loop

    /*
     * Preserve the DTB pointer passed by the bootloader (usually in x0)
     * We need to pass this to kernel_main later. Stash it in x19 for now,
     * as x19 is a callee-saved register.
     */
    mov     x19, x0

    /*
     * Drop from EL2 to EL1. seL4 runs in EL1.
     */
    // Set up return address (in EL1) to el1_entry
    adr     x1, el1_entry
    msr     elr_el2, x1 // Exception Link Register (EL2)

    // Set SPSR_EL2: Saved Program Status Register (EL2)
    // EL1h: Target EL1 using SP_EL1 stack pointer
    // A, I, F: Mask asynchronous aborts, IRQ, FIQ during transition
    mov     x1, #(SPSR_EL_EL1h | SPSR_EL_A | SPSR_EL_I | SPSR_EL_F)
    msr     spsr_el2, x1

    // Set up the stack pointer for EL1
    ldr     x1, =kernel_stack_end
    mov     sp, x1

    // Perform exception return to EL1
    eret

el1_entry:
    /* Now executing in EL1 */

    /* Set up exception vector table for EL1 */
    adr     x0, vectors
    msr     vbar_el1, x0 // Vector Base Address Register (EL1)

    /*
     * Set up basic MMU configuration.
     * We assume a C function `init_boot_tables` exists, which creates
     * the initial page table mappings (e.g., identity mapping kernel code/data).
     * This function should return with the MMU configured but disabled.
     */
    bl      init_boot_tables

    /*
     * Enable MMU, instruction cache (I), data cache (C), and alignment checks (A).
     * Read the current System Control Register (EL1), modify bits, write back.
     */
    mrs     x0, sctlr_el1
    orr     x0, x0, #SCTLR_EL1_M // M: MMU enable
    orr     x0, x0, #SCTLR_EL1_I // I: Instruction cache enable
    orr     x0, x0, #SCTLR_EL1_C // C: Data cache enable
    orr     x0, x0, #SCTLR_EL1_A // A: Alignment check enable
    msr     sctlr_el1, x0
    isb // Instruction Synchronization Barrier

    /* Clear the BSS (Block Started by Symbol) section. */
    ldr     x0, =__bss_start__
    ldr     x1, =__bss_end__
clear_bss_loop:
    cmp     x0, x1
    b.ge    bss_cleared
    str     xzr, [x0], #8 // Store zero register, post-increment address by 8 bytes
    b       clear_bss_loop
bss_cleared:

    /* Perform early platform initialization (e.g., UART for debug prints). */
    bl      plat_serial_init

    /*
     * Jump to the main C kernel initialization function (kernel_main).
     * Pass the DTB address (stashed in x19) back into x0, the standard
     * argument register for the first parameter.
     */
    mov     x0, x19
    b       kernel_main

/*
 * Secondary cores will spin here until woken by the primary core.
 * Source (zuma-cpu.txt, Tensor-G3.dts.txt): Cores use PSCI enable-method,
 * suggesting the kernel will wake them via PSCI calls.
 */
hang_secondary_cores:
    wfe // Wait For Event
    b       hang_secondary_cores

/*
 * Exception vector table stub.
 * A real implementation would have handlers that save state and call C functions.
 * This basic version just loops infinitely if an exception occurs early.
 */
.align 11 // Align to 2^11 = 2048 bytes for VBAR
vectors:
    // Entries for Sync, IRQ, FIQ, SError exceptions at different origins.
    // Each handler branch should save context and jump to a C handler.
    b       . // Sync EL1t
    b       . // IRQ EL1t
    b       . // FIQ EL1t
    b       . // SError EL1t

    b       . // Sync EL1h
    b       . // IRQ EL1h
    b       . // FIQ EL1h
    b       . // SError EL1h

    b       . // Sync EL0_64
    b       . // IRQ EL0_64
    b       . // FIQ EL0_64
    b       . // SError EL0_64

    b       . // Sync EL0_32
    b       . // IRQ EL0_32
    b       . // FIQ EL0_32
    b       . // SError EL0_32

/*
 * plat_cpu_init_early: Called by the kernel on each core (including primary)
 * after basic C environment is set up, but before the scheduler runs.
 * This is where per-core hardware like the interrupt controller interface
 * and local timers are initialized.
 */
plat_cpu_init_early:
    /* Initialize the GIC CPU Interface and Redistributor for this core. */
    bl      plat_interrupt_controller_init
    /* Initialize and enable the ARM Generic Timer for this core. */
    bl      initTimer // Note: Renamed from plat_generic_timer_init to match C code
    ret
