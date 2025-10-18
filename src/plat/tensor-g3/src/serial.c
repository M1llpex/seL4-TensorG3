/*
 * Copyright 2025, Millpex
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include <config.h>
#include <types.h>
#include <plat/machine.h> // Include master platform header
#include <plat/serial.h>
#include <utils/io.h>

/*
 * Google Tensor G3 (Zuma) Serial Console Driver
 *
 * Hardware: Exynos UART (Samsung serial controller)
 * Source (Base Address): Tensor-G3.dts.txt / iomem.txt confirm 0x10870000.
 * Source (IRQ): Tensor-G3.dts.txt confirms IRQ 119.
 * Source (dmesg): console=ttySAC0,3000000n8, clk=200MHz
 */

/* Helper macro to access a UART register via its offset from the base address. */
#define UART_REG(reg_offset)    (*(volatile uint32_t *)(G3_UART_BASE + (reg_offset)))

/*
 * Print a single character to the serial console.
 */
void plat_serial_putchar(unsigned char c)
{
    /* Wait until the transmit buffer holding register is empty. */
    while (!(UART_REG(EXYNOS_UART_UTRSTAT) & UART_TRSTAT_TXB_EMPTY));

    /* Write the character to the transmit holding register. */
    UART_REG(EXYNOS_UART_UTXH) = c;
}

/*
 * Initialize the serial console.
 */
void plat_serial_init(void)
{
    /*
     * NOTE: The bootloader (ABL) is assumed to have already fully configured
     * the UART for a 3 Mbaud 8N1 console.
     *
     * We do not re-initialize the baud rate or line control.
     * For now, we do nothing and rely solely on the bootloader configuration.
     */
    // Optional: Reset FIFOs for a clean state
    // UART_REG(EXYNOS_UART_UFCON) |= (UART_FCON_RX_FIFO_RESET | UART_FCON_TX_FIFO_RESET);
}
