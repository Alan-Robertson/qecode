#include <stdio.h>
#include <unistd.h>
#include "progress_bar.h"

int main()
{
	int bar_size = 1000;
	char name[] = "Bar";
	struct progress_bar p = progress_bar_create(bar_size, name);

	for (int i = 0; i <= bar_size; i++)
	{
		progress_bar_update(p, i);
		usleep(1000);
	}
	return 0;
}