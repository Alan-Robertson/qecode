#ifndef SYM_DEBUG
#define SYM_DEBUG

void check_commutation(sym* tableau)
{
	printf("#####\n");
	sym_print(tableau);
	for (size_t i = tableau->height / 2; i < tableau->height; i++)
	{
		for (size_t j = i + 1; j < tableau->height; j++)
		{
			unsigned total = 0;
			for (size_t k = 0; k < tableau->length; k++)
			{
				total += sym_get(tableau, i, k) & sym_get(tableau, j, (k + tableau->length / 2) % tableau->length);
			}
			total %= 2;
			printf("Commutants [%lu %lu]: %d\n", i - tableau->height / 2, j - tableau->height / 2, total);

		}
	}

	for (size_t i = 0; i < tableau->height / 2; i++)
	{
		for (size_t j = i + 1; j < tableau->height / 2; j++)
		{
			unsigned total = 0;
			for (size_t k = 0; k < tableau->length; k++)
			{
				total += sym_get(tableau, i, k) & sym_get(tableau, j, (k + tableau->length / 2) % tableau->length);
			}
			total %= 2;
			printf("Destabilisers [%lu %lu]: %d\n", i, j, total);

		}
	}

	printf("\n");
	return;
}



#endif