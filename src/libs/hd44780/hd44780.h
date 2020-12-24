#ifndef HD44780_H
#define HD44780_H

#include <inttypes.h>

#include <avr/io.h>
#include <util/delay.h>

#include "hd44780_settings.h"

// Configuración para el LCD
#define LCD_RESET_DELAY					15		// Tiempo para reinciar el LCD en ms
#define LCD_CMD_DELAY                   1       // Tiempo de espera entre comando en us

// Comandos para distintos modos de operacion
#define LCD_WRITE_4BITS                 0       // Escribe 4bits en los pines del LCD
#define LCD_WRITE_8BITS                 1       // Escribe 8bits en los pines del LCD

#define LCD_DDRAM                       0x80    // Direccion de la DDRAM

// Comandos de inicialización para el LCD
#define LCD_CMD_OFF                     0x08    // Apaga el LCD
#define LCD_CMD_ON                      0x0C    // Enciende el LCD

#define LCD_CMD_FUNC_SET                0x20    // Comando para modo de funcionamiento

#define LCD_CMD_LINES_1                 0x00    // Una linea
#define LCD_CMD_LINES_2                 0x08    // Dos linea

#define LCD_CMD_DOT_5x8                 0x00    // 5x8 puntos
#define LCD_CMD_DOT_5x10                0x04    // 5x10 puntos

#define LCD_CMD_ENTRY_MODE              0x04    // Comando de configuracion del display

#define LCD_CMD_CURSOR_DEC              0x00    // Incrementa el cursor al escribir
#define LCD_CMD_CURSOR_INC              0x02    // Decrementa el cursor al escribir

// Comandos del LCD
#define LCD_CMD_CLS					    0x01    // Limpia el contenido del LCD
#define LCD_CMD_HOME					0x02    // Cursor al origen

#define LCD_CURSOR_HIDE                 0x0C    // Cursor oculto
#define LCD_CURSOR_SHOW                 0x0E    // Cursor activo
#define LCD_CURSOR_BLINK                0x0F    // Cursor activo y parpadenado

// Se comprueba la configuración del LCD
#if ((LCD_MODE != 0) && (LCD_MODE != 1))
#error "El modo de operacion del LCD no es valida."
#endif

// Definiciones de puertos
#define DDR(x) (*(&x - 1))   // Esta operación devuelve el registro de DDR en base al PORT
#define PIN(x) (*(&x - 2))   // Esta operación devuelve el registro de PIN en base al PORT

// Operaciones de puertos
// Para estados bajos
#define lcd_rs_port_low() (LCD_RS_PORT &= ~(1 << LCD_RS_PIN))
#define lcd_e_port_low() (LCD_E_PORT &= ~(1 << LCD_E_PIN))

#define lcd_db0_port_low() (LCD_DB0_PORT &= ~(1 << LCD_DB0_PIN))
#define lcd_db1_port_low() (LCD_DB1_PORT &= ~(1 << LCD_DB1_PIN))
#define lcd_db2_port_low() (LCD_DB2_PORT &= ~(1 << LCD_DB2_PIN))
#define lcd_db3_port_low() (LCD_DB3_PORT &= ~(1 << LCD_DB3_PIN))
#define lcd_db4_port_low() (LCD_DB4_PORT &= ~(1 << LCD_DB4_PIN))
#define lcd_db5_port_low() (LCD_DB5_PORT &= ~(1 << LCD_DB5_PIN))
#define lcd_db6_port_low() (LCD_DB6_PORT &= ~(1 << LCD_DB6_PIN))
#define lcd_db7_port_low() (LCD_DB7_PORT &= ~(1 << LCD_DB7_PIN))

// Para estados altos
#define lcd_rs_port_high() (LCD_RS_PORT |= (1 << LCD_RS_PIN))
#define lcd_e_port_high() (LCD_E_PORT |= (1 << LCD_E_PIN))

#define lcd_db0_port_high() (LCD_DB0_PORT |= (1 << LCD_DB0_PIN))
#define lcd_db1_port_high() (LCD_DB1_PORT |= (1 << LCD_DB1_PIN))
#define lcd_db2_port_high() (LCD_DB2_PORT |= (1 << LCD_DB2_PIN))
#define lcd_db3_port_high() (LCD_DB3_PORT |= (1 << LCD_DB3_PIN))
#define lcd_db4_port_high() (LCD_DB4_PORT |= (1 << LCD_DB4_PIN))
#define lcd_db5_port_high() (LCD_DB5_PORT |= (1 << LCD_DB5_PIN))
#define lcd_db6_port_high() (LCD_DB6_PORT |= (1 << LCD_DB6_PIN))
#define lcd_db7_port_high() (LCD_DB7_PORT |= (1 << LCD_DB7_PIN))

// Para setear estados
#define lcd_rs_port_set(value) (LCD_RS_PORT = ((LCD_RS_PORT & ~(1 << LCD_RS_PIN)) | (value << LCD_RS_PIN)))
#define lcd_e_port_set(value) (LCD_E_PORT = ((LCD_E_PORT & ~(1 << LCD_E_PIN)) | (value << LCD_E_PIN)))

#define lcd_db0_port_set(value) (LCD_DB0_PORT = ((LCD_DB0_PORT & ~(1 << LCD_DB0_PIN)) | (value << LCD_DB0_PIN)))
#define lcd_db1_port_set(value) (LCD_DB1_PORT = ((LCD_DB1_PORT & ~(1 << LCD_DB1_PIN)) | (value << LCD_DB1_PIN)))
#define lcd_db2_port_set(value) (LCD_DB2_PORT = ((LCD_DB2_PORT & ~(1 << LCD_DB2_PIN)) | (value << LCD_DB2_PIN)))
#define lcd_db3_port_set(value) (LCD_DB3_PORT = ((LCD_DB3_PORT & ~(1 << LCD_DB3_PIN)) | (value << LCD_DB3_PIN)))
#define lcd_db4_port_set(value) (LCD_DB4_PORT = ((LCD_DB4_PORT & ~(1 << LCD_DB4_PIN)) | (value << LCD_DB4_PIN)))
#define lcd_db5_port_set(value) (LCD_DB5_PORT = ((LCD_DB5_PORT & ~(1 << LCD_DB5_PIN)) | (value << LCD_DB5_PIN)))
#define lcd_db6_port_set(value) (LCD_DB6_PORT = ((LCD_DB6_PORT & ~(1 << LCD_DB6_PIN)) | (value << LCD_DB6_PIN)))
#define lcd_db7_port_set(value) (LCD_DB7_PORT = ((LCD_DB7_PORT & ~(1 << LCD_DB7_PIN)) | (value << LCD_DB7_PIN)))

// Operaciones de ddr
// Para estados bajos
#define lcd_rs_ddr_low() (DDR(LCD_RS_PORT) &= ~(1 << LCD_RS_PIN))
#define lcd_e_ddr_low() (DDR(LCD_E_PORT) &= ~(1 << LCD_E_PIN))

#define lcd_db0_ddr_low() (DDR(LCD_DB0_PORT) &= ~(1 << LCD_DB0_PIN))
#define lcd_db1_ddr_low() (DDR(LCD_DB1_PORT) &= ~(1 << LCD_DB1_PIN))
#define lcd_db2_ddr_low() (DDR(LCD_DB2_PORT) &= ~(1 << LCD_DB2_PIN))
#define lcd_db3_ddr_low() (DDR(LCD_DB3_PORT) &= ~(1 << LCD_DB3_PIN))
#define lcd_db4_ddr_low() (DDR(LCD_DB4_PORT) &= ~(1 << LCD_DB4_PIN))
#define lcd_db5_ddr_low() (DDR(LCD_DB5_PORT) &= ~(1 << LCD_DB5_PIN))
#define lcd_db6_ddr_low() (DDR(LCD_DB6_PORT) &= ~(1 << LCD_DB6_PIN))
#define lcd_db7_ddr_low() (DDR(LCD_DB7_PORT) &= ~(1 << LCD_DB7_PIN))

// Para estados altos
#define lcd_rs_ddr_high() DDR(LCD_RS_PORT) |= (1 << LCD_RS_PIN)
#define lcd_e_ddr_high() (DDR(LCD_E_PORT) |= (1 << LCD_E_PIN))

#define lcd_db0_ddr_high() (DDR(LCD_DB0_PORT) |= (1 << LCD_DB0_PIN))
#define lcd_db1_ddr_high() (DDR(LCD_DB1_PORT) |= (1 << LCD_DB1_PIN))
#define lcd_db2_ddr_high() (DDR(LCD_DB2_PORT) |= (1 << LCD_DB2_PIN))
#define lcd_db3_ddr_high() (DDR(LCD_DB3_PORT) |= (1 << LCD_DB3_PIN))
#define lcd_db4_ddr_high() (DDR(LCD_DB4_PORT) |= (1 << LCD_DB4_PIN))
#define lcd_db5_ddr_high() (DDR(LCD_DB5_PORT) |= (1 << LCD_DB5_PIN))
#define lcd_db6_ddr_high() (DDR(LCD_DB6_PORT) |= (1 << LCD_DB6_PIN))
#define lcd_db7_ddr_high() (DDR(LCD_DB7_PORT) |= (1 << LCD_DB7_PIN))

// Para setear estados
#define lcd_rs_ddr_set(value) (LCD_RS_PORT = ((LCD_RS_PORT & ~(1 << LCD_RS_PIN)) | (value << LCD_RS_PIN)))
#define lcd_e_ddr_set(value) (LCD_E_PORT = ((LCD_E_PORT & ~(1 << LCD_E_PIN)) | (value << LCD_E_PIN)))

#define lcd_db0_ddr_set(value) (DDR(LCD_DB0_PORT) = ((DDR(LCD_DB0_PORT) & ~(1 << LCD_DB0_PIN)) | (value << LCD_DB0_PIN)))
#define lcd_db1_ddr_set(value) (DDR(LCD_DB1_PORT) = ((DDR(LCD_DB1_PORT) & ~(1 << LCD_DB1_PIN)) | (value << LCD_DB1_PIN)))
#define lcd_db2_ddr_set(value) (DDR(LCD_DB2_PORT) = ((DDR(LCD_DB2_PORT) & ~(1 << LCD_DB2_PIN)) | (value << LCD_DB2_PIN)))
#define lcd_db3_ddr_set(value) (DDR(LCD_DB3_PORT) = ((DDR(LCD_DB3_PORT) & ~(1 << LCD_DB3_PIN)) | (value << LCD_DB3_PIN)))
#define lcd_db4_ddr_set(value) (DDR(LCD_DB4_PORT) = ((DDR(LCD_DB4_PORT) & ~(1 << LCD_DB4_PIN)) | (value << LCD_DB4_PIN)))
#define lcd_db5_ddr_set(value) (DDR(LCD_DB5_PORT) = ((DDR(LCD_DB5_PORT) & ~(1 << LCD_DB5_PIN)) | (value << LCD_DB5_PIN)))
#define lcd_db6_ddr_set(value) (DDR(LCD_DB6_PORT) = ((DDR(LCD_DB6_PORT) & ~(1 << LCD_DB6_PIN)) | (value << LCD_DB6_PIN)))
#define lcd_db7_ddr_set(value) (DDR(LCD_DB7_PORT) = ((DDR(LCD_DB7_PORT) & ~(1 << LCD_DB7_PIN)) | (value << LCD_DB7_PIN)))

void lcd_init();  // Inicia el LCD con el modo de cursor enviado

void lcd_on(void);  // Enciende el LCD
void lcd_off(void); // Apaga el LCD

void lcd_set_cursor(uint8_t cursor);    // Setea el cursor

void lcd_cmd(uint8_t cmd);  // Envia un comando al LCD

void lcd_cls(void); // Borra la pantalla del LCD
void lcd_home(void);    // Vuelve el puntero al inicio
void lcd_gotoxy(uint8_t x, uint8_t y);  // Situa el puntero en la posición

void lcd_putc(char c);  // Escribe un caracter en pantalla
void lcd_puts(const char *s);   // Escribe un string en pantalla

#endif
