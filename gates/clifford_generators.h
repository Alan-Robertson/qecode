#ifndef GATE_CLIFFORD_GENERATORS
#define GATE_CLIFFORD_GENERATORS


// GATE OBJECTS ----------------------------------------------------------------------------------------

typedef struct {} gate_data_cnot_t;

// 0 is control, 1 is target
sym* gate_cnot(const sym* initial_state, void* gate_data, const unsigned* target_qubits)
{
    sym* final_state = sym_copy(initial_state);

    sym_set(final_state, 0, target_qubits[1] + initial_state->length / 2, sym_get(final_state, 0, target_qubits[1] + initial_state->length / 2) ^ sym_get(final_state, 0, target_qubits[0] + initial_state->length / 2));
    sym_set(final_state, 0, target_qubits[1], sym_get(final_state, 0, target_qubits[1]) ^ sym_get(final_state, 0, target_qubits[0]));
    return final_state;
}


typedef struct {} gate_data_hadamard_t;

sym* gate_hadamard(const sym* initial_state, void* gate_data, const unsigned* target_qubit)
{
    sym* final_state = sym_copy(initial_state);
    
    BYTE tmp = sym_get(final_state, 0, target_qubit[0]);
    sym_set(final_state, 0, target_qubit[0], sym_get(final_state, 0, target_qubit[0] + final_state->length / 2));
    sym_set(final_state, 0, target_qubit[0] + final_state->length/2, tmp);
    return final_state;
}

typedef struct {} gate_data_phase_t;

sym* gate_phase(const sym* initial_state, void* gate_data, const unsigned* target_qubit)
{
    sym* final_state = sym_copy(initial_state);
    
    sym_set(final_state, 0, target_qubit[0] + final_state->length / 2, sym_get(final_state, 0, target_qubit[0] + final_state->length / 2) ^ sym_get(final_state, 0, target_qubit[0]));
    return final_state;
}


typedef struct {} gate_data_identity_t;

sym* gate_identity(const sym* initial_state, void* gate_data, const unsigned* target_qubits)
{
	sym* final_state = sym_copy(initial_state);
	return final_state;
}

#endif
