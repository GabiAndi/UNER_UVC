#include "uvc.h"

// Tickers
static ticker_t ticker_led_blink;	            // Ticker para el LED
static ticker_t ticker_adc_convert;	        	// Ticker para la captura de datos del ADC
static ticker_t ticker_button_antibounce;	    // Ticker para evitar multiples pulsaciones de boton
static ticker_t ticker_ui_exec;	            	// Ticker del UI
static ticker_t ticker_ui_msj;					// Ticker para mostrar mensajes emergentes
static ticker_t ticker_uvc_timer;	            // Ticker del timer del UVC
static ticker_t ticker_uvc_buzzer;				// Ticker para el buzzer

// Control de LCD
static char lcd_buffer[LCD_ROWS][LCD_COLS + 1];				// Buffer para el LCD

static volatile uint8_t buttons[5];							// Flags para los botones
static volatile uint8_t button_enable = BUTTON_ENABLED;		// Bandera que indica si los botones estan habilitados para ser pulsados

// Control del ADC
static uint8_t adc_convertion_sec[2] =
{
	ADC_CHANNEL_0,
	ADC_CHANNEL_1
};												// Secuencia de conversiones

static uint8_t adc_convertion_sec_index = 0;	// Indice para las conversiones

// Interfaz de usuario
static uint8_t ui_menu_mode = UI_MODE_PROG_SELEC;		// Indica el estado de seleccion del menu
static uint8_t ui_program_index = 0;					// Indice de selección
static uint8_t ui_msj_stack[UI_MSJ_STACK_SIZE];			// Stack de mensajes
static uint8_t ui_msj_stack_index = 0;					// Posición del stack

static ui_msj_t ui_msjs[] = 
{
	{
		" Bienvenido UVC ",
		UI_MSJ_TYPE_INFO,
		UI_MSJ_WELLCOME_TIME
	},

	{
		" Puerta abierta ",
		UI_MSJ_TYPE_ERROR,
		UI_MSJ_LONG_TIME
	},

	{
		"   Finalizado   ",
		UI_MSJ_TYPE_INFO,
		UI_MSJ_SHORT_TIME
	}
};														// Mensajes que pueden surgir

// Control de la maquina
static uint8_t uvc_state = UVC_STATE_OFF;					// Estado de la maquina
static uint8_t uvc_time = 0;								// Segundos del temporizador

static volatile uint16_t uvc_door_open_sensor_value = 0;	// Valor del sensor de efecto HALL

// Programas de tiempo
static uvc_program_t uvc_programs[] = 
{
	{
		"   SARS-CoV-2   ",
		"   (Covid 19)   ",
		40
	},

	{
		" S. cerevisiae  ",
		"   (Levadura)   ",
		60
	},

	{
		"    S. aureus   ",
		"   (Bacteria)   ",
		45
	},

	{
		"     E. coli    ",
		"   (Bacteria)   ",
		45
	},

	{
		"      Otro      ",
		"(Personalizado) ",
		30
	}
};													// Tiempos programados

static void gpio_init(void);					// Funcion de inicializacion del GPIO
static void led_blink(void);					// Función de parpadeo para el LED de estado
static void adc_start_convertion(void);			// Funcion que dispara la conversion del ADC
static void button_enabled(void);				// Funcion que habilita el boton
static void button_capture(void);				// Funcion que captura las pulsaciones de los botones
static void ui_exec(void);						// Funcion que se encarga del UI
static void ui_msj(void);						// Funcion para mostrar mensajes emergentes
static void ui_msj_show(uint8_t msj_id);		// Función que agrega un mensaje en el stack
static void uvc_timer(void);					// Funcion para decrementar el contador del timer del UVC
static void uvc_manager(void);					// Funcion que controla el comportamiento de la maquina
static void uvc_buzzer_init(void);				// Funcion que configura el buzzer
static void uvc_buzzer_on(uint16_t ms);			// Funcion que inicia el pitido del buzzer
static void uvc_buzzer_off(void);				// Funcion que corta el pitido del buzzer

void uvc_init(void)
{
    // Se inicializa el GPIO del microcontrolador
	gpio_init();

	// Se inicia el LCD
	lcd_init();
	lcd_on();

	// Se inicia el modulo de tickers
	ticker_init();

	// Se configura el ADC
	adc_config_t adc_config;

	adc_config.pin = ADC_PIN_0 | ADC_PIN_1;
	adc_config.ref = ADC_REF_AVCC;
	adc_config.prescaler = ADC_DIV_128;
	adc_config.disable_digital = ADC_PIN_0 | ADC_PIN_1 | ADC_PIN_2 | ADC_PIN_3 | ADC_PIN_4 | ADC_PIN_5;

	adc_init(&adc_config);
	
	// Se configura el WATCHDOG
	wdt_init(WDT_MAX_TIME_125MS, WDT_MODE_IT_RESET);

	// Se inicializa el buzzer
	uvc_buzzer_init();

	// Se configura el ticker del LED
	ticker_led_blink.calls = 0;
	ticker_led_blink.ms_count = 0;
	ticker_led_blink.ms_max = LED_PERIOD;
	ticker_led_blink.priority = TICKER_LOW_PRIORITY;
	ticker_led_blink.state = TICKER_ACTIVE;
	ticker_led_blink.ticker_function = led_blink;

	ticker_new(&ticker_led_blink);

	// Se configura el ticker del ADC
	ticker_adc_convert.calls = 0;
	ticker_adc_convert.ms_count = 0;
	ticker_adc_convert.ms_max = BUTTON_REFRESH_TIME;
	ticker_adc_convert.priority = TICKER_LOW_PRIORITY;
	ticker_adc_convert.state = TICKER_ACTIVE;
	ticker_adc_convert.ticker_function = adc_start_convertion;

	ticker_new(&ticker_adc_convert);

	// Se configura el ticker del antibounce
	ticker_button_antibounce.calls = 0;
	ticker_button_antibounce.ms_count = 0;
	ticker_button_antibounce.ms_max = BUTTON_ANTIBOUNCE_MOVE;
	ticker_button_antibounce.priority = TICKER_LOW_PRIORITY;
	ticker_button_antibounce.state = TICKER_NO_ACTIVE;
	ticker_button_antibounce.ticker_function = button_enabled;

	ticker_new(&ticker_button_antibounce);

	// Se configura el ticker del UI
	ticker_ui_exec.calls = 0;
	ticker_ui_exec.ms_count = 0;
	ticker_ui_exec.ms_max = 1000 / UI_REFRESH;
	ticker_ui_exec.priority = TICKER_LOW_PRIORITY;
	ticker_ui_exec.state = TICKER_ACTIVE;
	ticker_ui_exec.ticker_function = ui_exec;

	ticker_new(&ticker_ui_exec);

	// Se configura el ticker para los mensajes
	ticker_ui_msj.calls = 0;
	ticker_ui_msj.ms_count = 0;
	ticker_ui_msj.ms_max = UI_MSJ_LONG_TIME;
	ticker_ui_msj.priority = TICKER_LOW_PRIORITY;
	ticker_ui_msj.state = TICKER_NO_ACTIVE;
	ticker_ui_msj.ticker_function = ui_msj;

	ticker_new(&ticker_ui_msj);

	// Se configura el ticker del timer del UVC
	ticker_uvc_timer.calls = 0;
	ticker_uvc_timer.ms_count = 0;
	ticker_uvc_timer.ms_max = 1000;
	ticker_uvc_timer.priority = TICKER_LOW_PRIORITY;
	ticker_uvc_timer.state = TICKER_NO_ACTIVE;
	ticker_uvc_timer.ticker_function = uvc_timer;

	ticker_new(&ticker_uvc_timer);

	// Se configura el ticker del buzzer del UVC
	ticker_uvc_buzzer.calls = 0;
	ticker_uvc_buzzer.ms_count = 0;
	ticker_uvc_buzzer.ms_max = 1000;
	ticker_uvc_buzzer.priority = TICKER_LOW_PRIORITY;
	ticker_uvc_buzzer.state = TICKER_NO_ACTIVE;
	ticker_uvc_buzzer.ticker_function = uvc_buzzer_off;

	ticker_new(&ticker_uvc_buzzer);

	// Mostrar mensaje de bienvenida
	ui_msj_show(UI_MSJ_WELLCOME);
}

void uvc_exec(void)
{
	// Se capturan los botones
	button_capture();

	// Eventos y toma de deciciones
	uvc_manager();

    // Se ejecutan los tickers
    ticker_exec();

    // Se resetea al WATCHDOG
    wdt_reset();
}

static void gpio_init(void)
{
	// Configuración del LED
	DDR(LED_PORT) |= (1 << LED_PIN);

	// Configuracion del backlight del LCD
	DDR(LCD_BACKLIGHT_PORT) |= (1 << LCD_BACKLIGHT_PIN);
	
	lcd_backlight_on();

	// Configuracion de las lamparas
	DDR(UVC_LAMP_PORT) |= (1 << UVC_LAMP_PIN);

	UVC_LAMP_PORT |= (1 << UVC_LAMP_PIN);
}

static void led_blink(void)
{
	// Cambio de estado del led
	led_toggle();
}

static void adc_start_convertion(void)
{
	adc_start_it(adc_convertion_sec[adc_convertion_sec_index]);

	adc_convertion_sec_index++;

	if (adc_convertion_sec_index > 1)
	{
		adc_convertion_sec_index = 0;
	}
}

static void button_enabled(void)
{
	button_enable = BUTTON_ENABLED;

	ticker_button_antibounce.state = TICKER_NO_ACTIVE;
}

static void button_capture(void)
{
	// Si se esta capturando los botones
	if (button_enable == BUTTON_ENABLED)
	{
		// Se comprueba si un botton fue presionado
		// Boton derecha
		if (buttons[0] == BUTTON_PENDING)
		{
			// Se setea la captura
			buttons[0] = BUTTON_IDLE;
			button_enable = BUTTON_DISABLED;
			ticker_button_antibounce.ms_count = 0;
			ticker_button_antibounce.ms_max = BUTTON_ANTIBOUNCE_SELECT;
			ticker_button_antibounce.state = TICKER_ACTIVE;

			// Operaciones del botton
			if (ui_menu_mode == UI_MODE_PROG_SELEC)
			{
				ui_menu_mode = UI_MODE_ON_OFF;

				uvc_time = uvc_programs[ui_program_index].time;
			}
		}

		// Boton arriba
		else if (buttons[1] == BUTTON_PENDING)
		{
			// Se setea la captura
			buttons[1] = BUTTON_IDLE;
			button_enable = BUTTON_DISABLED;
			ticker_button_antibounce.ms_count = 0;
			ticker_button_antibounce.ms_max = BUTTON_ANTIBOUNCE_MOVE;
			ticker_button_antibounce.state = TICKER_ACTIVE;

			// Operaciones del botton
			if (ui_menu_mode == UI_MODE_PROG_SELEC)
			{
				if (ui_program_index < (UVC_PROGRAMS_N - 1))
				{
					ui_program_index++;
				}

				else
				{
					ui_program_index = 0;
				}
			}

			else if ((uvc_state == UVC_STATE_OFF) && (ui_menu_mode == UI_MODE_ON_OFF))
			{
				if (uvc_time < 255)
				{
					uvc_time++;
				}
				
				else
				{
					uvc_time = 0;
				}
			}
		}
		
		// Boton abajo
		else if (buttons[2] == BUTTON_PENDING)
		{
			// Se setea la captura
			buttons[2] = BUTTON_IDLE;
			button_enable = BUTTON_DISABLED;
			ticker_button_antibounce.ms_count = 0;
			ticker_button_antibounce.ms_max = BUTTON_ANTIBOUNCE_MOVE;
			ticker_button_antibounce.state = TICKER_ACTIVE;

			// Operaciones del botton
			if (ui_menu_mode == UI_MODE_PROG_SELEC)
			{
				if (ui_program_index > 0)
				{
					ui_program_index--;
				}

				else
				{
					ui_program_index = UVC_PROGRAMS_N - 1;
				}
			}

			else if ((uvc_state == UVC_STATE_OFF) && (ui_menu_mode == UI_MODE_ON_OFF))
			{
				if (uvc_time > 0)
				{
					uvc_time--;
				}

				else
				{
					uvc_time = 255;
				}
			}
		}

		// Boton izquierda
		else if (buttons[3] == BUTTON_PENDING)
		{
			// Se setea la captura
			buttons[3] = BUTTON_IDLE;
			button_enable = BUTTON_DISABLED;
			ticker_button_antibounce.ms_count = 0;
			ticker_button_antibounce.ms_max = BUTTON_ANTIBOUNCE_SELECT;
			ticker_button_antibounce.state = TICKER_ACTIVE;

			// Operaciones del botton
			if ((ui_menu_mode == UI_MODE_ON_OFF) && (ui_msj_stack_index == 0) && (uvc_state == UVC_STATE_OFF))
			{
				ui_menu_mode = UI_MODE_PROG_SELEC;
			}
		}

		// Boton de seleccion
		else if (buttons[4] == BUTTON_PENDING)
		{
			// Se setea la captura
			buttons[4] = BUTTON_IDLE;
			button_enable = BUTTON_DISABLED;
			ticker_button_antibounce.ms_count = 0;
			ticker_button_antibounce.ms_max = BUTTON_ANTIBOUNCE_SELECT;
			ticker_button_antibounce.state = TICKER_ACTIVE;

			// Operaciones del botton
			if ((ui_menu_mode == UI_MODE_ON_OFF) && (uvc_time > 0) && (ui_msj_stack_index == 0))
			{
				if (uvc_state == UVC_STATE_ON)
				{
					uvc_state = UVC_STATE_TURNING_OFF;

					uvc_buzzer_on(UVC_BUZZER_ON_OFF_TIME);
				}

				else if (uvc_state == UVC_STATE_OFF)
				{
					uvc_state = UVC_STATE_TURNING_ON;

					uvc_buzzer_on(UVC_BUZZER_ON_OFF_TIME);
				}
			}

			else if (ui_msj_stack_index > 0)
			{
				ui_msj();
			}
		}
	}
}

static void ui_exec(void)
{
	// Formateo del menu a mostrar si no hay mensajes pendientes
	if (ui_msj_stack_index == 0)
	{
		switch (ui_menu_mode)
		{
			case UI_MODE_PROG_SELEC:
				sprintf(lcd_buffer[0], "%s", uvc_programs[ui_program_index].name);
				sprintf(lcd_buffer[1], "%s", uvc_programs[ui_program_index].desc);

				break;
			
			case UI_MODE_ON_OFF:
				if (uvc_state == UVC_STATE_ON)
				{
					sprintf(lcd_buffer[0], "  Lamparas ON   ");
				}

				else if (uvc_state == UVC_STATE_OFF)
				{
					sprintf(lcd_buffer[0], "  Lamparas OFF  ");
				}
				
				if (uvc_time < 10)
				{
					sprintf(lcd_buffer[1], "  Tiempo: %useg  ", uvc_time);
				}
				
				else
				{
					sprintf(lcd_buffer[1], "  Tiempo: %useg ", uvc_time);
				}

				break;
		}
	}

	else
	{
		switch (ui_msjs[ui_msj_stack[ui_msj_stack_index]].type)
		{
			case UI_MSJ_TYPE_ERROR:
				sprintf(lcd_buffer[0], "      ERROR     ");

				break;

			case UI_MSJ_TYPE_WARNG:
				sprintf(lcd_buffer[0], "     WARNING    ");

				break;

			case UI_MSJ_TYPE_INFO:
				sprintf(lcd_buffer[0], "      INFO      ");

				break;
		}

		sprintf(lcd_buffer[1], "%s", ui_msjs[ui_msj_stack[ui_msj_stack_index]].msj);
	}
	

	// Se escribe la linea 1 del buffer
	lcd_gotoxy(0, 0);
	lcd_puts(lcd_buffer[0]);

	// Se escribe la linea 2 del buffer
	lcd_gotoxy(0, 1);
	lcd_puts(lcd_buffer[1]);
}

static void ui_msj(void)
{
	ui_msj_stack_index--;

	if (ui_msj_stack_index == 0)
	{
		ticker_ui_msj.state = TICKER_NO_ACTIVE;
	}

	else
	{
		ticker_ui_msj.ms_max = ui_msjs[ui_msj_stack[ui_msj_stack_index]].time;
	}
}

static void ui_msj_show(uint8_t msj_id)
{
	ui_msj_stack_index++;
	ui_msj_stack[ui_msj_stack_index] = msj_id;

	ticker_ui_msj.ms_count = 0;
	ticker_ui_msj.ms_max = ui_msjs[msj_id].time;
	ticker_ui_msj.state = TICKER_ACTIVE;
}

static void uvc_timer(void)
{
	if (uvc_time > 0)
	{
		uvc_time--;
	}

	else
	{
		uvc_state = UVC_STATE_TURNING_OFF;
	}
}

static void uvc_manager(void)
{
	// Comportamiento segun si las lamparas estan encendidas o apagadas
	switch (uvc_state)
	{
		case UVC_STATE_TURNING_ON:
			ticker_uvc_timer.ms_count = 0;
			ticker_uvc_timer.state = TICKER_ACTIVE;

			uvc_state = UVC_STATE_ON;

			break;
		
		case UVC_STATE_ON:
			if (uvc_door_open_sensor_value >= UVC_DOOR_OPEN_TRIGGER)
			{
				uvc_state = UVC_STATE_TURNING_OFF;

				ui_msj_show(UI_MSJ_OPEN_DOOR);

				uvc_buzzer_on(UVC_BUZZER_ERROR_TIME);
			}

			uvc_lamp_on();

			break;

		case UVC_STATE_TURNING_OFF:
			ticker_uvc_timer.state = TICKER_NO_ACTIVE;

			uvc_state = UVC_STATE_OFF;

			// Si el programa se termino
			if (uvc_time == 0)
			{
				ui_msj_show(UI_MSJ_PROG_FINISH);

				uvc_buzzer_on(UVC_BUZZER_ON_OFF_TIME);
			}

			break;

		case UVC_STATE_OFF:
			uvc_lamp_off();

			break;
	}
}

static void uvc_buzzer_init(void)
{
	// Se configura el pin a utilizar
    DDR(UVC_BUZZER_PORT) |= (1 << UVC_BUZZER_PIN);

    UVC_BUZZER_PORT &= ~(1 << UVC_BUZZER_PIN);

	// Timer se configura
    cli();

    TCCR2A = 0b00000010;    // Modo CTC
    
    TCCR2B = 0b10000000;    // Prescaler de 64

    TIMSK2 = 0b00000010;    // Interrupcion de comparación

    OCR2A = 60;

    TCNT2 = 0;

    sei();
}

static void uvc_buzzer_on(uint16_t ms)
{
	// Se activa el timer
	TCCR2B |= (1 << CS22);

	TCNT2 = 0;

	// Configura el ticker para cortar el pitido
	ticker_uvc_buzzer.ms_count = 0;
	ticker_uvc_buzzer.ms_max = ms;
	ticker_uvc_buzzer.state = TICKER_ACTIVE;
}

static void uvc_buzzer_off(void)
{
	// Se desactiva el timer
	TCCR2B &= ~(1 << CS22);

	// Configura el ticker
	ticker_uvc_buzzer.state = TICKER_NO_ACTIVE;
}

void adc_callback(adc_interrupt_request_t *adc_interrupt_request)
{
	// Si se lee el canal 0
	if (adc_interrupt_request->adc_channel == ADC_CHANNEL_0)
	{
		// Si se esta capturando la presion de un boton
		if (button_enable == BUTTON_ENABLED)
		{
			// Se comprueba el estado de los botones
			// Boton derecha
			if (adc_interrupt_request->adc_value < 50)
			{
				buttons[0] = BUTTON_PENDING;
			}

			// Boton arriba
			else if (adc_interrupt_request->adc_value < 195)
			{
				buttons[1] = BUTTON_PENDING;
			}
			
			// Boton abajo
			else if (adc_interrupt_request->adc_value < 380)
			{
				buttons[2] = BUTTON_PENDING;
			}

			// Boton izquierda
			else if (adc_interrupt_request->adc_value < 555)
			{
				buttons[3] = BUTTON_PENDING;
			}

			// Boton de seleccion
			else if (adc_interrupt_request->adc_value < 790)
			{
				buttons[4] = BUTTON_PENDING;
			}
		}
	}

	else if (adc_interrupt_request->adc_channel == ADC_CHANNEL_1)
	{
		uvc_door_open_sensor_value = adc_interrupt_request->adc_value;
	}
}

void wdt_callback(void)
{

}

ISR(TIMER2_COMPA_vect)
{
    UVC_BUZZER_PORT ^= (1 << UVC_BUZZER_PIN);

    TCNT2 = 0;
}
