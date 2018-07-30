#ifndef CIRCUIT_RECOVERY
#define CIRCUIT_RECOVERY

#include "../sym.h"
#include "circuit.h"
#include "../decoders/decoders.h"
#include "error_probabilities.h"


typedef struct {
	uint32_t n_code_qubits;
	uint32_t n_ancilla_qubits;
} circuit_recovery_data;

circuit* recovery_circuit(const sym* code, const decoder* decoders, const gate* cnot, const gate* hadamard, const gate* phase)
{
	// Qubits 0 -> length / 2 are the regular qubits, the others are ancillas
	circuit* recovery = circuit_create(code->length / 2 + code->height);

	syndrome_measurement_circuit(recovery, code, cnot, hadamard);

	return recovery;
}

void syndrome_measurement_circuit(circuit* recovery, const sym* code, const gate* cnot, const gate* hadamard, const gate* phase)
{
	size_t start_ancilla = code->n_qubits;

	// Pauli Z checks
	for (size_t i = 0; i < code->n_qubits; i++)
	{
		for (size_t j = 0; j < code->height; j++)
		{
			if (sym_is_Z(code, j, i))
			{
				circuit_add_gate(recovery, cnot, i, j + start_ancilla);
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
					circuit_add_gate(recovery, hadamard, i);
				}

				circuit_add_gate(recovery, cnot, i, j + start_ancilla);
			}
		}

		// Qubit has been mapped to the X basis, map it back
		if (found)
		{
			circuit_add_gate(recovery, hadamard, i);
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
				{
					circuit_add_gate(recovery, phase, i);
					circuit_add_gate(recovery, hadamard, i);
				}


				circuit_add_gate(recovery, cnot, i, j + start_ancilla);
			}
		}

		// Qubit has been mapped to the Y basis, map it back
		if (found)
		{
			circuit_add_gate(recovery, hadamard, i);
			circuit_add_gate(recovery, phase, i);
			circuit_add_gate(recovery, phase, i);
			circuit_add_gate(recovery, phase, i);
		}
	}

	return;
}


circuit* circuit_syndrome_measurement_create(
	const unsigned int n_qubits, // Includes ancilla qubits
	const sym* code, 
	const gate* cnot, 
	const gate* hadamard, 
	const gate* phase)
{
	circuit* recovery = circuit_create(n_qubits);
	recovery->circuit_operation = circuit_syndrome_measurement_run;
	syndrome_measurement_circuit(circuit* recovery, code, cnot, hadamard, phase);

	circuit_recovery_data* rd = (circuit_recovery_data*)malloc(sizeof(circuit_recovery_data));
	rd->n_ancilla_qubits = code->height;
	rd->n_code_qubits = code->length / 2;
	recovery->circuit_data = rd;

	return recovery;
}

double* circuit_syndrome_measurement_run(circuit* recovery, double* initial_error_rates, gate* noise)
{

	circuit_recovery_data* rd = (circuit_recovery_data*)recovery->circuit_data;
	double* expanded_error_probs = error_probabilities_zeros(rd->n_sancilla_qubits + rd->n_code_qubits);

	// Copy the errors from the initial buffer to our larger buffer
	sym_iter* cpy_iter = sym_iter_create_n_qubits(rd->n_code_qubits);
	sym_iter* target_buffer = sym_iter_create_n_qubits(rd->n_ancilla_qubits + rd->n_code_qubits);
	while (sym_iter_next(cpy_iter))
	{
		if (initial_error_rates[sym_iter_ll_from_state(cpy_iter)] > 0)
		{
			// Copy the state to the target buffer
			for (uint32_t i = 0; i < n_code_qubits; i++)
			{ // All other values in the target buffer should be zero
				sym_set(target_buffer->state, 0, i, sym_get(cpy_iter->state, 0, i)); // X elements
 				sym_set(target_buffer->state, 0, i + target_buffer->state->n_qubits, sym_get(cpy_iter->state, 0, i + cpy_iter->state->n_qubits)); // Z elements
			} 

			// Set the value in the new buffer
			expanded_error_probs[sym_iter_ll_from_state(target_buffer)] += initial_error_rates[sym_iter_ll_from_state(cpy_iter)];
		}
	}
	sym_iter_free(target_buffer);
	sym_iter_free(cpy_iter);

	
	// Iterate over the gates in the circuit
	double* output_error_rates = circuit_run_default(recovery, expanded_error_probs, noise);

	// Measure and recover?
	// TODO

	// Strip off the ancilla qubits
	double* final_error_rates = error_probabilities_zeros(rd->n_code_qubits);

	// Copy the errors from the initial buffer to our larger buffer
	sym_iter* cpy_iter = sym_iter_create_n_qubits(rd->n_code_qubits + rd->n_ancilla_qubits);
	sym_iter* target_buffer = sym_iter_create_n_qubits(rd->n_code_qubits);
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

void syndrome_measurement_circuit(circuit* recovery, const sym* code, const gate* cnot, const gate* hadamard, const gate* phase)
{
	size_t start_ancilla = code->length / 2;

	// Pauli Z checks
	for (size_t i = 0; i < code->length / 2; i++)
	{
		for (size_t j = 0; j < code->height; j++)
		{
			if (sym_is_Z(code, j, i))
			{
				circuit_add_gate(recovery, cnot, i, j + start_ancilla);
			}
		}
	}

	// Pauli X checks
	for (size_t i = 0; i < code->length / 2; i++)
	{
		bool found = false;
		for (size_t j = 0; j < code->height; j++)
		{
			if (sym_is_X(code, j, i))
			{
				if (false == found)
				{
					circuit_add_gate(recovery, hadamard, i);
				}

				circuit_add_gate(recovery, cnot, i, j + start_ancilla);
			}
		}

		// Qubit has been mapped to the X basis, map it back
		if (found)
		{
			circuit_add_gate(recovery, hadamard, i);
		}
	}

	// Pauli Y checks
	for (size_t i = 0; i < code->length / 2; i++)
	{
		bool found = false;
		for (size_t j = 0; j < code->height; j++)
		{
			if (sym_is_Y(code, j, i))
			{
				if (false == found)
				{
					circuit_add_gate(recovery, phase, i);
					circuit_add_gate(recovery, hadamard, i);
				}
				circuit_add_gate(recovery, cnot, i, j + start_ancilla);
			}
		}

		// Qubit has been mapped to the Y basis, map it back
		if (found)
		{
			circuit_add_gate(recovery, hadamard, i);
			circuit_add_gate(recovery, phase, i);
			circuit_add_gate(recovery, phase, i);
			circuit_add_gate(recovery, phase, i);
		}
	}
	return;
}


#endif