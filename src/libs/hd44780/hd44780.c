#include "hd44780.h"

static inline void lcd_e_port_togle(void)
{
	_delay_us(LCD_CMD_DELAY);
	lcd_e_port_high();
	_delay_us(LCD_CMD_DELAY);
	lcd_e_port_low();
}

static void lcd_gpio_init(void)
{
	// Se setean los pines como salida
	lcd_e_ddr_high();
	lcd_rs_ddr_high();

	lcd_db4_ddr_high();
	lcd_db5_ddr_high();
	lcd_db6_ddr_high();
	lcd_db7_ddr_high();

	// Se inicia el pin enable en apagado
	lcd_e_port_low();

	// Retraso de inicio
	_delay_ms(LCD_RESET_DELAY);
}

#if (LCD_MODE == 0)
static void lcd_write(uint8_t data, uint8_t rs, uint8_t mode)
{
	lcd_rs_port_set(rs);

	// Si se van a escribir 8bits
	if (mode == LCD_WRITE_8BITS)
	{
		lcd_db4_port_set((data & (1 << 4)) >> 4);
		lcd_db5_port_set((data & (1 << 5)) >> 5);
		lcd_db6_port_set((data & (1 << 6)) >> 6);
		lcd_db7_port_set((data & (1 << 7)) >> 7);

		lcd_e_port_togle();
	}

	lcd_db4_port_set((data & (1 << 0)) >> 0);
	lcd_db5_port_set((data & (1 << 1)) >> 1);
	lcd_db6_port_set((data & (1 << 2)) >> 2);
	lcd_db7_port_set((data & (1 << 3)) >> 3);

	lcd_e_port_togle();

	// Tiempo de espera luego del comando
	if ((!rs) && ((data == LCD_CMD_CLS) || (data == LCD_CMD_HOME)))
	{
		_delay_us(1640);
	}
        
    else
	{
		_delay_us(40);
	}
}
#endif

void lcd_init(void)
{
	// Se configura el gpio para el LCD
	lcd_gpio_init();

	// Inicialización del LCD
	// Secuencia de arranque para modo de 4bits
	lcd_write(0x03, 0, LCD_WRITE_4BITS);
	_delay_us(4100);

	// Secuencia de arranque para modo de 4bits
	lcd_write(0x03, 0, LCD_WRITE_4BITS);
	_delay_us(100);

	// Secuencia de arranque para modo de 4bits
	lcd_write(0x03, 0, LCD_WRITE_4BITS);
	_delay_us(40);

	// Secuencia de arranque para modo de 4bits
	lcd_write(0x02, 0, LCD_WRITE_4BITS);
	_delay_us(40);

	// Se establece el modo de 4bits, dos lineas y 5x8 puntos
	lcd_cmd(LCD_CMD_FUNC_SET | LCD_CMD_LINES_2 | LCD_CMD_DOT_5x8);

	// Apagado
	lcd_cmd(LCD_CMD_OFF);

	// Se borra la pantalla
	lcd_cmd(LCD_CMD_CLS);

	// Modo de inicio
	lcd_cmd(LCD_CMD_ENTRY_MODE | LCD_CMD_CURSOR_INC);
}

void lcd_cmd(uint8_t cmd)
{
	// Escribe un comando el el LCD
	lcd_write(cmd, 0, LCD_WRITE_8BITS);
}

void lcd_on(void)
{
	// Se enciende el lcd
	lcd_cmd(LCD_CMD_ON);

	// Se pone el cursor en el inicio
	lcd_home();
}

void lcd_off(void)
{
	// Se apaga el lcd
	lcd_cmd(LCD_CMD_OFF);
}

void lcd_set_cursor(uint8_t cursor)
{
	// Cambia el modo del cursor
	lcd_cmd(cursor);
}

void lcd_gotoxy(uint8_t x, uint8_t y)
{	
	// Ubica el cursor en pantalla
	#if (LCD_ROWS == 2)
	if (y == 0)
	{
		lcd_cmd(LCD_DDRAM + x);
	}

	else if (y == 1)
	{
		lcd_cmd(LCD_DDRAM + x + 0x40);
	}

	#elif (LCD_ROWS == 4)
	if (y == 0)
	{
		lcd_cmd(LCD_DDRAM + x);
	}

	else if (y == 1)
	{
		lcd_cmd(LCD_DDRAM + x + 0x40);
	}

	else if (y == 2)
	{
		lcd_cmd(LCD_DDRAM + x + 0x14);
	}

	else if (y == 3)
	{
		lcd_cmd(LCD_DDRAM + x + 0x54);
	}
	#endif
}

void lcd_cls(void)
{
	// Se limpia la pantalla
	lcd_cmd(LCD_CMD_CLS);
}

void lcd_home(void)
{
	// Ubica el cursor en el home
	lcd_cmd(LCD_CMD_HOME);
}

void lcd_putc(char c)
{
	// Escribe un caracter
	lcd_write(c, 1, LCD_WRITE_8BITS);
}

void lcd_puts(const char *s)
{
	// Escribe caracter por caracter de una cadena de texto
	register char c;

	while ((c = *s++))
		lcd_putc(c);
}
