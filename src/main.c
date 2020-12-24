#include "uvc.h"

int main (void)
{
	uvc_init();

	while (1)
	{
		uvc_exec();
	}
}
