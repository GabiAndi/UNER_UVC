#ifndef ADC_H
#define ADC_H

#include <avr/io.h>
#include <avr/interrupt.h>

#include <inttypes.h>

#define ADC_NO_USE              0       // No se configura
#define ADC_PIN_0               1       // Pin del ADC 0
#define ADC_PIN_1               2       // Pin del ADC 1
#define ADC_PIN_2               4       // Pin del ADC 2
#define ADC_PIN_3               8       // Pin del ADC 3
#define ADC_PIN_4               16      // Pin del ADC 4
#define ADC_PIN_5               32      // Pin del ADC 5

#define ADC_REF_AREF            0       // Refencia al pin AREF
#define ADC_REF_AVCC            1       // Refencia al pin AVCC
#define ADC_REF_INTERNAL        3       // Refencia interna de 1.1V

#define ADC_DIV_2               1       // Precaler de 2
#define ADC_DIV_4               2       // Precaler de 4
#define ADC_DIV_8               3       // Precaler de 8
#define ADC_DIV_16              4       // Precaler de 16
#define ADC_DIV_32              5       // Precaler de 32
#define ADC_DIV_64              6       // Precaler de 64
#define ADC_DIV_128             7       // Precaler de 128

#define ADC_CHANNEL_0           0       // Canal 0 del ADC
#define ADC_CHANNEL_1           1       // Canal 1 del ADC
#define ADC_CHANNEL_2           2       // Canal 2 del ADC
#define ADC_CHANNEL_3           3       // Canal 3 del ADC
#define ADC_CHANNEL_4           4       // Canal 4 del ADC
#define ADC_CHANNEL_5           5       // Canal 5 del ADC

// Estructura de interrupción de transmisión completa
typedef struct
{
    uint8_t adc_channel;
    uint16_t adc_value;
}adc_interrupt_request_t;

// Estructura de configuración
typedef struct
{
    uint8_t pin;
    uint8_t ref;
    uint8_t disable_digital;
    uint8_t prescaler;
}adc_config_t;

void adc_init(adc_config_t *adc_config);    // Funcion que inicializa el ADC
uint8_t adc_start(uint8_t adc_channel);	// Funcion que inicia la conversion del ADC
uint8_t adc_start_it(uint8_t adc_channel);    // Funcion que inicia la conversion del ADC con interrupción de conversion completa

ISR(ADC_vect);  // Interrupcion de conversion completa

void adc_callback(adc_interrupt_request_t *adc_interrupt_request);  // Funcion callback para conversion completa

#endif
