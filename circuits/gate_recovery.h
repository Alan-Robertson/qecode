#ifndef GATE_OPERATIONS
#define GATE_OPERATIONS

#include "../sym.h"
#include "gate_operations.h"

typedef struct {
	sym* code;
	decoder* ;
} gate_data_recovery_t;

sym* gate_recovery(const sym* initial_state, void* gate_data, const unsigned* target_qubit)
{
    sym* final_state = sym_copy(initial_state);

    // Call the decoder on the measurement output of the ancilla qubits 
    // Should really develop a new MC gate for this.
    decoder_call();


    BYTE tmp = sym_get(final_state, 0, target_qubit[0]);
    sym_set(final_state, 0, target_qubit[0], sym_get(final_state, 0, target_qubit[0] + final_state->length / 2));
    sym_set(final_state, 0, target_qubit[0] + final_state->length/2, tmp);
    return final_state;
}


#endif