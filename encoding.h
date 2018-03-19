#ifndef ENCODING
#define ENCODING

#include "circuit.h"
#include "codeword.h"


circuit* encoding_circuit(
	const sym* code, 
	const sym** destabilisers
	)
{
	sym* tableu = sym_create(code->height * 2; code->length);

	// Create the tableau from 10.1103/PhysRevA.70.052328
	for (size_t i = 0; i < code->height * 2; i++)
	{
		if (i / code->height)
		{
			sym_row_copy(tableu, destabilisers[i], i, 0);	
		}
		else
		{
			sym_row_copy(tableu, code, i, i % code->height);
		}
	}


	// Get a codeword
	sym* codeword = codeword_find(code, logicals);




	// Find an encoding from the all |0>^n state to the codeword
	for (size_t i = 0; i < codeword->length/2; i++)
	{


	}


	// Verify the encoding worked correctly
	for (size_t i = 0; i < codeword->length/2; i++)
	{


	}

}



#endif