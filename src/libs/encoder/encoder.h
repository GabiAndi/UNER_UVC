#ifndef ENCODER_H
#define ENCODER_H

#include "encoder_settings.h"

#include <avr/io.h>
#include <avr/interrupt.h>

// Macros de configuracion
#define PIN(x) (*(&x - 2))  // Macro que devuelve el registro del PIN a partir del PORT
#define DDR(x) (*(&x - 1))  // Macro que devuelve el registro del DDR a partir del PORT

#define encoder_pin_select_read() (((PIN(ENCODER_PIN_SELECT_PORT) & (1 << ENCODER_PIN_SELECT_PIN)) == 0) ? 1 : 0)

#define encoder_pin_a_read() (((PIN(ENCODER_PIN_A_PORT) & (1 << ENCODER_PIN_A_PIN)) == 0) ? 0 : 1)
#define encoder_pin_b_read() (((PIN(ENCODER_PIN_B_PORT) & (1 << ENCODER_PIN_B_PIN)) == 0) ? 0 : 1)

void encoder_init(void);    // Funcion que inicializa el comportamiento del encoder
void encoder_exec(void);    // Funcion que ejecuta el comportamiento del encoder

void encoder_select_callback(void); // Callback para la accion de boton de seleccion
void encoder_clockwise_callback(void);  // Callback para movimiento en sentido horario
void encoder_counter_clockwise_callback(void);  // Callback para movimiento en sentido antihorario

#endif
