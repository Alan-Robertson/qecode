#ifndef DECODER_NO_ACTION
#define DECODER_NO_ACTION

#include "decoders.h"

//Null Decoder--------------------------------------------------------------------

typedef struct{
	unsigned n_qubits;
} null_decoder_data;

// Don't actually decode anything
sym* decoder_call_no_action(void* v_decoder, const sym* syndrome)
{
	sym* recovery_operator = sym_create(1, ((null_decoder_data*)decoder_data)->n_qubits * 2);
	return recovery_operator;
}


#endif