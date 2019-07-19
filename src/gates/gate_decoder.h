#ifndef GATE_DECODER
#define GATE_DECODER

#include "../sym.h"
#include "../decoders/decoders.h"
#include "gate_result.h"

typedef struct {
    uint32_t n_syndrome_bits;
	decoder* decoder_operations;
} gate_data_decoder_t;

// Here the initial state is the output of the measurement data and should only contain the ancilla bits
sym* gate_decoder(const sym* initial_state, void* gate_data, const unsigned* target_qubits)
{
    gate_data_decoder_t* decoder_gate_data = (gate_data_decoder_t*)gate_data;

    // Calling the decoder on the syndrome bits
    sym* recovery_operation = decoder_call(recovery_data->decoder, syndrome);
    
    return recovery_operation;
}

#endif