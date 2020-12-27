#include "encoder.h"

// Banderas de estado para los comportamientos del encoder
static volatile uint8_t encoder_clockwise = 0;
static volatile uint8_t encoder_counter_clockwise = 0;

void encoder_init(void)
{
    // Se iniciliza los pines como pullup
    DDR(ENCODER_PIN_A_PORT) &= ~(1 << ENCODER_PIN_A_PIN);

    ENCODER_PIN_A_PORT &= ~(1 << ENCODER_PIN_A_PIN);

    DDR(ENCODER_PIN_B_PORT) &= ~(1 << ENCODER_PIN_B_PIN);

    ENCODER_PIN_B_PORT &= ~(1 << ENCODER_PIN_B_PIN);

    DDR(ENCODER_PIN_SELECT_PORT) &= ~(1 << ENCODER_PIN_SELECT_PIN);

    ENCODER_PIN_SELECT_PORT &= ~(1 << ENCODER_PIN_SELECT_PIN);

    // Configuracion de las interrupciones
    cli();

    EICRA = 0x0A;   // Interrupcion por cambio de estado

    EIMSK = 0x03;   // se habilitan la interrupcion en ambos pines

    sei();
}

void encoder_exec(void)
{
    // Se verifica si se presiono el boton de seleccion
    if (encoder_pin_select_read())
    {
        encoder_select_callback();
    }

    // Se verifica si el encoder giro en sentido horario
    if (encoder_clockwise)
    {
        encoder_clockwise = 0;

        encoder_clockwise_callback();
    }

    // Se verifica si el encoder giro en sentido antihorario
    if (encoder_counter_clockwise)
    {
        encoder_counter_clockwise = 0;

        encoder_counter_clockwise_callback();
    }
}

__attribute__ ((weak)) void encoder_select_callback(void)
{

}

__attribute__ ((weak)) void encoder_clockwise_callback(void)
{

}

__attribute__ ((weak)) void encoder_counter_clockwise_callback(void)
{

}

ISR(INT0_vect)
{
    if (encoder_pin_b_read())
    {
        if (encoder_pin_a_read())
        {
            encoder_clockwise = 1;
        }

        else
        {
            encoder_counter_clockwise = 1;
        }
        
    }

    else
    {
        if (encoder_pin_a_read())
        {
            encoder_counter_clockwise = 1;
        }

        else
        {
            encoder_clockwise = 1;
        }
    }
}

ISR(INT1_vect)
{
    if (encoder_pin_a_read())
    {
        if (encoder_pin_b_read())
        {
            encoder_counter_clockwise = 1;
        }

        else
        {
            encoder_clockwise = 1;
        }
        
    }

    else
    {
        if (encoder_pin_b_read())
        {
            encoder_clockwise = 1;
        }

        else
        {
            encoder_counter_clockwise = 1;
        }
    }
}
