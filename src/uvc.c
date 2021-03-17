#include "uvc.h"

// Tickers
static ticker_t ticker_uvc_led_blink;	        			// Ticker para el LED
static ticker_t ticker_uvc_ui_exec;	            			// Ticker del UI
static ticker_t ticker_uvc_ui_msj;							// Ticker para mostrar mensajes emergentes
static ticker_t ticker_uvc_timer;	            			// Ticker del timer del UVC
static ticker_t ticker_uvc_buzzer;							// Ticker para el buzzer
static ticker_t ticker_uvc_door;							// Ticker para mantener la puerta abierta
static ticker_t ticker_uvc_encoder_select;					// Ticker para el antibounce del boton del encoder
static ticker_t ticker_uvc_encoder_rot;						// Ticker para el antibounce del giro del encoder

// Control de LCD
static char uvc_lcd_buffer[4][21];							// Buffer para el LCD

// Interfaz de usuario
static uint8_t uvc_ui_menu_view = UVC_UI_MENU_HOME;			// Indica el menu que se esta mostrando en pantalla
static uint8_t uvc_ui_menu_home_index = 0;					// Indice de entrada de menu
static uint8_t uvc_ui_menu_index = 0;						// Indice de selección del menu
static uint8_t uvc_ui_menu_prog_index = 0;					// Indice de selección de los programas
static uint8_t uvc_ui_msj_stack[UVC_UI_MSJ_STACK_SIZE];		// Stack de mensajes
static uint8_t uvc_ui_msj_stack_index = 0;					// Posición del stack

static uvc_ui_msj_t uvc_ui_msjs[] = 
{
	{
		"  Bienvenido (UVC)  ",
		"   UNER FCAL 2020   ",
		UVC_UI_MSJ_TYPE_INFO,
		UVC_UI_MSJ_WELLCOME_TIME
	},
	
	{
		"   Puerta abierta   ",
		" Cierrela por favor ",
		UVC_UI_MSJ_TYPE_ERROR,
		UVC_UI_MSJ_LONG_TIME
	},

	{
		"     Finalizado     ",
		"Operacion completada",
		UVC_UI_MSJ_TYPE_INFO,
		UVC_UI_MSJ_SHORT_TIME
	}
};														// Mensajes que pueden surgir

static uint8_t uvc_ui_change_time = 0;					// Cambiar los segundos del programa
static uint8_t uvc_ui_change_prog = 0;					// Cambiar los programas

// Control de la maquina
static uint8_t uvc_state = UVC_STATE_OFF;				// Estado de la maquina
static uint8_t uvc_time = 0;							// Segundos del temporizador

// Programas de tiempo
static uvc_program_t uvc_programs[] = 
{
	{
		"     SARS-CoV-2     ",
		40
	},

	{
		"   S. cerevisiae    ",
		60
	},

	{
		"      S. aureus     ",
		45
	},

	{
		"       E. coli      ",
		45
	}
};													// Tiempos programados

static uint8_t uvc_encoder_enable = UVC_ENCODER_ENABLE;						// Encoder habilitado
static uint8_t uvc_encoder_select_enable = UVC_ENCODER_ENABLE;				// Boton del encoder habilitado
static uint8_t uvc_encoder_rot_enable = UVC_ENCODER_ENABLE;					// Giro del encoder habilitado

static void uvc_gpio_init(void);					// Funcion de inicializacion del GPIO
static void uvc_led_blink(void);					// Función de parpadeo para el LED de estado
static void uvc_ui_exec(void);						// Funcion que se encarga del UI
static void uvc_ui_msj(void);						// Funcion para mostrar mensajes emergentes
static void uvc_ui_msj_show(uint8_t msj_id);		// Función que agrega un mensaje en el stack
static void uvc_timer(void);						// Funcion para decrementar el contador del timer del UVC
static void uvc_manager(void);						// Funcion que controla el comportamiento de la maquina
static void uvc_buzzer_start(uint16_t ms);			// Funcion que inicia el pitido del buzzer
static void uvc_buzzer_stop(void);					// Funcion que corta el pitido del buzzer
static void uvc_door_opening(uint8_t sec);			// FUncion que abre la puerta por un periodo de tiempo
static void uvc_door_closing(void);					// Funcion que cierra la puerta
static void uvc_encoder_select(void);				// Funcion que controla el bounce del encoder
static void uvc_encoder_rotate(void);				// Funcion que controla el giro del encoder

void uvc_init(void)
{
    // Se inicializa el GPIO del microcontrolador
	uvc_gpio_init();

	// Se inicia el LCD
	lcd_init();
	lcd_on();

	// Se inicia el modulo de tickers
	ticker_init();

	// Se configura el encoder
	encoder_init();
	
	// Se configura el WATCHDOG
	wdt_init(WDT_MAX_TIME_125MS, WDT_MODE_IT_RESET);

	// Se configura el ticker del LED
	ticker_uvc_led_blink.calls = 0;
	ticker_uvc_led_blink.ms_count = 0;
	ticker_uvc_led_blink.ms_max = UVC_LED_PERIOD;
	ticker_uvc_led_blink.priority = TICKER_LOW_PRIORITY;
	ticker_uvc_led_blink.state = TICKER_ACTIVE;
	ticker_uvc_led_blink.ticker_function = uvc_led_blink;

	ticker_new(&ticker_uvc_led_blink);

	// Se configura el ticker del UI
	ticker_uvc_ui_exec.calls = 0;
	ticker_uvc_ui_exec.ms_count = 0;
	ticker_uvc_ui_exec.ms_max = 1000 / UVC_UI_FPS;
	ticker_uvc_ui_exec.priority = TICKER_LOW_PRIORITY;
	ticker_uvc_ui_exec.state = TICKER_ACTIVE;
	ticker_uvc_ui_exec.ticker_function = uvc_ui_exec;

	ticker_new(&ticker_uvc_ui_exec);

	// Se configura el ticker para los mensajes
	ticker_uvc_ui_msj.calls = 0;
	ticker_uvc_ui_msj.ms_count = 0;
	ticker_uvc_ui_msj.ms_max = UVC_UI_MSJ_LONG_TIME;
	ticker_uvc_ui_msj.priority = TICKER_LOW_PRIORITY;
	ticker_uvc_ui_msj.state = TICKER_NO_ACTIVE;
	ticker_uvc_ui_msj.ticker_function = uvc_ui_msj;

	ticker_new(&ticker_uvc_ui_msj);

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
	ticker_uvc_buzzer.ticker_function = uvc_buzzer_stop;

	ticker_new(&ticker_uvc_buzzer);

	// Se configura el ticker del actuador de la puerta
	ticker_uvc_door.calls = 0;
	ticker_uvc_door.ms_count = 0;
	ticker_uvc_door.ms_max = UVC_DOOR_OPEN_SEC * 1000;
	ticker_uvc_door.priority = TICKER_LOW_PRIORITY;
	ticker_uvc_door.state = TICKER_NO_ACTIVE;
	ticker_uvc_door.ticker_function = uvc_door_closing;

	ticker_new(&ticker_uvc_door);

	// Se configura el ticker del boton de seleccion del encoder
	ticker_uvc_encoder_select.calls = 0;
	ticker_uvc_encoder_select.ms_count = 0;
	ticker_uvc_encoder_select.ms_max = 500;
	ticker_uvc_encoder_select.priority = TICKER_LOW_PRIORITY;
	ticker_uvc_encoder_select.state = TICKER_NO_ACTIVE;
	ticker_uvc_encoder_select.ticker_function = uvc_encoder_select;

	ticker_new(&ticker_uvc_encoder_select);

	// Se configura el ticker del giro del encoder
	ticker_uvc_encoder_rot.calls = 0;
	ticker_uvc_encoder_rot.ms_count = 0;
	ticker_uvc_encoder_rot.ms_max = 180;
	ticker_uvc_encoder_rot.priority = TICKER_LOW_PRIORITY;
	ticker_uvc_encoder_rot.state = TICKER_NO_ACTIVE;
	ticker_uvc_encoder_rot.ticker_function = uvc_encoder_rotate;

	ticker_new(&ticker_uvc_encoder_rot);

	// Mostrar mensaje de bienvenida
	uvc_ui_msj_show(UVC_UI_MSJ_WELLCOME);

	uvc_buzzer_start(UVC_BUZZER_SELECT_TIME);
}

void uvc_exec(void)
{
	// Eventos y toma de deciciones
	uvc_manager();

	// Captura de los eventos del encoder
	encoder_exec();

    // Se ejecutan los tickers
    ticker_exec();

    // Se resetea al WATCHDOG
    wdt_reset();
}

static void uvc_gpio_init(void)
{
	// Configuración del LED
	DDR(UVC_LED_PORT) |= (1 << UVC_LED_PIN);

	UVC_LED_PORT &= ~(1 << UVC_LED_PIN);

	// Configuracion del buzzer
	DDR(UVC_BUZZER_PORT) |= (1 << UVC_BUZZER_PIN);

	UVC_BUZZER_PORT &= ~(1 << UVC_BUZZER_PIN);

	// Configuracion del sensor de puerta abierta
	DDR(UVC_DOOR_OPEN_PORT) &= ~(1 << UVC_DOOR_OPEN_PIN);

	UVC_DOOR_OPEN_PORT |= (1 << UVC_DOOR_OPEN_PIN);

	// Configuracion de las lamparas
	DDR(UVC_LAMP_PORT) |= (1 << UVC_LAMP_PIN);

	UVC_LAMP_PORT |= (1 << UVC_LAMP_PIN);

	// Configuracion del cierre magnetico
	DDR(UVC_DOOR_LOCK_PORT) |= (1 << UVC_DOOR_LOCK_PIN);

	UVC_DOOR_LOCK_PORT |= (1 << UVC_DOOR_LOCK_PIN);
}

static void uvc_led_blink(void)
{
	// Cambio de estado del led
	uvc_led_toggle();
}

static void uvc_ui_exec(void)
{
	// Formateo del menu a mostrar si no hay mensajes pendientes
	if (uvc_ui_msj_stack_index == 0)
	{
		switch (uvc_ui_menu_view)
		{
			case UVC_UI_MENU_HOME:
				sprintf(uvc_lcd_buffer[0], "Inicio rapido       ");
				sprintf(uvc_lcd_buffer[1], "Iniciar programa    ");
				sprintf(uvc_lcd_buffer[2], "Abir puerta         ");
				sprintf(uvc_lcd_buffer[3], "Configuracion       ");

				switch (uvc_ui_menu_index)
				{
					case 0:
					case 1:
					case 2:
					case 3:
						uvc_lcd_buffer[uvc_ui_menu_index][19] = '<';

						break;

					case 255:
						uvc_ui_menu_index = 3;

						break;
					
					default:
						uvc_ui_menu_index = 0;

						break;
				}

				break;

			case UVC_UI_MENU_FAST_INIT:
				sprintf(uvc_lcd_buffer[0], "    Inicio rapido   ");
				
				if (uvc_time < 10)
				{
					sprintf(uvc_lcd_buffer[1], "Tiempo exp: %u seg   ", uvc_time);
				}

				else if ((uvc_time >= 10) && (uvc_time < 100))
				{
					sprintf(uvc_lcd_buffer[1], "Tiempo exp: %u seg  ", uvc_time);
				}
				
				else
				{
					sprintf(uvc_lcd_buffer[1], "Tiempo exp: %u seg ", uvc_time);
				}
				

				sprintf(uvc_lcd_buffer[2], "     Abir puerta    ");
				
				if (uvc_state == UVC_STATE_OFF)
				{
					sprintf(uvc_lcd_buffer[3], " Volver     Iniciar ");
				}

				else if (uvc_state == UVC_STATE_ON)
				{
					sprintf(uvc_lcd_buffer[3], " Volver     Detener ");
				}

				switch (uvc_ui_menu_index)
				{
					case 0:
						uvc_lcd_buffer[1][19] = '<';
						
						break;

					case 1:
						uvc_lcd_buffer[2][0] = '>';
						uvc_lcd_buffer[2][19] = '<';
						
						break;

					case 2:
						uvc_lcd_buffer[3][0] = '>';
						uvc_lcd_buffer[3][7] = '<';
						
						break;

					case 3:
						uvc_lcd_buffer[3][11] = '>';
						uvc_lcd_buffer[3][19] = '<';
						
						break;

					case 255:
						uvc_ui_menu_index = 3;

						break;

					default:
						uvc_ui_menu_index = 0;
						
						break;
				}

				break;

			case UVC_UI_MENU_INIT_PROG:
				if (uvc_ui_menu_prog_index < UVC_PROGRAMS_N)
				{
					sprintf(uvc_lcd_buffer[0], "%s", uvc_programs[uvc_ui_menu_prog_index].name);
				}

				else if (uvc_ui_menu_prog_index == 255)
				{
					uvc_ui_menu_prog_index = (UVC_PROGRAMS_N - 1);
				}
				
				else
				{
					uvc_ui_menu_prog_index = 0;
				}
				
				if (uvc_ui_change_prog)
				{
					uvc_time = uvc_programs[uvc_ui_menu_prog_index].time;
				}
				
				if (uvc_time < 10)
				{
					sprintf(uvc_lcd_buffer[1], "Tiempo exp: %u seg   ", uvc_time);
				}

				else if ((uvc_time >= 10) && (uvc_time < 100))
				{
					sprintf(uvc_lcd_buffer[1], "Tiempo exp: %u seg  ", uvc_time);
				}
				
				else
				{
					sprintf(uvc_lcd_buffer[1], "Tiempo exp: %u seg ", uvc_time);
				}
				

				sprintf(uvc_lcd_buffer[2], "     Abir puerta    ");
				
				if (uvc_state == UVC_STATE_OFF)
				{
					sprintf(uvc_lcd_buffer[3], " Volver     Iniciar ");
				}

				else if (uvc_state == UVC_STATE_ON)
				{
					sprintf(uvc_lcd_buffer[3], " Volver     Detener ");
				}

				switch (uvc_ui_menu_index)
				{
					case 0:
						uvc_lcd_buffer[0][0] = '>';
						uvc_lcd_buffer[0][19] = '<';
						
						break;

					case 1:
						uvc_lcd_buffer[1][19] = '<';
						
						break;

					case 2:
						uvc_lcd_buffer[2][0] = '>';
						uvc_lcd_buffer[2][19] = '<';
						
						break;

					case 3:
						uvc_lcd_buffer[3][0] = '>';
						uvc_lcd_buffer[3][7] = '<';
						
						break;

					case 4:
						uvc_lcd_buffer[3][11] = '>';
						uvc_lcd_buffer[3][19] = '<';
						
						break;

					case 255:
						uvc_ui_menu_index = 4;

						break;

					default:
						uvc_ui_menu_index = 0;
						
						break;
				}

				break;

			/*case UVC_UI_MODE_PROG_SELEC:
				sprintf(uvc_lcd_buffer[0], "%s", uvc_programs[uvc_ui_program_index].name);
				sprintf(uvc_lcd_buffer[1], "%s", uvc_programs[uvc_ui_program_index].desc);

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

				break;*/
		}
	}

	else
	{
		switch (uvc_ui_msjs[uvc_ui_msj_stack[uvc_ui_msj_stack_index]].type)
		{
			case UVC_UI_MSJ_TYPE_ERROR:
				sprintf(uvc_lcd_buffer[0], "------- ERROR ------");

				break;

			case UVC_UI_MSJ_TYPE_WARNG:
				sprintf(uvc_lcd_buffer[0], "------ WARNING -----");

				break;

			case UVC_UI_MSJ_TYPE_INFO:
				sprintf(uvc_lcd_buffer[0], "------- INFO -------");

				break;
		}

		sprintf(uvc_lcd_buffer[1], "%s", uvc_ui_msjs[uvc_ui_msj_stack[uvc_ui_msj_stack_index]].msj);
		sprintf(uvc_lcd_buffer[2], "%s", uvc_ui_msjs[uvc_ui_msj_stack[uvc_ui_msj_stack_index]].desc);

		sprintf(uvc_lcd_buffer[3], "-------> OK <-------");
	}
	

	// Se escribe la linea 1 del buffer
	lcd_gotoxy(0, 0);
	lcd_puts(uvc_lcd_buffer[0]);

	// Se escribe la linea 2 del buffer
	lcd_gotoxy(0, 1);
	lcd_puts(uvc_lcd_buffer[1]);

	// Se escribe la linea 3 del buffer
	lcd_gotoxy(0, 2);
	lcd_puts(uvc_lcd_buffer[2]);

	// Se escribe la linea 4 del buffer
	lcd_gotoxy(0, 3);
	lcd_puts(uvc_lcd_buffer[3]);
}

static void uvc_ui_msj(void)
{
	uvc_ui_msj_stack_index--;

	if (uvc_ui_msj_stack_index == 0)
	{
		ticker_uvc_ui_msj.state = TICKER_NO_ACTIVE;
	}

	else
	{
		ticker_uvc_ui_msj.ms_max = uvc_ui_msjs[uvc_ui_msj_stack[uvc_ui_msj_stack_index]].time;
	}
}

static void uvc_ui_msj_show(uint8_t msj_id)
{
	uvc_ui_msj_stack_index++;
	uvc_ui_msj_stack[uvc_ui_msj_stack_index] = msj_id;

	ticker_uvc_ui_msj.ms_count = 0;
	ticker_uvc_ui_msj.ms_max = uvc_ui_msjs[msj_id].time;
	ticker_uvc_ui_msj.state = TICKER_ACTIVE;
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
			if (uvc_door_is_open())
			{
				uvc_state = UVC_STATE_TURNING_OFF;

				uvc_ui_msj_show(UVC_UI_MSJ_OPEN_DOOR);

				uvc_buzzer_start(UVC_BUZZER_ERROR_TIME);
			}

			uvc_lamp_on();

			break;

		case UVC_STATE_TURNING_OFF:
			ticker_uvc_timer.state = TICKER_NO_ACTIVE;

			uvc_state = UVC_STATE_OFF;

			// Si el programa se termino
			if (uvc_time == 0)
			{
				uvc_ui_msj_show(UVC_UI_MSJ_PROG_FINISH);

				uvc_buzzer_start(UVC_BUZZER_ON_OFF_TIME);
			}

			break;

		case UVC_STATE_OFF:
			uvc_lamp_off();

			break;
	}
}

static void uvc_buzzer_start(uint16_t ms)
{
	// Se activa el buzzer
	uvc_buzzer_on();

	// Configura el ticker para cortar el pitido
	ticker_uvc_buzzer.ms_count = 0;
	ticker_uvc_buzzer.ms_max = ms;
	ticker_uvc_buzzer.state = TICKER_ACTIVE;
}

static void uvc_buzzer_stop(void)
{
	// Se desactiva el buzzer
	uvc_buzzer_off();

	// Configura el ticker
	ticker_uvc_buzzer.state = TICKER_NO_ACTIVE;
}

static void uvc_door_opening(uint8_t sec)
{
	ticker_uvc_door.ms_count = 0;
	ticker_uvc_door.ms_max = sec * 1000;
	ticker_uvc_door.state = TICKER_ACTIVE;

	uvc_door_open();
}

static void uvc_door_closing(void)
{
	ticker_uvc_door.state = TICKER_NO_ACTIVE;

	uvc_door_close();
}

static void uvc_encoder_select(void)
{
	ticker_uvc_encoder_select.state = TICKER_NO_ACTIVE;

	uvc_encoder_select_enable = UVC_ENCODER_ENABLE;
}

static void uvc_encoder_rotate(void)
{
	ticker_uvc_encoder_rot.state = TICKER_NO_ACTIVE;

	uvc_encoder_rot_enable = UVC_ENCODER_ENABLE;
}

void wdt_callback(void)
{
	
}

void encoder_select_callback(void)
{
	if ((uvc_encoder_enable == UVC_ENCODER_ENABLE) && (uvc_encoder_select_enable == UVC_ENCODER_ENABLE))
	{
		uvc_encoder_select_enable = UVC_ENCODER_DISABLE;

		ticker_uvc_encoder_select.ms_count = 0;
		ticker_uvc_encoder_select.state = TICKER_ACTIVE;

		uvc_buzzer_start(UVC_BUZZER_SELECT_TIME);

		if (uvc_ui_msj_stack_index != 0)
		{
			uvc_ui_msj();
		}

		else if (uvc_ui_menu_view == UVC_UI_MENU_HOME)
		{
			switch (uvc_ui_menu_index)
			{
				case 0:
					uvc_ui_menu_view = UVC_UI_MENU_FAST_INIT;

					uvc_ui_menu_home_index = uvc_ui_menu_index;

					uvc_ui_menu_index = 0;
					
					break;

				case 1:
					uvc_ui_menu_view = UVC_UI_MENU_INIT_PROG;

					uvc_time = uvc_programs[uvc_ui_menu_prog_index].time;

					uvc_ui_menu_home_index = uvc_ui_menu_index;

					uvc_ui_menu_index = 0;
					
					break;

				case 2:
					uvc_door_opening(UVC_DOOR_OPEN_SEC);
					
					break;
			}
		}
		
		else if (uvc_ui_menu_view == UVC_UI_MENU_FAST_INIT)
		{
			switch (uvc_ui_menu_index)
			{
				case 0:
					if (uvc_state == UVC_STATE_OFF)
					{
						uvc_ui_change_time = !uvc_ui_change_time;
					}
					
					break;

				case 1:
					if (uvc_state == UVC_STATE_OFF)
					{
						uvc_door_opening(UVC_DOOR_OPEN_SEC);
					}
					
					break;

				case 2:
					if (uvc_state == UVC_STATE_OFF)
					{
						uvc_ui_menu_view = UVC_UI_MENU_HOME;

						uvc_ui_menu_index = uvc_ui_menu_home_index;
					}
					
					break;

				case 3:
					if (uvc_state == UVC_STATE_ON)
					{
						uvc_state = UVC_STATE_TURNING_OFF;
					}

					else if ((uvc_state == UVC_STATE_OFF) && (uvc_time > 0))
					{
						uvc_door_closing();

						uvc_state = UVC_STATE_TURNING_ON;
					}
					
					break;
			}
		}

		else if (uvc_ui_menu_view == UVC_UI_MENU_INIT_PROG)
		{
			if (uvc_ui_change_prog)
			{
				uvc_time = uvc_programs[uvc_ui_menu_prog_index].time;
			}

			switch (uvc_ui_menu_index)
			{
				case 0:
					if (uvc_state == UVC_STATE_OFF)
					{
						uvc_ui_change_prog = !uvc_ui_change_prog;
					}

					break;

				case 1:
					if (uvc_state == UVC_STATE_OFF)
					{
						uvc_ui_change_time = !uvc_ui_change_time;
					}
	
					break;

				case 2:
					if (uvc_state == UVC_STATE_OFF)
					{
						uvc_door_opening(UVC_DOOR_OPEN_SEC);
					}
					
					break;

				case 3:
					if (uvc_state == UVC_STATE_OFF)
					{
						uvc_ui_menu_view = UVC_UI_MENU_HOME;

						uvc_ui_menu_index = uvc_ui_menu_home_index;
					}
					
					break;

				case 4:
					if (uvc_state == UVC_STATE_ON)
					{
						uvc_state = UVC_STATE_TURNING_OFF;
					}

					else if ((uvc_state == UVC_STATE_OFF) && (uvc_time > 0))
					{
						uvc_door_closing();

						uvc_state = UVC_STATE_TURNING_ON;
					}
					
					break;
			}
		}
	}
}

void encoder_clockwise_callback(void)
{
	if ((uvc_encoder_enable == UVC_ENCODER_ENABLE) && (uvc_encoder_rot_enable == UVC_ENCODER_ENABLE) && (uvc_ui_msj_stack_index == 0))
	{
		if (((uvc_ui_menu_view == UVC_UI_MENU_FAST_INIT) || (uvc_ui_menu_view == UVC_UI_MENU_INIT_PROG)) && (uvc_ui_change_time))
		{
			uvc_time++;
		}

		else if (((uvc_ui_menu_view == UVC_UI_MENU_FAST_INIT) || (uvc_ui_menu_view == UVC_UI_MENU_INIT_PROG)) && (uvc_ui_change_prog))
		{
			uvc_ui_menu_prog_index++;
		}

		else
		{
			uvc_ui_menu_index++;
		}
		

		uvc_encoder_rot_enable = UVC_ENCODER_DISABLE;

		ticker_uvc_encoder_rot.ms_count = 0;
		ticker_uvc_encoder_rot.state = TICKER_ACTIVE;

		uvc_buzzer_start(UVC_BUZZER_ROT_TIME);
	}
}

void encoder_counter_clockwise_callback(void)
{
	if ((uvc_encoder_enable == UVC_ENCODER_ENABLE) && (uvc_encoder_rot_enable == UVC_ENCODER_ENABLE) && (uvc_ui_msj_stack_index == 0))
	{
		if (((uvc_ui_menu_view == UVC_UI_MENU_FAST_INIT) || (uvc_ui_menu_view == UVC_UI_MENU_INIT_PROG)) && (uvc_ui_change_time))
		{
			uvc_time--;
		}

		else if (((uvc_ui_menu_view == UVC_UI_MENU_FAST_INIT) || (uvc_ui_menu_view == UVC_UI_MENU_INIT_PROG)) && (uvc_ui_change_prog))
		{
			uvc_ui_menu_prog_index--;
		}

		else
		{
			uvc_ui_menu_index--;
		}
		

		uvc_encoder_rot_enable = UVC_ENCODER_DISABLE;

		ticker_uvc_encoder_rot.ms_count = 0;
		ticker_uvc_encoder_rot.state = TICKER_ACTIVE;

		uvc_buzzer_start(UVC_BUZZER_ROT_TIME);
	}
}
