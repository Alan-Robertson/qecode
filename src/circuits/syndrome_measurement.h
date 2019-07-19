#ifndef CIRCUIT_SYNDROME_MEASUREMENT
#define CIRCUIT_SYNDROME_MEASUREMENT

// ----------------------------------------------------------------------------------------
// DIRECTIVES
// ----------------------------------------------------------------------------------------

#include "../sym.h"
#include "circuit.h"
#include "error_probabilities.h"

// ----------------------------------------------------------------------------------------
// STRUCTS
// ----------------------------------------------------------------------------------------

typedef struct {
	uint32_t n_code_qubits;
	uint32_t n_ancilla_qubits;
} circuit_syndrome_measurement_data_t;

// ----------------------------------------------------------------------------------------
// FUNCTION DECLARATIONS 
// ----------------------------------------------------------------------------------------

circuit* syndrome_measurement_circuit_create(
	const sym* code,
	gate* cnot,
	gate* hadamard,
	gate* phase);

void syndrome_measurement_circuit_construct(
	circuit* syndrome_measurement,
	const sym* code,
	gate* cnot,
	gate* hadamard,
	gate* phase);

/*
 * circuit_syndrome_measurement_run
 * 
 * :: circuit* recovery ::
 * :: double* initial_error_rates ::
 * :: gate* noise :: 
 * 
 */
double* circuit_syndrome_measurement_run(
	circuit* recovery, 
	double* initial_error_rates, 
	gate* noise);

// ----------------------------------------------------------------------------------------
// FUNCTION DEFINITIONS
// ----------------------------------------------------------------------------------------

circuit* syndrome_measurement_circuit_create(
	const sym* code,
	gate* cnot,
	gate* hadamard,
	gate* phase)
{
	// Qubits 0 -> n_qubits are the regular qubits, the others are ancillas
	circuit* syndrome_measurement = circuit_create(code->n_qubits + code->height);

	// Override the operation that runs the circuit
	syndrome_measurement->circuit_operation = circuit_syndrome_measurement_run;

	// Setup the circuit data
	circuit_syndrome_measurement_data_t* circuit_data = (circuit_syndrome_measurement_data_t*)malloc(sizeof(circuit_syndrome_measurement_data_t));
	circuit_data->n_code_qubits = code->n_qubits;
	circuit_data->n_ancilla_qubits = code->height;
	syndrome_measurement->circuit_data = circuit_data;

	// Construct the gates for the circuit
	syndrome_measurement_circuit_construct(syndrome_measurement, code, cnot, hadamard, phase);

	return syndrome_measurement;
}

void syndrome_measurement_circuit_construct(
	circuit* syndrome_measurement,
	const sym* code,
	gate* cnot,
	gate* hadamard,
	gate* phase)
{
	size_t start_ancilla = code->n_qubits;

	// Pauli Z checks
	for (size_t i = 0; i < code->n_qubits; i++)
	{
		for (size_t j = 0; j < code->height; j++)
		{
			if (sym_is_Z(code, j, i))
			{
				// Cnot in the Z basis
				circuit_add_gate(syndrome_measurement, cnot, i, j + start_ancilla);
			}
		}
	}

	// Pauli X checks
	for (size_t i = 0; i < code->n_qubits; i++)
	{
		bool found = false;
		for (size_t j = 0; j < code->height; j++)
		{
			if (sym_is_X(code, j, i))
			{
				if (false == found)
				{
					circuit_add_gate(syndrome_measurement, hadamard, i);
					found = true;
				}
				circuit_add_gate(syndrome_measurement, cnot, i, j + start_ancilla);
			}
		}

		// Cnot in the X basis
		if (found)
		{
			circuit_add_gate(syndrome_measurement, hadamard, i);
		}
	}

	// Pauli Y checks
	for (size_t i = 0; i < code->n_qubits; i++)
	{
		bool found = false;
		for (size_t j = 0; j < code->height; j++)
		{
			if (sym_is_Y(code, j, i))
			{
				if (false == found)
				{ // Map Y to Z then cnot to pass the error along
					circuit_add_gate(syndrome_measurement, phase, i);
					circuit_add_gate(syndrome_measurement, phase, i);
					circuit_add_gate(syndrome_measurement, phase, i);
					circuit_add_gate(syndrome_measurement, hadamard, i);
					found = true;
				}
				// Cnot in the Y basis
				circuit_add_gate(syndrome_measurement, cnot, i, j + start_ancilla);
			}
		}

		// Qubit has been mapped to the Y basis, map it back
		if (found)
		{
			circuit_add_gate(syndrome_measurement, hadamard, i);
			circuit_add_gate(syndrome_measurement, phase, i);
		}
	}
	return;
}


/*
 * circuit_syndrome_measurement_run
 * 
 * :: circuit* recovery ::
 * :: double* initial_error_rates ::
 * :: gate* noise :: 
 * 
 */
double* circuit_syndrome_measurement_run(
	circuit* recovery, 
	double* initial_error_rates, 
	gate* noise)
{
	// Unpack the syndrome measurement data
	circuit_syndrome_measurement_data_t* smd = (circuit_syndrome_measurement_data_t*)recovery->circuit_data;

	// Setup the larger state space
	double* expanded_error_probs = error_probabilities_zeros(smd->n_ancilla_qubits + smd->n_code_qubits);

	// Copy the errors from the initial buffer to our larger buffer
	sym_iter* cpy_iter = sym_iter_create_n_qubits(smd->n_code_qubits);
	sym_iter* target_buffer = sym_iter_create_n_qubits(smd->n_ancilla_qubits + smd->n_code_qubits);
	while (sym_iter_next(cpy_iter))
	{
		if (initial_error_rates[sym_iter_ll_from_state(cpy_iter)] > 0)
		{
			// Copy the state to the target buffer
			for (uint32_t i = 0; i < smd->n_code_qubits; i++)
			{ // All other values in the target buffer should be zero
				sym_set(target_buffer->state, 0, i, sym_get(cpy_iter->state, 0, i)); // X elements
 				sym_set(target_buffer->state, 0, i + target_buffer->state->n_qubits, sym_get(cpy_iter->state, 0, i + cpy_iter->state->n_qubits)); // Z elements
			} 
			
			// Update the other values in the buffer
			sym_iter_update(target_buffer);
			
			// Set the value in the new buffer
			expanded_error_probs[sym_iter_ll_from_state(target_buffer)] += initial_error_rates[sym_iter_ll_from_state(cpy_iter)];
		}
	}
	sym_iter_free(target_buffer);
	sym_iter_free(cpy_iter);

	// Iterate over the gates in the circuit
	double* output_error_rates = circuit_run_default(recovery, expanded_error_probs, noise);

	// Cleanup anything that needs to be de-allocated
	error_probabilities_free(expanded_error_probs);

	return output_error_rates;
}


#endif