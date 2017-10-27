#ifndef DECODERS
#define DECODERS

#include <string.h>
#include <math.h>
#include <stdio.h>
#include "sym.h"

// Decoder template:
/*

sym* decoder(sym* error, void* v_m);
{
	// Recast
	model_data* m = (model_data*)v_m;
	
	Evaluate the error string
	If you need to view the error in a string format use the error_sym_to_str() function

	return FLOAT
}

*/

sym* decoder_2_qubit_bit_flip(const sym* syndrome, void* decoder_data)
{
	//sym* recovery = sym_create(1, 4);
	sym* recovery_operator;

	switch (sym_get(syndrome, 0, 0))
	{
		case 0:
			// Do Nothing, no error detected
			recovery_operator = error_str_to_sym("II");
			return recovery_operator;
		break;

		case 1:
			// ZI error
			recovery_operator = error_str_to_sym("XI");
			return recovery_operator;
			//sym_set(recovery, 0, 2, (BYTE) 1);
		break;
	}
	//return recovery;
	return NULL;
}


typedef struct{
	unsigned n_qubits;
} null_decoder_data;

// Don't actually decode anything
sym* decoder_null(const sym* syndrome, void* decoder_data)
{
	sym* recovery_operator = sym_create(1, ((null_decoder_data*)decoder_data)->n_qubits * 2);
	return recovery_operator;
}

#endif