#include "../sym_iter.h"

int main()
{
	uint32_t n_qubits = 3;
	for (int i = 0; i < 4; i++)
	{
		sym_iter* siter = sym_iter_create_range(2 * n_qubits, i, i + 1);
		while(sym_iter_next(siter))
		{
			sym_print(siter->state);
		}
		printf("-----\n");
		sym_iter_free(siter);
	}

	return 0;
}