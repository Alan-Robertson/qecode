#ifndef GATE_RESULT
#define GATE_RESULT

/*
	gate_result:
	The gate result struct, when a gate is called, it returns this object
	:: unsigned n_qubits :: Number of qubits in the gate
	:: error_model_f gate_error_model :: The error model to be applied when this gate is used
	:: void* model_data :: Data for the error model
*/
typedef struct {
	sym** state_results;
	double* prob_results;
	uint32_t n_results;
} gate_result;

// Create a new gate result object
gate_result* gate_result_create(const uint32_t n_results)
{
	gate_result* gr = (gate_result*)malloc(sizeof(gate_result));

	gr->prob_results = (double*)malloc(sizeof(double) * n_results);

	gr->state_results = (sym**)malloc(sizeof(sym*) * n_results);

	gr->n_results = n_results;
	return gr;
}

// Create the single case
gate_result* gate_result_create_single(double prob_result, sym* result)
{
	// Create our gate result object with a single entry
	gate_result* gr = gate_result_create(1);

	// 
	gr->state_results[0] = result;

	gr->prob_results[0] = prob_result;

	return gr;
}

// Freeing a gate result object
void gate_result_free(gate_result* gr)
{
	for (uint32_t i = 0; i < gr->n_results; i++)
	{
		sym_free(gr->state_results[i]);
	}
	free(gr->state_results);
	free(gr->prob_results);
	free(gr);
	return;
}

#endif