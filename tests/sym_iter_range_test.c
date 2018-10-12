#include "../sym_iter.h"

int main()
{
	// uint32_t n_qubits = 3;
	// for (int i = 0; i < 5; i++)
	// {
	// 	sym_iter* siter = sym_iter_create_range(2 * n_qubits, i, i + 1);

	// 	while(sym_iter_next(siter))
	// 	{
	// 		printf("%ld \t", siter->ll_counter);
	// 		sym_print(siter->state);
	// 	}
	// 	printf("-----\n");
	// 	sym_iter_free(siter);
	// }

	// for (int i = 1; i < n_qubits; i++)
	// {
	// 	sym_iter* siter = sym_iter_create_n_qubits(i);

	// 	while (sym_iter_next(siter))
	// 	{
	// 		printf("%ld \t", siter->ll_counter);
	// 		sym_print(siter->state);
	// 	}
	// 	sym_iter_free(siter);
	// }

	// for (int i = 1; i < n_qubits; i++)
	// {
	// 	sym_iter* siter = sym_iter_create_n_qubits_range(n_qubits, 1, 2);

	// 	while (sym_iter_next(siter))
	// 	{
	// 		printf("%ld \t", siter->ll_counter);
	// 		sym_print(siter->state);
	// 	}
	// 	sym_iter_free(siter);
	// }

	{
		sym_iter* siter = sym_iter_create_n_qubits(3);

		while (sym_iter_next(siter))
		{
			printf("%ld \t", siter->ll_counter);
			sym_print(siter->state);
		}
		sym_iter_free(siter);
	}
	

	return 0;
}