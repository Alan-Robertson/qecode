#ifndef PROGRESS_BAR
#define PROGRESS_BAR

struct progress_bar {
	unsigned max_iters; 
	unsigned curr_iter; 
	char* name; 
	unsigned bar_length;
};

struct progress_bar progress_bar_create(const unsigned max_iters, char* name)
{
	struct progress_bar p = {max_iters, 0, name, 20};
	setbuf(stdout, NULL);
	return p;
}

void progress_bar_update(struct progress_bar* p)
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

#endif