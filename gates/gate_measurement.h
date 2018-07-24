#ifndef GATE_MEASUREMENT
#define GATE_MEASUREMENT

#include "../sym.h"
#include "gate_result.h"
#include "gate.h"

typedef struct {
    uint32_t n_bits;
} gate_data_recovery_t;

typedef struct {
    uint32_t n_bits;
} gate_data_prepare_t;

gate_result* gate_prepare_Z(const sym* initial_state, void* gate_data, const unsigned* target_qubits)
{
	sym* final_state = sym_copy(initial_state);
	uint32_t n_bits = ((gate_data_recovery_t*)gate_data)->n_bits;
	for (int i = 0; i < n_bits; i++)
	{
		sym_set(final_state, 0, target_qubits[i], 0);
		sym_set(final_state, 0, target_qubits[i] + initial_state->length / 2, 1);
	}	
	return gate_result_create_single(1.0, final_state);
}

gate_result* gate_measure_Z(const sym* initial_state, void* gate_data, const unsigned* target_qubits)
{
	uint32_t n_bits = ((gate_data_recovery_t*)gate_data)->n_bits;

	// Find the number of probabilistic syndromes that can be measured 
	// Given some fault in the ancilla qubit
	uint32_t n_outputs = 1;
	uint32_t n_undecided = 0;
	double p_output = 1.0;
    for (int i = 0; i < n_bits; i++)
    {
    	if (sym_is_X(initial_state, 0, target_qubits[i]) 
    		|| sym_is_Y(initial_state, 0, target_qubits[i]))
    	{
    		n_outputs <<= 1;
    		p_output /= 2;
    		n_undecided++;
    	}
    }

    uint32_t* undecided_bits = (*uint32_t)malloc(sizeof(uint32_t) * n_undecided);

    // Find the base result (before concerning ourselves with measurement)
    // Also construct an array of the positions of all the undecided bits
    sym* base_result = sym_create(1, n_bits);
    uint32_t tmp = 0;
    for (int i = 0; i < n_bits; i++)
    {	/* Fix this bit up later */
    	if (sym_is_I(initial_state, 0, target_qubits[i]))
    	{
    		sym_set(base_result, 0, target_qubits[i], 1);	
    	}
    	else if (sym_is_Z(initial_state, 0, target_qubits[i]))
    	{
    		sym_set(base_result, 0, target_qubits[i], 0);
    	}
    	else (sym_is_X(initial_state, 0, target_qubits[i]) || sym_is_Y(initial_state, 0, target_qubits[i]))
    	{
    		undecided_bits[tmp] = i;
    		tmp++;
    	}
    }

    gate_result* gr = gate_result_create(n_outputs);
    for (uint32_t i = 0; i < n_outputs; i++)
    {
    	gr->state_results[i] = sym_copy(base_result);
    	gr->prob_results = p_output;
    }

    // Iterate over all the probabilistic outputs
    sym_iter* output_states = sym_iter_create(n_outputs);
    while(sym_iter_next(output_states))
    {



    }
    sym_iter_free(output_states);

    return gr;
}


typedef struct {
    uint32_t n_bits;
    sym* code;
    const unsigned* code_qubits;
} gate_data_syndrome_measurement_t;

gate_result* gate_measure_syndromes(const sym* initial_state, void* gate_data, const unsigned* target_qubits)
{
	gate_data_syndrome_measurement_t* sm = (gate_data_syndrome_measurement_t*)gate_data;
	sym* error = sym_create(1, sm->code->length);

	for (uint32_t i = 0; i < sm->code->length / 2; i++)
	{
		sym_set(error, 0, i, sym_get(initial_state, 0, code_qubits[i]));
		sym_set(error, 0, i + error->length / 2, sym_get(initial_state, 0, code_qubits[i] + initial_state->length / 2));
	}
	sym* syndrome = sym_syndrome(sm->code, error);
	sym_free(error);

	for (uint32_t i = 0; i < sm->n_bits; i++)
	{
		sym_xor(syndrome, 0, i, sym_get());
	}

}

#endif