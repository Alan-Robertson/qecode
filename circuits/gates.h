#ifndef GATES
#define GATES
#include "../sym.h" 
#include "../sym_iter.h"
#include "../error_models/error_models.h"
#include "gate_operations.h"


// FUNCTION DECLARATIONS ----------------------------------------------------------------------------------------
/* 
    gate_create:
	Creates a new gate object
	:: const unsigned n_qubits :: Number of qubits in the gate
	:: const error_model_f error_model :: The error model to be applied when this gate is used
	:: void* model_data :: Data for the error model
	Returns a heap pointer to the new gate
*/
gate* gate_create(
	const unsigned n_qubits, 
	gate_operation_f operation,
	error_model* em,
	void* gate_data);

/* 
    gate_apply:
	Applies a gate object to an existing noise model this is comprised of both a noise operator and a gate operator
	:: const unsigned n_qubits :: Number of qubits in the gate
	:: const double* initial_probabilities :: The current probabilities for each noise operator
	:: const void* noisy_gate :: The gate to be applied
	Returns a heap pointer to a block of allocated memory containing the new probabilities
*/
double* gate_apply(const unsigned n_qubits,
	double* probabilities,
	const gate* g,
	const unsigned* target_qubits);

/* 
    gate_noise:
	Applies a gate object to an existing noise model
	:: const unsigned n_qubits :: Number of qubits in the gate
	:: const double* initial_probabilities :: The current probabilities for each noise operator
	:: const void* noisy_gate :: The gate to be applied
	Returns a heap pointer to a block of allocated memory containing the new probabilities
*/
double* gate_noise(const unsigned n_qubits, 
	double* probabilities, 
	const gate* g,
	const unsigned* target_qubits);

/* 
    gate_operator:
	Applies a gate object to an existing noise model
	:: const unsigned n_qubits :: Number of qubits in the gate
	:: const double* initial_probabilities :: The current probabilities for each noise operator
	:: const void* noisy_gate :: The gate to be applied
	Returns a heap pointer to a block of allocated memory containing the new probabilities
*/
double* gate_operator(const unsigned n_qubits, 
	double* probabilities, 
	const gate* g,
	const unsigned* target_qubits);

/* 
    gate_free:
	Frees a gate object
	:: gate* g :: The gate to be freed
	Does not return anything
*/
void gate_free(gate* g);

// FUNCTION DEFINITIONS ----------------------------------------------------------------------------------------
/* 
    gate_create:
	Creates a new gate object
	:: const unsigned n_qubits :: Number of qubits in the gate
	:: const error_model_f error_model :: The error model to be applied when this gate is used
	:: void* error_model_data :: Data for the error model
	Returns a heap pointer to the new gate
*/
gate* gate_create(
	const unsigned n_qubits,
	gate_operation_f operation,
	error_model* em,
	void* operation_data)
{
	gate* g = (gate*)malloc(sizeof(gate));

	// Add all the arguments to the appropriate fields of the struct
	g->n_qubits = n_qubits;
	g->operation = operation;
	g->gate_error_model = em;
	g->operation_data = operation_data;
	
	return g;
}

gate* gate_create_independent(
	const unsigned n_qubits,
	gate_operation_f operation,
	error_model* em,
	void* operation_data)
{
	gate* g = (gate*)malloc(sizeof(gate));

	// Add all the arguments to the appropriate fields of the struct
	g->n_qubits = n_qubits;
	g->operation = operation;
	g->gate_error_model = error_model_copy(em);
	g->operation_data = operation_data;
	
	return g;
}

gate* gate_create_iid_noise(error_model* em)
{
	return gate_create(1, NULL, em, NULL);
}


double* gate_apply(const unsigned n_qubits,
	double* probabilities,
	const gate* g,
	const unsigned* target_qubits)
{	
	double* gate_operator_probabilities = gate_operator(n_qubits, probabilities, g, target_qubits);
	double* gate_noise_probabilities = gate_noise(n_qubits, gate_operator_probabilities, g, target_qubits);

	free(gate_operator_probabilities);
	return gate_noise_probabilities;
}

/* 
    gate_noise:
	Applies a noise object to an existing noise model
	:: const unsigned n_qubits :: Number of qubits in the gate
	:: const double* initial_probabilities :: The current probabilities for each noise operator
	:: const gate* g:: The gate to be applied
	Returns a heap pointer to a block of allocated memory containing the new probabilities
*/
double* gate_noise(const unsigned n_qubits, 
	double* initial_probabilities, 
	const gate* g,
	const unsigned* target_qubits)
{
	// Allocate memory for the new output probabilities
	double* p_error_probabilities = (double*)calloc(sizeof(double),  1ull << (n_qubits * 2));
	
	// Noiseless gate, no operation
	if (NULL == g->gate_error_model)
	{
		memcpy(p_error_probabilities, initial_probabilities, sizeof(double) * 1ull << (n_qubits * 2));
		return p_error_probabilities;
	}

	// Use the sym_iterator to loop over all the possible errors caused by this noise model
	sym_iter* gate_error = sym_iter_create(g->n_qubits * 2);
	while (sym_iter_next(gate_error))
	{
		// Determine the error rate associated with each possible error
		double p_error = error_model_call(g->gate_error_model, gate_error->state);

		// If the error rate is non zero, apply the error
		if (p_error > 0.0)
		{
			// Loop over all possible states
			sym_iter* initial_state = sym_iter_create(n_qubits * 2);
			while(sym_iter_next(initial_state))
			{
				if (initial_probabilities[sym_to_ll(initial_state->state)] > 0)
				{
					// Determine the state after the error has been applied
					sym* physical_error = sym_partial_add(initial_state->state, gate_error->state, target_qubits);

					// Cumulatively determine the new probability of each state after the gate has been applied
					p_error_probabilities[sym_to_ll(physical_error)] += initial_probabilities[sym_to_ll(initial_state->state)] * p_error; 
				
					// Free allocated memory
					sym_free(physical_error);
				}
			}
			sym_iter_free(initial_state);
		}
	}
	sym_iter_free(gate_error);

	// Return the new probabilities
	return p_error_probabilities;
}

/* 
    gate_operator:
	Applies a gate object to an existing noise model
	:: const unsigned n_qubits :: Number of qubits in the gate
	:: const double* initial_probabilities :: The current probabilities for each noise operator
	:: const void* noisy_gate :: The gate to be applied
	Returns a heap pointer to a block of allocated memory containing the new probabilities
*/
double* gate_operator(const unsigned n_qubits,
	double* initial_probabilities,
	const gate* applied_gate,
	const unsigned* target_qubits)
{
	// Allocate memory for the new output
	double* p_state_probabilities = (double*)calloc(1ull << (n_qubits * 2), sizeof(double));

	// Identity gate, no operation
	if (NULL == applied_gate->operation)
	{
		memcpy(p_state_probabilities, initial_probabilities, sizeof(double) * 1ull << (n_qubits * 2));
		return p_state_probabilities;
	}

	// Loop over all possible states
	sym_iter* initial_state = sym_iter_create(n_qubits * 2);
	while(sym_iter_next(initial_state))
	{
		// Determine the state after the error has been applied
		sym* operation_output = applied_gate->operation(initial_state->state, applied_gate->operation_data, target_qubits);
		
		// Cumulatively determine the new probability of each state after the gate has been applied
		p_state_probabilities[sym_to_ll(operation_output)] += initial_probabilities[sym_to_ll(initial_state->state)]; 

		// Free allocated memory
		sym_free(operation_output);
	}
	sym_iter_free(initial_state);

	return p_state_probabilities;
}

/* 
    gate_free:
	Frees a gate object
	:: gate* g :: The gate to be freed
	Does not return anything
*/
void gate_free(gate* g)
{
	free(g);
}

#endif