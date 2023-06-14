#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/sync.h"
#include "hardware/dma.h"
#include "hardware/irq.h"
#include "pin_ctrl.pio.h"


const uint TEST_PIN_BASE = 2;

const uint CAPTURE_PIN_BASE = 2;
const uint CAPTURE_PIN_COUNT = 2;
const uint CAPTURE_N_SAMPLES = 1000;


// Ensure `timing_buffer` is aligned to 16-bytes so we can use DMA address
// wrapping
uint32_t timing_buffer[4] __attribute__((aligned (16))) = {
    (4 << 2) | 0x1,
    (3 << 2) | 0x2,
    (8 << 2) | 0x3,
    (5 << 2) | 0x0
};

uint32_t pio_dma_chan;

void __not_in_flash_func(dma_irh)() {
    dma_hw->ints0 = (1u << pio_dma_chan);
    dma_hw->ch[pio_dma_chan].al3_read_addr_trig = timing_buffer;
}

int pin_ctrl_init()
{

    PIO pio = pio0;

    // Load the pin_ctrl program into the PIO
    uint offset = pio_add_program(pio, &pin_ctrl_program);


    // Run the pin_ctrl program
    pin_ctrl_run(pio, 0, offset, TEST_PIN_BASE);


//    while(1) {
//        __wfi();
//    }

    return 0;
}

void pin_ctrl_program_init(PIO pio, uint sm, uint offset, uint pin) {
    // Setup pin and pin+1 to be accessible from the PIO
    pio_gpio_init(pio, pin);
    pio_gpio_init(pio, pin+1);

    // Setup PIO SM to control pin and pin + 1
    pio_sm_set_consecutive_pindirs(pio, sm, pin, 2, true);

    // Configure PIO SM with pin_ctrl program
    pio_sm_config c = pin_ctrl_program_get_default_config(offset);
    // pin and pin + 1 are controlled by `out pins, ...` instructions
    sm_config_set_out_pins(&c, pin, 2);
    sm_config_set_clkdiv(&c, 1.0f);
    // Join FIFOs together to get an 8 entry TX FIFO
    sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_TX);
    pio_sm_init(pio, sm, offset, &c);
}

void pin_ctrl_run(PIO pio, uint sm, uint offset, uint pin) {
    // Allocate a DMA channel to feed the pin_ctrl SM its command words
    pio_dma_chan = dma_claim_unused_channel(true);

    dma_channel_config pio_dma_chan_config = dma_channel_get_default_config(pio_dma_chan);
    // Transfer 32 bits each time
    channel_config_set_transfer_data_size(&pio_dma_chan_config, DMA_SIZE_32);
    // Increment read address (a different command word from `timing_buffer`
    // each time)
    channel_config_set_read_increment(&pio_dma_chan_config, true);
    // Write to the same address (the PIO SM TX FIFO)
    channel_config_set_write_increment(&pio_dma_chan_config, false);
    // Set read address to wrap on a 16-byte boundary
    channel_config_set_ring(&pio_dma_chan_config, false, 4);
    // Transfer when PIO SM TX FIFO has space
    channel_config_set_dreq(&pio_dma_chan_config, pio_get_dreq(pio, sm, true));

    // Setup the channel and set it going
    dma_channel_configure(
        pio_dma_chan,
        &pio_dma_chan_config,
        &pio->txf[sm], // Write to PIO TX FIFO
        timing_buffer, // Read values from timing buffer
        16, // `timing_buffer` has 4 entries, so 16 will go through it 4 times
        false // don't start yet
    );

    // Setup IRQ for DMA transfer end
    dma_channel_set_irq0_enabled(pio_dma_chan, true);
    irq_set_exclusive_handler(DMA_IRQ_0, dma_irh);
    irq_set_enabled(DMA_IRQ_0, true);

    // Initialise PIO SM with pin_ctrl program
    pin_ctrl_program_init(pio, sm, offset, pin);
    // Start the DMA (must do this after program init or DMA won't do anything)
    dma_channel_start(pio_dma_chan);
    // Start the PIO
    pio_sm_set_enabled(pio, sm, true);
}