#ifndef GATE_MEASUREMENT
#define GATE_MEASUREMENT

#include "../sym.h"
#include "gate_result.h"
#include "gate.h"

typedef struct {
    uint32_t n_bits;
} gate_data_recovery_t;

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
    	if (sym_is_I(initial_state, 0, target_qubits[i]) || sym_is_Z(initial_state, 0, target_qubits[i]))
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
    sym_iter sym_iter_create(n_outputs);
    while(sym_iter_next(initial_state))
    {

    }

    return gr;
}

#endif