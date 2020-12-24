#include "adc.h"

static adc_interrupt_request_t adc_interrupt_request;   // Respuesta de una interrupcion

void adc_init(adc_config_t *adc_config)
{
	// Configuración de los pines del ADC
	DDRC &= ~(adc_config->pin);

	PORTC &= ~(adc_config->pin);

	// Se deshabilita el comportamiento digital de los pines siguientes
	DIDR0 |= adc_config->disable_digital;

	// Ahora se configura el modulo
	ADMUX |= (adc_config->ref << REFS0);	// Referencia con ajuste a la derecha

	ADCSRA |= (1 << ADEN) | adc_config->prescaler;	// ADC habilitado y prescaler de 128

	ADCSRB = 0; // Modo free runing
}

uint8_t adc_start(uint8_t adc_channel)
{
	// Si actualmente no hay una conversión en curso
	if ((ADCSRA & (1 << ADSC)) == 0)
	{
        // Se setea el canal correspondiente
        ADMUX = (ADMUX & ~0xf) | (adc_channel);

		// Se inicia la captura del ADC
		ADCSRA |= (1 << ADSC);

        return 1;
	}

    return 0;
}

uint8_t adc_start_it(uint8_t adc_channel)
{
    // Se habilita la interrupcion
    cli();

    ADCSRA |= (1 << ADIE);

    sei();

    // Devuelve el resultado del inicio de la conversion
    return adc_start(adc_channel);
}

ISR(ADC_vect)
{
    // Se obtienen los datos del ADC que dispararon la interrupcion
    adc_interrupt_request.adc_channel = ADMUX & 0xf;
    adc_interrupt_request.adc_value = ADC;

    adc_callback(&adc_interrupt_request);

    // De desactiva la interrupcion por conversion completa
    cli();

    ADCSRA &= ~(1 << ADIE);

    sei();
}

__attribute__ ((weak)) void adc_callback(adc_interrupt_request_t *adc_interrupt_request)
{

}
