#ifndef GATE_RECOVERY
#define GATE_RECOVERY

#include "../sym.h"
#include "../decoders/decoders.h"
#include "gate_result.h"

typedef struct {
    uint32_t n_syndrome_bits;
	decoder* recovery_operations;
} gate_data_recovery_t;

// This should eventually be swapped out with reading the syndrome qubits
sym* gate_recovery(const sym* initial_state, void* gate_data, const unsigned* target_qubits)
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