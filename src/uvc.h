#ifndef UVC_H
#define UVC_H

#include "uvc_settings.h"

#include <inttypes.h>
#include <stdio.h>

#include <avr/io.h>
#include <avr/interrupt.h>

#include "ticker.h"
#include "hd44780.h"
#include "wdt.h"
#include "encoder.h"

#define UVC_UI_MSJ_STACK_SIZE				6		// Maxima cantidad de mensajes en cola

#define UVC_UI_MSJ_WELLCOME					0		// Mensaje de bienvenida
#define UVC_UI_MSJ_OPEN_DOOR				1		// Mensaje de puerta abierta
#define UVC_UI_MSJ_PROG_FINISH				2		// Mensaje de programa terminado

#define UVC_UI_MSJ_TYPE_ERROR				0		// Mensaje de error
#define UVC_UI_MSJ_TYPE_WARNG				1		// Mensaje de advertencia
#define UVC_UI_MSJ_TYPE_INFO				2		// Mensaje de información

#define UVC_UI_MENU_HOME					0		// Menu principal
#define UVC_UI_MENU_FAST_INIT				1		// Inicio rapido
#define UVC_UI_MENU_INIT_PROG				2		// Inicio de un programa

#define UVC_PROGRAMS_N						4		// Cantidad de programas

#define UVC_STATE_OFF						0		// Maquina apagada
#define UVC_STATE_TURNING_OFF				1		// Maquina por apagarse apagada
#define UVC_STATE_ON						2		// Maquina encendida
#define UVC_STATE_TURNING_ON				3		// Maquina por encenderse

#define UVC_ENCODER_DISABLE					0		// Deshabilita el uso del encoder
#define UVC_ENCODER_ENABLE					1		// Habilita el uso del encoder

// Macros
#define DDR(x) (*(&x - 1))   // Esta operación devuelve el registro de DDR en base al PORT
#define PIN(x) (*(&x - 2))   // Esta operación devuelve el registro de PIN en base al PORT

#define uvc_led_toggle() (UVC_LED_PORT ^= (1 << UVC_LED_PIN))	// Cambio de estado del led de estado

#define uvc_buzzer_off() (UVC_BUZZER_PORT &= ~(1 << UVC_BUZZER_PIN))	// Se apaga el buzzer
#define uvc_buzzer_on() (UVC_BUZZER_PORT |= (1 << UVC_BUZZER_PIN))	// Se enciende el buzzer

#define uvc_door_is_open() (((PIN(UVC_DOOR_OPEN_PORT) & (1 << UVC_DOOR_OPEN_PIN)) == 0) ? 0 : 1)	// Devuelve el estado de la puerta

#define uvc_door_close() (UVC_DOOR_LOCK_PORT |= (1 << UVC_DOOR_LOCK_PIN))	// Se cierra la puerta
#define uvc_door_open() (UVC_DOOR_LOCK_PORT &= ~(1 << UVC_DOOR_LOCK_PIN))	// Se abre la puerta

#define uvc_lamp_off() (UVC_LAMP_PORT |= (1 << UVC_LAMP_PIN))	// Se apagan las lamparas uvc
#define uvc_lamp_on() (UVC_LAMP_PORT &= ~(1 << UVC_LAMP_PIN))	// Se encienden las lamparas uvc

// Distintos programas de seleccion
typedef struct
{
	char name[21];
	uint8_t time;
}uvc_program_t;

// Mensajes emergentes
typedef struct
{
	char msj[21];
	char desc[21];
	uint8_t type;
	uint16_t time;
}uvc_ui_msj_t;

void uvc_init(void);    // Funcion que inicializa el comportamiento del UVC
void uvc_exec(void);	// Funcion que ejecuta el comportamieto del UVC

#endif
