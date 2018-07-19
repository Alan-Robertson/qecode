#ifndef GATES
#define GATES
#include "../sym.h" 
#include "../sym_iter.h"
#include "gate_result.h"
#include "../error_models/error_models.h"

// STRUCT OBJECTS ----------------------------------------------------------------------------------------

//  Gate operation function pointer
//typedef sym* (*gate_operation_f)(const sym*, void*, const unsigned* target_qubits);
typedef gate_result* (*gate_operation_f)(const sym*, const void*, const unsigned* target_qubits);

/*
 *	gate:
 *	The gate struct
 *	:: unsigned n_qubits :: Number of qubits in the gate
 *	:: error_model_f gate_error_model :: The error model to be applied when this gate is used
 *	:: void* model_data :: Data for the error model
 */
typedef struct {
	unsigned n_qubits; // Number of qubits the gate operates on
	gate_operation_f operation; // The gate operation performed (any absolute operation)
	error_model* gate_error_model; // The error operation performed (Any probabilistic operation)
	void* operation_data; // Any additional data to pass to the gate
	void* error_model_data; // Any additional data to pass to the noise
} gate;

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


gate_result* gate_operation(const gate* g, sym* initial_state, unsigned* target_qubits);

/* 
    gate_free:
	Frees a gate object
	:: gate* g :: The gate to be freed
	Does not return anything
*/
void gate_free(gate* g);


gate_result* gate_iid(const sym* initial_state, const void* gate_data, const unsigned* target_qubits);

// FUNCTION DEFINITIONS ----------------------------------------------------------------------------------------
/* 
    gate_create:
	Creates a new gate object
	:: const unsigned n_qubits :: Number of qubits in the gate
	:: const error_model em :: The error model to be applied when this gate is used
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

/* 
    gate_create_independent:
	Creates a new gate object, copies the data rather than references it
	:: const unsigned n_qubits :: Number of qubits in the gate
	:: const error_model* em :: The error model to be applied when this gate is used
	:: void* operation_data :: Data for the operation
	Returns a heap pointer to the new gate
*/
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
	// Apply the gate operation	
	double* gate_operator_probabilities = gate_operator(n_qubits, probabilities, g, target_qubits);

	// Apply the noise operation associated with that gate
	double* gate_noise_probabilities = gate_noise(n_qubits, gate_operator_probabilities, g, target_qubits);

	// Free allocated memory that is no longer required
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
	const gate* applied_gate,
	const unsigned* target_qubits)
{
	// Allocate memory for the new output
	double* p_state_probabilities = (double*)calloc(1ull << (n_qubits * 2), sizeof(double));

	// Identity gate, no operation_output
	if (NULL == applied_gate->operation)
	{
		memcpy(p_state_probabilities, initial_probabilities, sizeof(double) * 1ull << (n_qubits * 2));
		return p_state_probabilities;
	}

	// Loop over all possible states
	sym_iter* initial_state = sym_iter_create_n_qubits(n_qubits);
	while(sym_iter_next(initial_state))
	{
		// Save this value as we may be needing it quite a bit
		double initial_prob = initial_probabilities[sym_to_ll(initial_state->state)];

		if (initial_prob > 0.0)
		{
			// Determine the state after the error has been applied 
			gate_result* operation_output = gate_iid(initial_state->state, applied_gate, target_qubits);

			for (unsigned i = 0; i < operation_output->n_results; i++)
			{
				// Cumulatively determine the new probability of each state after the gate has been applied
				p_state_probabilities[sym_to_ll(operation_output->state_results[i])] += operation_output->prob_results[i] * initial_prob; 
			}

			// Free allocated memory
			gate_result_free(operation_output);
		}
	}

	sym_iter_free(initial_state);

	return p_state_probabilities;
}


// Wrapper
gate_result* gate_operation(const gate* g, sym* initial_state, const unsigned* target_qubits)
{
	return g->operation(initial_state, g, target_qubits);
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
	sym_iter* initial_state = sym_iter_create_n_qubits(n_qubits);
	while(sym_iter_next(initial_state))
	{
		// Save this value as we may be needing it quite a bit
		double initial_prob = initial_probabilities[sym_to_ll(initial_state->state)];

		if (initial_prob > 0.0)
		{
			// Determine the state after the error has been applied 
			gate_result* operation_output = gate_operation(applied_gate, initial_state->state, target_qubits);

			for (unsigned i = 0; i < operation_output->n_results; i++)
			{
				// Cumulatively determine the new probability of each state after the gate has been applied
				p_state_probabilities[sym_to_ll(operation_output->state_results[i])] += operation_output->prob_results[i] * initial_prob; 
			}
			// Free allocated memory
			gate_result_free(operation_output);
		}
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

// Applies an iid error model
gate_result* gate_iid(const sym* initial_state, const void* gate_data, const unsigned* target_qubits)
{
	gate* g = (gate*)gate_data;
	gate_result* gr = gate_result_create(1 << (g->n_qubits * 2));

	uint32_t count = 0;
	sym_iter* gate_error = sym_iter_create_n_qubits(g->n_qubits);
	while (sym_iter_next(gate_error))
	{
		gr->state_results[count] = sym_partial_add(initial_state, gate_error->state, target_qubits);
		gr->prob_results[count] = error_model_call(g->gate_error_model, gate_error->state);
		count++;
	}
	sym_iter_free(gate_error);
	return gr;
}

#endif
