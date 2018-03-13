#ifndef ENCODING
#define ENCODING

#include "circuit.h"
#include "codeword.h"


// This may not scale for more than 1 logical qubit, need to investigate
circuit* encoding_circuit(
	const sym* code, 
	const sym* logicals, 
	const gate* cnot, 
	const gate* hadamard)
{
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