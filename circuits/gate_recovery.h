#ifndef GATE_OPERATIONS
#define GATE_OPERATIONS

#include "../sym.h"
#include "gate_operations.h"

typedef struct {
	sym* code;
	decoder* recovery_operations;
} gate_data_recovery_t;

// This should eventually be swapped out with reading the syndrome qubits
sym* gate_recovery(const sym* initial_state, void* gate_data, const unsigned* target_qubit)
{
    gate_data_recovery_t* recovery_data = (gate_data_recovery_t*)gate_data;

    // Call the decoder on the measurement output of the ancilla qubits
    sym* syndrome = sym_syndrome(recovery_data->code, initial_state);

    sym* recovery = decoder_call(recovery_data->decoder, syndrome);

    sym* final_state = sym_add(initial_state, recovery);
    
    sym_free(syndrome);
    sym_free(recovery);
    return final_state;
}


#endif