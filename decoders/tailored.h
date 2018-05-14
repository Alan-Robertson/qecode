#ifndef DECODER_TAILORED
#define DECODER_TAILORED

#include "decoders.h"

sym* decoder_call_tailored(void* v_decoder, const sym* syndrome)
{
	sym** recovery_operators = *(sym***)decoder_data;
	sym* recovery_operator = sym_copy(*(recovery_operators + sym_to_ll(syndrome)));
	return recovery_operator;
}

#endif