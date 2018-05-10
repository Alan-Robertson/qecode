#ifndef GATE_OPERATIONS
#define GATE_OPERATIONS

#include "sym.h"


// STRUCT OBJECTS ----------------------------------------------------------------------------------------

//  Gate operation function pointer
typedef sym* (*gate_operation_f)(const sym*, void*, const unsigned* target_qubits);

/*
	gate:
	The gate struct
	:: unsigned n_qubits :: Number of qubits in the gate
	:: error_model_f error_model :: The error model to be applied when this gate is used
	:: void* model_data :: Data for the error model
*/

typedef struct {
	unsigned n_qubits; // Number of qubits the gate operates on
	gate_operation_f operation; // The gate operation performed (any absolute operation)
	error_model_t* error_model; // The error operation performed (Any probabilistic operation)
	void* operation_data; // Any additional data to pass to the gate
	void* error_model_data; // Any additional data to pass to the noise
} gate;

// GATE  OBJECTS ----------------------------------------------------------------------------------------

typedef struct {} gate_data_cnot;

// 0 is control, 1 is target
sym* gate_cnot(const sym* initial_state, void* gate_data, const unsigned* target_qubits)
{
    sym* final_state = sym_copy(initial_state);

    sym_set(final_state, 0, target_qubits[1] + initial_state->length / 2, sym_get(final_state, 0, target_qubits[1] + initial_state->length / 2) ^ sym_get(final_state, 0, target_qubits[0] + initial_state->length / 2));
    sym_set(final_state, 0, target_qubits[1], sym_get(final_state, 0, target_qubits[1]) ^ sym_get(final_state, 0, target_qubits[0]));
    return final_state;
}


typedef struct {} gate_data_hadamard;

sym* gate_hadamard(const sym* initial_state, void* gate_data, const unsigned* target_qubit)
{
    sym* final_state = sym_copy(initial_state);
    
    BYTE tmp = sym_get(final_state, 0, target_qubit[0]);
    sym_set(final_state, 0, target_qubit[0], sym_get(final_state, 0, target_qubit[0] + final_state->length / 2));
    sym_set(final_state, 0, target_qubit[0] + final_state->length/2, tmp);
    return final_state;
}

typedef struct {} gate_data_phase;

sym* gate_phase(const sym* initial_state, void* gate_data, const unsigned* target_qubit)
{
    sym* final_state = sym_copy(initial_state);
    
    sym_set(final_state, 0, target_qubit[0] + final_state->length / 2, sym_get(final_state, 0, target_qubit[0] + final_state->length / 2) ^ sym_get(final_state, 0, target_qubit[0]));
    return final_state;
}

sym* gate_identity(const sym* initial_state, void* gate_data, const unsigned* target_qubits)
{
	sym* final_state = sym_copy(initial_state);
	return final_state;
}

#endif
