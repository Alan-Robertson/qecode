#ifndef ERROR_MODELS_DEBUG
#define ERROR_MODELS_DEBUG

// Define this for full printing
// #define ERROR_MODEL_DEBUG_PRINT_ALL

#include "error_models.h"
#include "../sym_iter.h"

double error_model_debug(error_model* m, uint32_t n_qubits)
{
	double total = 0;
	sym_iter* error = sym_iter_create_n_qubits(n_qubits);
	printf("%d\n", n_qubits);
	while (sym_iter_next(error))
	{
		// Enable to print all calls to this to diagnose errors on particular 
		// calls to the error model
		#ifdef ERROR_MODEL_DEBUG_PRINT_ALL
			printf("%e \t ", error_model_call(m, error->state));
			sym_print(error->state);
		#endif

		total += error_model_call(m, error->state);
	}
	sym_iter_free(error);
	return total;
}

#endif