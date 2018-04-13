#ifndef SYM_DEBUG
#define SYM_DEBUG

void check_commutation(sym* tableau)
{
	sym* s_x = sym_create(tableau->height / 2, tableau->length / 2);
	sym* s_z = sym_create(tableau->length / 2, tableau->length / 2);

	for (size_t i = 0; i < tableau->height / 2; i++)
	{
		for (size_t j = 0; j < tableau->length / 2; j++)
		{
			sym_set(s_x, i, j, sym_get(tableau, i + tableau->height/2, j));
			sym_set(s_z, i, j, sym_get(tableau, i + tableau->height/2, j + tableau->length / 2));
		}
	}


	sym* s_xt = sym_transpose(s_x);
	sym* s_zt = sym_transpose(s_z);

	sym* a = sym_multiply(s_xt, s_z);
	sym* b = sym_multiply(s_zt, s_x);


	unsigned total = 0;
	for (size_t i = 0; i < s_x->height; i++)
	{
		for (size_t j = 0; j < s_x->length; j++)
		{
			total += sym_get(a, i, j) ^ sym_get(b, i, j);
		}
	}

	printf("## Commutant: %u \n", total);
	return;
}


#endif