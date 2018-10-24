#include "../sym_iter.h"

int main()
{
	{
		sym_iter* siter = sym_iter_create_n_qubits(5);

		while (sym_iter_next(siter))
		{
			//if (siter->ll_counter != sym_to_ll(siter->state))
			//{

				printf("%ld \t", siter->ll_counter);
				printf("%lld\t", sym_to_ll(siter->state));
				sym_print(siter->state);

			//}
		}
		sym_iter_free(siter);
	}
	

	return 0;
}