#ifndef GATE_CLIFFORD_GENERATORS
#define GATE_CLIFFORD_GENERATORS

#include "gates.h"
#include "gate_result.h"

#include "../sym.h"
#include "../error_models/error_models.h"

//typedef struct {} gate_data_cnot_t;
gate_result* gate_cnot(const sym* initial_state, const void* gate_data, const unsigned* target_qubits);

//typedef struct {} gate_data_hadamard_t;
gate_result* gate_hadamard(const sym* initial_state, const void* gate_data, const unsigned* target_qubit);

//typedef struct {} gate_data_phase_t;
gate_result* gate_phase(const sym* initial_state, const void* gate_data, const unsigned* target_qubit);

//typedef struct {} gate_data_identity_t;
gate_result* gate_identity(const sym* initial_state, const void* gate_data, const unsigned* target_qubits);


// GATE OBJECTS ----------------------------------------------------------------------------------------

// 0 is control, 1 is target
gate_result* gate_cnot(const sym* initial_state, const void* gate_data, const unsigned* target_qubits)
{
    sym* final_state = sym_copy(initial_state);

    sym_set(final_state, 0, target_qubits[1] + initial_state->length / 2, sym_get(final_state, 0, target_qubits[1] + initial_state->length / 2) ^ sym_get(final_state, 0, target_qubits[0] + initial_state->length / 2));
    sym_set(final_state, 0, target_qubits[1], sym_get(final_state, 0, target_qubits[1]) ^ sym_get(final_state, 0, target_qubits[0]));

    return gate_result_create_single(1, final_state);
}

gate_result* gate_hadamard(const sym* initial_state, const void* gate_data, const unsigned* target_qubit)
{
    sym* final_state = sym_copy(initial_state);
    
    BYTE tmp = sym_get(final_state, 0, target_qubit[0]);
    sym_set(final_state, 0, target_qubit[0], sym_get(final_state, 0, target_qubit[0] + final_state->length / 2));
    sym_set(final_state, 0, target_qubit[0] + final_state->length/2, tmp);
    return gate_result_create_single(1, final_state);;
}


gate_result* gate_phase(const sym* initial_state, const void* gate_data, const unsigned* target_qubit)
{
    sym* final_state = sym_copy(initial_state);
    
    sym_set(final_state, 0, target_qubit[0] + final_state->length / 2, sym_get(final_state, 0, target_qubit[0] + final_state->length / 2) ^ sym_get(final_state, 0, target_qubit[0]));
    return gate_result_create_single(1, final_state);
}

gate_result* gate_identity(const sym* initial_state, const void* gate_data, const unsigned* target_qubits)
{
	sym* final_state = sym_copy(initial_state);
	return gate_result_create_single(1, final_state);
}

#endif
