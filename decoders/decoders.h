#ifndef DECODERS
#define DECODERS

#include <string.h>
#include <math.h>
#include "../sym.h"

#include "../destabilisers.h"

// Decoder template:
/*

sym* decoder(sym* error, void* v_m);
{
	Evaluate the error string
	If you need to view the error in a string format use the error_sym_to_str() function

	return correction_operation;
}
*/

typedef sym* (*decoder_call_f)(void* decoder, const sym* syndrome);

typedef void (*decoder_free_f)(void* decoder);

typedef struct {
	void* decoder_params;

	decoder_call_f call_decoder;
	decoder_free_f free_decoder;
} decoder;









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



//-------------------------------------------------------------------------------------------

sym* decoder_tailored(const sym* syndrome, void* decoder_data)
{
	sym** recovery_operators = *(sym***)decoder_data;
	sym* recovery_operator = sym_copy(*(recovery_operators + sym_to_ll(syndrome)));
	return recovery_operator;
}

/* 
    decoder_free:
	Frees all elements in a destabiliser
	:: sym** d :: The set of destabilisers to free
	:: const unsigned length ::  The number of destabilisers in the set
	Returns nothing
*/
void decoder_free(sym** d, const unsigned n_syndrome_bits)
{
	for (int i = 0; i < (1ull << (n_syndrome_bits)); i++)
	{
		if (d[i] != NULL)
		{
			sym_free(d[i]);	
		}
	}
	free(d);
	return;
}

#endif