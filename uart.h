// Definitions for Intel 8250 serial port (UART).

// Base addresses.
#define COM1    0x3f8
#define COM2    0x2f8
#define COM3    0x3e8
#define COM4    0x2e8

// Registers. Note that the same address can refer to different
// internal registers depending on read/write/DLAB (the "divisor
// latch access bit," see below).
#define UART_TRANSMIT_BUFFER    0	// write
#define UART_RECEIVE_BUFFER     0       // read
#define UART_DIVISOR_LOW        0       // read/write DLAB
#define UART_INTERRUPT_ENABLE   1       // read/write
#define UART_DIVISOR_HIGH       1       // read/write DLAB
#define UART_INTERRUPT_ID       2       // read
#define UART_FIFO_CONTROL       2       // write
#define UART_LINE_CONTROL       3       // read/write
#define UART_MODEM_CONTROL      4       // read/write
#define UART_LINE_STATUS        5       // read
#define UART_MODEM_STATUS       6       // read
#define UART_SCRATCH            7       // read/write

// Bits in the line control register.
#define UART_DIVISOR_LATCH      (1 << 7)        // Unlock divisor latch.

// Bits in the line status register.
#define UART_TRANSMIT_READY     (1 << 5)        // Can send more data.
#define UART_RECEIVE_READY      (1 << 0)        // Have received some data.

// Bits in the interrupt enable register.
#define UART_RECEIVE_INTERRUPT  (1 << 0)        // Have received some data.
