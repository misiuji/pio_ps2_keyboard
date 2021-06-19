

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/irq.h"
#include "hardware/pio.h"
#include "keyboard.pio.h"

#define DATA_PIN 14
#define CLOCK_PIN 15

#define SEND_DATA 30

PIO pio = pio1;
uint sm = 0;
unsigned int keycode_buffer = 0;
bool a_pressed = false, d_pressed = false, w_pressed = false;

void get_code()
{
    int got = pio_sm_get_blocking(pio, sm);
    int data = (got >> 22) & 0xff; //right justify

    keycode_buffer = keycode_buffer << 8;
    keycode_buffer &= 0xffffff00;
    keycode_buffer += data;
    bool not_break_code = !((keycode_buffer & 0x0000ff00) == 0xf000);
    switch (data)
    {
    case 0x1c: //a
        a_pressed = not_break_code;
        break;
    case 0x23: //d
        d_pressed = not_break_code;
        break;
        break;
    case 0x1d: //w
        w_pressed = not_break_code;
        break;

    default:
        break;
    }
}

int main()
{
    stdio_init_all();

    // Set up the state machine we're going to use to receive them.
    uint offset = pio_add_program(pio, &keyboard_pio_program);

    pio_sm_config c = pio_get_default_sm_config();
    sm_config_set_in_pins(&c, DATA_PIN);
    sm_config_set_in_shift(&c, true, true, 11);

    pio_sm_set_consecutive_pindirs(pio, sm, DATA_PIN, 2, false);
    pio_set_irq0_source_enabled(pio, pis_sm0_rx_fifo_not_empty, true);

    pio_sm_init(pio, sm, offset, &c);
    pio_sm_set_enabled(pio, sm, true);

    irq_set_exclusive_handler(PIO0_IRQ_0, &get_code);
    irq_set_enabled(PIO0_IRQ_0, true);

    printf("\nstarting\n");
    while (true)
    {
        printf("a: %i, d: %i, w: %i\n", a_pressed, d_pressed, w_pressed);
        busy_wait_ms(100);
    }
}
