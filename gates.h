#ifndef GATES
#define GATES
#include "sym.h" 
#include "error_models.h"


typedef struct {
	unsigned n_qubits;
	error_model_f error_model;
	void* model_data;
} gate;

gate* gate_build(
	const unsigned n_qubits, 
	const error_model_f error_model, 
	void* model_data)
{
	gate* g = (gate*)malloc(sizeof(gate));
	g->n_qubits = n_qubits;
	g->error_model = error_model;
	g->model_data = model_data;
	return g;
}

double* gate_apply_noisy(unsigned n_qubits, 
	double* initial_probabilities, 
	gate* noisy_gate, 
	unsigned* target_qubits)
{
	double* p_error_probabilities = (double*)calloc(1ull << (n_qubits * 2), sizeof(double));

	sym_iter* gate_error = sym_iter_create(noisy_gate->n_qubits * 2);
	while (sym_iter_next(gate_error))
	{
		//sym_print(gate_error->state);
		
		double p_error = noisy_gate->error_model(gate_error->state, noisy_gate->model_data);
		if (p_error > 0)
		{ 
			sym_iter* initial_state = sym_iter_create(n_qubits * 2);
			while(sym_iter_next(initial_state))
			{

				sym* physical_error = sym_partial_add(initial_state->state, gate_error->state, target_qubits);
				p_error_probabilities[sym_to_ll(physical_error)] += initial_probabilities[sym_to_ll(initial_state->state)] * p_error; 
			}
			sym_iter_free(initial_state);
		}
	}
	sym_iter_free(gate_error);

	return p_error_probabilities;
}

#endif