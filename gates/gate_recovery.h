#ifndef GATE_RECOVERY
#define GATE_RECOVERY

#include "../sym.h"
#include "../decoders/decoders.h"
#include "gate_result.h"

typedef struct {
    uint32_t n_syndrome_bits;
	decoder* recovery_operations;
} gate_data_recovery_t;

// Here the initial state is the output of the measurement data and should only contain the ancilla bits
sym* gate_recovery(const sym* initial_state, void* gate_data, const unsigned* target_qubits)
{
    gate_data_recovery_t* recovery_data = (gate_data_recovery_t*)gate_data;

    // Calling the decoder on the syndrome bits
    sym* recovery = decoder_call(recovery_data->decoder, syndrome);
    
    return recovery;
}

#endif