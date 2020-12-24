#ifndef TICKER_H
#define TICKER_H

#include <avr/io.h>
#include <avr/interrupt.h>

#include <stdint.h>

#define TICKER_MAX_USE                 	    10U	// Tickers como maximo

#define TICKER_LOW_PRIORITY					0U	// Prioridad baja (se ejecuta en blucle principal)
#define TICKER_HIGH_PRIORITY				1U	// Prioridad alta (se ejecuta en interrupcion)

#define TICKER_NO_ACTIVE					0U	// Ticker esta desactivado
#define TICKER_ACTIVE						1U	// Ticker esta activo

typedef void(*ticker_ptrfun)(void);	// Función a ser llamada en el ticker

typedef struct
{
	ticker_ptrfun ticker_function;		// Puntero a función que será llamada en el ticker
	uint32_t ms_max;					// Tiempo cada cuanto se ejecutará el ticker
	volatile uint32_t ms_count;			// Tiempo actual del ticker
	volatile uint16_t calls;			// Cantidad de veces que se llamo al ticker
	uint8_t state;						// Estado actual del ticker
	uint8_t priority;					// Prioridad del ticker
}ticker_t;

void ticker_init(void);
uint8_t ticker_new(ticker_t *ticker);
uint8_t ticker_delete(ticker_t *ticker);
uint8_t ticker_exec(void);

ISR(TIMER0_OVF_vect);
#endif
