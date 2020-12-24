#ifndef WDT_H
#define WDT_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>

#include <inttypes.h>

#define WDT_MAX_TIME_16MS       0   // Tiempo de vencimiento de 16ms
#define WDT_MAX_TIME_32MS       1   // Tiempo de vencimiento de 32ms
#define WDT_MAX_TIME_64MS       2   // Tiempo de vencimiento de 64ms
#define WDT_MAX_TIME_125MS      3   // Tiempo de vencimiento de 125ms
#define WDT_MAX_TIME_250MS      4   // Tiempo de vencimiento de 250ms
#define WDT_MAX_TIME_500MS      5   // Tiempo de vencimiento de 500ms
#define WDT_MAX_TIME_1000MS     6   // Tiempo de vencimiento de 1000ms
#define WDT_MAX_TIME_2000MS     7   // Tiempo de vencimiento de 2000ms
#define WDT_MAX_TIME_4000MS     40  // Tiempo de vencimiento de 4000ms
#define WDT_MAX_TIME_8000MS     41  // Tiempo de vencimiento de 8000ms

#define WDT_MODE_IT             0   // Se reinicia si el WATCHDOG se activa
#define WDT_MODE_RESET          1   // Se interrumpe si el WATCHDOG se activa
#define WDT_MODE_IT_RESET       2   // Se interrumpe y reinicia si el WATCHDOG se activa

void wdt_init(uint8_t prescaler, uint8_t mode);	// Funcion que inicializa en WATCHDOG
void wdt_stop(void);    // Funcion que detiene el WATCHDOG

void wdt_callback(void);    // Callback del WATCHDOG

#endif
