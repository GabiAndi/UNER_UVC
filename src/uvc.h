#ifndef UVC_H
#define UVC_H

#include <inttypes.h>
#include <stdio.h>

#include "ticker.h"
#include "hd44780.h"
#include "adc.h"
#include "wdt.h"

#define LED_PERIOD					1000	// Velocidad de parpadeo del LED

#define LED_PORT					PORTB	// Puerto del LED de estado
#define LED_PIN						5		// Pin del LED de estado

#define LCD_BACKLIGHT_PORT			PORTB	// Puerto del backlight del LCD
#define LCD_BACKLIGHT_PIN			2		// Pin del backlight del LCD

#define BUTTON_DISABLED				0		// Botones deshabilitados
#define BUTTON_ENABLED				1		// Botones habilitados

#define BUTTON_IDLE					0		// Boton sin ser presionado
#define BUTTON_PENDING				1		// Boton pendiente

#define BUTTON_REFRESH_TIME			50		// Frecuencia de muestreo de los botones

#define BUTTON_ANTIBOUNCE_MOVE		400		// Tiempo para los botones de movimiento
#define BUTTON_ANTIBOUNCE_SELECT	800		// Tiempo para los botones de selección

#define UI_REFRESH					4		// Tasa de refresco del UI

#define UI_MODE_PROG_SELEC			0		// Seleccón de programa
#define UI_MODE_ON_OFF				1		// Temporización

#define UI_MSJ_N					3		// Numero de mensajes
#define UI_MSJ_STACK_SIZE			4		// Maxima cantidad de mensajes en cola

#define UI_MSJ_WELLCOME				0		// Mensaje de bienvenida
#define UI_MSJ_OPEN_DOOR			1		// Mensaje de puerta abierta
#define UI_MSJ_PROG_FINISH			2		// Mensaje de programa terminado

#define UI_MSJ_WELLCOME_TIME		4000	// Tiempo del mensaje de bienvenida

#define UI_MSJ_TYPE_ERROR			0		// Mensaje de error
#define UI_MSJ_TYPE_WARNG			1		// Mensaje de advertencia
#define UI_MSJ_TYPE_INFO			2		// Mensaje de información

#define UI_MSJ_SHORT_TIME			6000	// Tiempo de mensaje corto
#define UI_MSJ_LONG_TIME			20000	// Tiempo de mensaje largo

#define UVC_PROGRAMS_N				5		// Cantidad de programas

#define UVC_BUZZER_PORT				PORTB	// Puerto para el sbuzzer
#define UVC_BUZZER_PIN				3		// Pin para el buzzer

#define UVC_BUZZER_ON_OFF_TIME		250		// Tiempo del pitido del buzzer encendido apagado
#define UVC_BUZZER_ERROR_TIME		1400	// Tiempo del pitido del buzzer error

#define UVC_DOOR_OPEN_TRIGGER		400		// Umbral de puerta cerrada

#define UVC_LAMP_PORT				PORTB	// Puerto para la lampara
#define UVC_LAMP_PIN				4		// Pin para la lampara

#define UVC_STATE_OFF				0		// Maquina apagada
#define UVC_STATE_TURNING_OFF		1		// Maquina por apagarse apagada
#define UVC_STATE_ON				2		// Maquina encendida
#define UVC_STATE_TURNING_ON		3		// Maquina por encenderse

// Macros
#define DDR(x) (*(&x - 1))   // Esta operación devuelve el registro de DDR en base al PORT
#define PIN(x) (*(&x - 2))   // Esta operación devuelve el registro de PIN en base al PORT

#define lcd_backlight_off() (LCD_BACKLIGHT_PORT &= ~(1 << LCD_BACKLIGHT_PIN))
#define lcd_backlight_on() (LCD_BACKLIGHT_PORT |= (1 << LCD_BACKLIGHT_PIN))

#define led_toggle() (LED_PORT ^= (1 << LED_PIN))

#define uvc_lamp_off() (UVC_LAMP_PORT |= (1 << UVC_LAMP_PIN))
#define uvc_lamp_on() (UVC_LAMP_PORT &= ~(1 << UVC_LAMP_PIN))

// Distintos programas de seleccion
typedef struct
{
	char name[17];
	char desc[17];
	uint8_t time;
}uvc_program_t;

// Mensajes emergentes
typedef struct
{
	char msj[17];
	uint8_t type;
	uint16_t time;
}ui_msj_t;

void uvc_init(void);    // Funcion que inicializa el comportamiento del UVC
void uvc_exec(void);	// Funcion que ejecuta el comportamieto del UVC

#endif
