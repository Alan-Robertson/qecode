#include <stdio.h>
#include "sym_iter.h"

int main()
{
	printf("### Full range ###\n");
	sym_iter* s = sym_iter_create(3);
	while (sym_iter_next(s))
	{
		printf("%lu %lu\n", s->ll_counter, s->max_ll_counter);
		sym_print(s->state);
	}
	sym_iter_free(s);
	
	printf("### Limited range ###\n");
	sym_iter* t = sym_iter_create_range(3, 1, 3);
	while (sym_iter_next(t))
	{
		printf("%lu %lu\n", t->ll_counter, t->max_ll_counter);
		sym_print(t->state);
	}
	sym_iter_free(t);

	printf("### Limited range ###\n");
	sym_iter* w = sym_iter_create_range(5, 2, 4);
	while (sym_iter_next(w))
	{
		printf("%lu %lu\n", w->ll_counter, w->max_ll_counter);
		sym_print(w->state);
	}
	sym_iter_free(w);

	printf("### N Qubits ###\n");
	sym_iter* r = sym_iter_create_n_qubits(7);
	while (sym_iter_next(r))
	{
		printf("%lu %lu\n", r->ll_counter, r->max_ll_counter);
		sym_print(r->state);
	}
	sym_iter_free(r);

	return 0;
}