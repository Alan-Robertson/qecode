#ifndef TAILORED
#define TAILORED
#include "sym.h"
#include "sym_iter.h"
#include "decoders/decoders.h"
#include "decoders/destabiliser.h"
#include "logical.h"
#include "error_models/error_models.h"


// FUNCTION DECLARATIONS ----------------------------------------------------------------------------------------

/* 
	tailor_decoder:
	Finds the best possible tailored decoder for a given QECC and error model
	:: const sym* code :: The error correcting code
	:: const sym* logicals :: Logical operators
	:: double (*error_model)(const sym*, void*) :: The error model
	:: void* model_data :: The data associated with the error model
	Returns an array of recovery operators where the binary representation of the syndrome gives the 
	associated recovery
*/
/*sym** tailor_decoder(const sym* code, 
				const sym* logicals, 
				error_model* error_model);*/

/* 
	tailor_decoder_prob_only:
	Returns just the probability associated with the best possible decoding of a given QECC and error model
	:: const sym* code :: The error correcting code
	:: const sym* logicals :: Logical operators
	:: double (*error_model)(const sym*, void*) :: The error model
	:: void* model_data :: The data associated with the error model
	Returns a double containing the probability
*/
double tailored_prob(const sym* code, 
				const sym* logicals, 
				error_model* error_model);

#endif