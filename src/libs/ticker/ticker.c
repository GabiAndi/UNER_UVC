#include "ticker.h"

// Vector que almacena los tickers
static ticker_t *tickers[TICKER_MAX_USE];

void ticker_init(void)
{
	// Configuracion del timer 0 base
	cli();

	TCCR0A = 0b00000000;	// Modo de operacion normal
	TCCR0B = 0b00000011;	// Prescaler de 64
	
	TCNT0 = 0;

	TIMSK0 = 0b00000001;	// Interrupción por desbordamiento

	sei();

	for (uint8_t i = 0 ; i < TICKER_MAX_USE ; i++)
	{
		tickers[i] = 0;
	}
}

uint8_t ticker_new(ticker_t *ticker)
{
	for (uint8_t i = 0 ; i < TICKER_MAX_USE ; i++)
	{
		if (tickers[i] == ticker)
		{
			return 0;
		}
	}

	for (uint8_t i = 0 ; i < TICKER_MAX_USE ; i++)
	{
		if (tickers[i] == 0)
		{
			tickers[i] = ticker;

			return 1;
		}
	}

    return 0;
}

uint8_t ticker_delete(ticker_t *ticker)
{
	for (uint8_t i = 0 ; i < TICKER_MAX_USE ; i++)
	{
		if (tickers[i] == ticker)
		{
			tickers[i] = 0;

			return 1;
		}
	}

    return 0;
}

uint8_t ticker_exec(void)
{
	uint8_t n_exec = 0;

	for (uint8_t i = 0 ; i < TICKER_MAX_USE ; i++)
	{
		if (tickers[i] != 0)
		{
			if ((tickers[i]->ms_count >= tickers[i]->ms_max) && (tickers[i]->priority == TICKER_LOW_PRIORITY)
					&& (tickers[i]->state == TICKER_ACTIVE))
			{
				tickers[i]->ms_count = 0;

				tickers[i]->ticker_function();

				tickers[i]->calls++;

				n_exec++;
			}
		}
	}

	return n_exec;
}

ISR(TIMER0_OVF_vect)
{
	for (uint8_t i = 0 ; i < TICKER_MAX_USE ; i++)
	{
		if (tickers[i] != 0)
		{
			if (tickers[i]->state == TICKER_ACTIVE)
			{
				tickers[i]->ms_count++;
			}

			if ((tickers[i]->ms_count >= tickers[i]->ms_max) && (tickers[i]->priority == TICKER_HIGH_PRIORITY)
					&& (tickers[i]->state == TICKER_ACTIVE))
			{
				tickers[i]->ms_count = 0;

				tickers[i]->ticker_function();

				tickers[i]->calls++;
			}
		}
	}
}
