#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/timer.h"
#include "hardware/structs/bus_ctrl.h"
#include "quadrature_decoder.h"
//#include "pin_ctrl.h"
#include <pin_ctrl.pio.h>

#define CMD_READ_X = 0x01
void pin_ctrl_run(PIO pio, uint sm, uint offset, uint pin);
void rfcomm_send_async(char data[30]);

void controller_sio_interrupt_handler()
{

    while (multicore_fifo_rvalid())
    {
        uint32_t cmd = multicore_fifo_pop_blocking();
        // switch (cmd) {
        //     case 0x01
        // }
        // printf("Data= %s\n", p);
        /*
           int32_t (*func)() = (int32_t(*)()) multicore_fifo_pop_blocking();
        int32_t p = multicore_fifo_pop_blocking();
        int32_t result = (*func)(p);
        multicore_fifo_push_blocking(result);
        */

        // const float conversion_factor = 3.3f / (1 << 12);
        // float result = raw * conversion_factor;
        // float temp = 27 - (result - 0.706)/0.001721;
        // printf("Temp = %f C\n", temp);
    }

    multicore_fifo_clear_irq(); // Clear interrupt
}

#define NCODER_A 9  // Pin Base
#define NCODER_B 10 // Pin Base + 1

quadrature_decoder ncoder;
int32_t ncoder_index;

int32_t quad_encoder_init()
{
    gpio_init(NCODER_A);
    gpio_init(NCODER_B);

    gpio_pull_down(NCODER_A);
    gpio_pull_down(NCODER_B);
    quadrature_decoder_init(&ncoder, pio0);
    return add_quadrature_decoder(&ncoder, NCODER_A);
}

int controller_run()
{

    stdio_init_all();
    ncoder_index = quad_encoder_init();

    //printf("Starting\n");
    // Run the pin_ctrl program
    //pin_ctrl_init();


    // add_repeating_timer_us(-25, repeating_timer_callback, NULL, &timer);

    absolute_time_t lastSampleTime = get_absolute_time();
    while (1)
    {
        absolute_time_t nextSampleTime = delayed_by_us(lastSampleTime, 100);
        while (get_absolute_time() < nextSampleTime)
        {
            sleep_us(10);
        }

        int32_t new_value;
        int32_t old_value;
        int32_t delta;
        new_value = get_count(&ncoder, ncoder_index);
        delta = new_value - old_value;
        old_value = new_value;

        printf("position %8d, delta %6d\n", new_value, delta);
        char data[30];
        static int counter = 0;

        // uint32_t raw = multicore_fifo_pop_blocking();
        char *p = &data[0];

        snprintf(p, sizeof(data), "X%d;Y%d;Z0;", new_value, delta);

        rfcomm_send_async(data);
        sleep_ms(100);
        // multicore_fifo_push_blocking(deltaCounter);

        //__wfi();
    }
}
