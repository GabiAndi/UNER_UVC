#ifndef UVC_SETTINGS_H
#define UVC_SETTINGS_H

#define UVC_UI_FPS				    8		// Tasa de refresco del UI

#define UVC_UI_MSJ_WELLCOME_TIME	4000	// Tiempo del mensaje de bienvenida

#define UVC_UI_MSJ_SHORT_TIME		6000	// Tiempo de mensaje corto
#define UVC_UI_MSJ_LONG_TIME		20000	// Tiempo de mensaje largo

#define UVC_LED_PORT				PORTC	// Puerto del LED de estado
#define UVC_LED_PIN					0		// Pin del LED de estado

#define UVC_LED_PERIOD				1000	// Velocidad de parpadeo del LED

#define UVC_BUZZER_PORT				PORTC	// Puerto para el buzzer
#define UVC_BUZZER_PIN				1		// Pin para el buzzer

#define UVC_BUZZER_ON_OFF_TIME		150		// Tiempo del pitido del buzzer encendido apagado
#define UVC_BUZZER_SELECT_TIME		50		// Tiempo del pitido del buzzer en seleccion
#define UVC_BUZZER_ROT_TIME		    3		// Tiempo del pitido del buzzer en giro
#define UVC_BUZZER_ERROR_TIME		800	    // Tiempo del pitido del buzzer error

#define UVC_DOOR_OPEN_PORT			PORTD	// Puerto para el sensor de puerta cerrada
#define UVC_DOOR_OPEN_PIN			5		// Pin para el sensor de puerta cerrada

#define UVC_LAMP_PORT				PORTD	// Puerto para la lampara
#define UVC_LAMP_PIN				6		// Pin para la lampara

#define UVC_DOOR_LOCK_PORT	    	PORTD	// Puerto para el cierre de la puerta
#define UVC_DOOR_LOCK_PIN			7		// Pin para el cierre de la puerta

#define UVC_DOOR_OPEN_SEC           2       // Tiempo en segundo que se abre la puerta

#endif