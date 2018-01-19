#ifndef DECODERS
#define DECODERS

#include <string.h>
#include <math.h>
#include <stdio.h>
#include "sym.h"

#include "destabilisers.h"

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

typedef sym* (*decoder_f)(const sym* syndrome, void* decoder_data);

//2 Qubit Bit Flip Decoder--------------------------------------------------------------------

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

//Null Decoder--------------------------------------------------------------------

typedef struct{
	unsigned n_qubits;
} null_decoder_data;

// Don't actually decode anything
sym* decoder_null(const sym* syndrome, void* decoder_data)
{
	sym* recovery_operator = sym_create(1, ((null_decoder_data*)decoder_data)->n_qubits * 2);
	return recovery_operator;
}

// Destabiliser Decoder -------------------------------------------------------------------------------------
typedef struct{
	sym** destabilisers;
} destabiliser_decoder_data;

sym* decoder_destabiliser(const sym* syndrome, void* decoder_data)
{
	destabiliser_decoder_data* d = (destabiliser_decoder_data*)decoder_data;
	
	sym* correction = sym_create(1, d->destabilisers[0]->length);

	for (int i = 0; i < syndrome->height; i++)
	{
		if (sym_get(syndrome, i, 0)) 
		{
			sym_add_in_place(correction, d->destabilisers[i]);
		}
	}
	return correction;
}

//-------------------------------------------------------------------------------------------

sym* decoder_tailored(const sym* syndrome, void* decoder_data)
{
	sym** recovery_operators = *(sym***)decoder_data;
	sym* recovery_operator = sym_copy(*(recovery_operators + sym_to_ll(syndrome)));
	return recovery_operator;
}

#endif