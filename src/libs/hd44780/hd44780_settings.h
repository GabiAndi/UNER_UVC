#ifndef HD44780_SETTINGS_H
#define HD44780_SETTINGS_H

// Tamaño del LCD
#define LCD_COLS					16				// Cantidad de columnas
#define LCD_ROWS					2				// Cantidad de filas

#define LCD_MODE					0				// 0 para el modo de 4 bits
													// 1 para el modo de 8 bits

// Pines de control
#define LCD_RS_PORT					PORTB
#define LCD_RS_PIN					0

#define LCD_E_PORT					PORTB
#define LCD_E_PIN					1

// Pines de los primeros 4 bits de datos
#if (LCD_MODE == 1)
#define LCD_DB0_PORT				PORTC
#define LCD_DB0_PIN					0

#define LCD_DB1_PORT				PORTC
#define LCD_DB1_PIN					1

#define LCD_DB2_PORT				PORTC
#define LCD_DB2_PIN					2

#define LCD_DB3_PORT				PORTC
#define LCD_DB3_PIN					3
#endif

// Pines de los ultimos 4 bits de datos
#define LCD_DB4_PORT				PORTD
#define LCD_DB4_PIN					4

#define LCD_DB5_PORT				PORTD
#define LCD_DB5_PIN					5

#define LCD_DB6_PORT				PORTD
#define LCD_DB6_PIN					6

#define LCD_DB7_PORT				PORTD
#define LCD_DB7_PIN					7

#endif
