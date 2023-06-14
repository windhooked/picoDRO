
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include "btstack_run_loop.h"
#include "pico/multicore.h"
#include "hardware/irq.h"

extern  int btstack_main(int argc, const char *argv[]);
void rfcomm_send_async(char data[30]);

// Core 1 interrupt Handler
void broker_sio_interrupt_handler()
{

    // Receive Raw Value, Convert and Print Temperature Value

    char data[30];
    static int counter = 0;

    while (multicore_fifo_rvalid())
    {
        uint32_t raw = multicore_fifo_pop_blocking();
        char *p = &data[0];
        snprintf(p, sizeof(data), "X%d;Y%d;Z0;", raw, ++counter);
        rfcomm_send_async(data);
        // printf("Data= %s\n", p);

        // const float conversion_factor = 3.3f / (1 << 12);
        // float result = raw * conversion_factor;
        // float temp = 27 - (result - 0.706)/0.001721;
        // printf("Temp = %f C\n", temp);
    }

    multicore_fifo_clear_irq(); // Clear interrupt
}

void broker_run()
{
    // initialize CYW43 driver
    if (cyw43_arch_init())
    {
        printf("cyw43_arch_init() failed.\n");
        return;
    }
    // Configure Core 1 Interrupt
    multicore_fifo_clear_irq();
    irq_set_exclusive_handler(SIO_IRQ_PROC1, broker_sio_interrupt_handler);

    irq_set_enabled(SIO_IRQ_PROC1, true);

    // run the app
    // https://bluekitchen-gmbh.com/btstack/v1.0/examples/examples/
    btstack_main(0, NULL);
    // btstack_run_loop_
    btstack_run_loop_execute();
}
