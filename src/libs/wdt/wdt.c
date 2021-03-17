#include "wdt.h"

void wdt_init(uint8_t prescaler, uint8_t mode)
{
    cli();

    WDTCSR |= (1 << WDCE);	// Se habilita el cambio de prescaler del WATCHDOG

    WDTCSR = (WDTCSR & ~0x27) | prescaler;  // Se configura el prescaler

    switch (mode)
    {
        case WDT_MODE_IT:
            WDTCSR |= (1 << WDIE);

            break;

        case WDT_MODE_RESET:
            WDTCSR &= ~(1 << WDIE);

            WDTCSR |= (1 << WDE);

            break;

        case WDT_MODE_IT_RESET:
            WDTCSR |= (1 << WDIE);

            WDTCSR |= (1 << WDE);

            break;
    }

    sei();
}

void wdt_stop(void)
{
    WDTCSR |= (1 << WDCE);	// Se habilita el cambio de prescaler del WATCHDOG

    WDTCSR = 0b00010000;    // Se deshabilita toda la configuracion
}

ISR(WDT_vect)
{
    wdt_callback();
}

__attribute__ ((weak)) void wdt_callback(void)
{

}
