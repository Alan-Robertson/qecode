#ifndef CIRCUIT_RECOVERY
#define CIRCUIT_RECOVERY

#include "../sym.h"
#include "circuit.h"
#include "error_probabilities.h"
#include "../decoders/decoders.h"

typedef struct {
	uint32_t n_code_qubits;
	uint32_t n_ancilla_qubits;
	decoder* d;
	gate* pauli_X;
	gate* pauli_Y;
	gate* pauli_Z;
	gate* measure_Z;
} circuit_recovery_data_t;


circuit* circuit_recovery_create(
	uint32_t n_code_qubits,
	uint32_t n_ancilla_qubits,
	decoder* d,
	gate* pauli_X,
	gate* pauli_Y,
	gate* pauli_Z,
	gate* measure_Z)
{
	circuit* recovery = circuit_create(code->n_qubits + code->height);
	recovery->circuit_run = circuit_recovery_run;
	recovery->circuit_param_free = circuit_recovery_param_free;
		
	// Measure the ancilla qubits to get the syndromes
	uint32_t* measurement_targets = (uint32_t*)malloc(sizeof(uint32_t) * n_ancilla_qubits);
	for (int i = 0; i < n_ancilla_qubits; i++)
	{
		measurement_targets[i] = n_code_qubits + i;
	}
	circuit_add_gate_non_varg(recovery, measure_Z, measurement_targets);
	free(measurement_targets);

	// Use the decoder to determine the 
}


double* circuit_recovery_run(
	circuit* recovery, 
	double* initial_error_rates, 
	gate* noise)
{

	circuit_recovery_data_t rd = (*circuit_recovery_data_t)c->circuit_data;

	sym_iter* siter = sym_iter_create_n_qubits()
	// Measure
	

	// Decode
	sym* correction = 

	// Correct

	for (uint32_t = 0; i < rd->n_qubits; i++)
	{
		if () // Apply an X

		if () // Apply a Y

		if () // Apply a Z 
	}


	// Strip off the ancilla qubits
	double* final_error_rates = error_probabilities_zeros(smd->n_code_qubits);

	// Copy the errors from the initial buffer to our larger buffer
	sym_iter* cpy_iter = sym_iter_create_n_qubits(smd->n_code_qubits + smd->n_ancilla_qubits);
	sym_iter* target_buffer = sym_iter_create_n_qubits(smd->n_code_qubits);
	while (sym_iter_next(cpy_iter))
	{
		if (output_error_rates[sym_iter_ll_from_state(cpy_iter)] > 0.0)
		{
			// Copy the state to the target buffer
			for (uint32_t i = 0; i < n_code_qubits; i++)
			{ // All other values in the target buffer should be zero
				sym_set(target_buffer->state, 0, i, sym_get(cpy_iter->state, 0, i)); // X elements
 				sym_set(target_buffer->state, 0, i + target_buffer->state->n_qubits, sym_get(cpy_iter->state, 0, i + cpy_iter->state->n_qubits)); // Z elements
			} 

			// Set the value in the new buffer
			final_error_rates[sym_iter_ll_from_state(target_buffer)] += output_error_rates[sym_iter_ll_from_state(cpy_iter)];
		}
	}
	sym_iter_free(target_buffer);
	sym_iter_free(cpy_iter);

	sym_free(output_error_rates);
	sym_free(expanded_error_probs);

	return final_error_rates;
}

#endif