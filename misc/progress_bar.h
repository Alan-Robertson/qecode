#ifndef PROGRESS_BAR
#define PROGRESS_BAR
#include <string.h>

typedef struct {
	unsigned max_iters; 
	unsigned curr_iter; 
	char* name; 
	unsigned bar_length;
}  progress_bar;

progress_bar* progress_bar_create(const unsigned max_iters, char const* name)
{
	progress_bar* p = (progress_bar*)malloc(sizeof(progress_bar));
	p->max_iters = max_iters;
	p->curr_iter = 0;
	p->bar_length = 20;
	p->name = (char*)malloc(sizeof(char) * strlen(name));
	strcpy(p->name, name);
	setbuf(stdout, NULL);
	return p;
}

void progress_bar_update(progress_bar* p)
{
	p->curr_iter++;
	fflush(stdout);
	printf("\r %s ", p->name);
	printf("[");
	unsigned progress = ((double) p->curr_iter / (double) p->max_iters) * p->bar_length;
	for (unsigned i = 0; i < p->bar_length; i++)
	{
		if ( i <= progress )
		{
			printf("#");	
		}
		else
		{
			printf("-");
		}
	}
	printf("]");
	printf( " %d Percent Complete", (unsigned)(100.0 * ((double) p->curr_iter / (double) p->max_iters)));
}

void progress_bar_free(progress_bar* p)
{
	free(p->name);
	free(p);
	printf("\n");
	return;
}

#endif