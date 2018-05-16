#ifndef DECODER_TAILORED
#define DECODER_TAILORED

#include "decoders.h"
#include "destabiliser.h"
#include "logical_destabiliser.h"

//----------------------------------------------------------------------------------------
// Function Declarations
//----------------------------------------------------------------------------------------

/* 
	tailor_recovery_operators:
	Finds the best possible tailored decoder for a given QECC and error model
	:: const sym* code :: The error correcting code
	:: const sym* logicals :: Logical operators
	:: double (*error_model)(const sym*, void*) :: The error model
	:: void* model_data :: The data associated with the error model
	Returns an array of recovery operators where the binary representation of the syndrome gives the 
	associated recovery
*/
sym** tailor_recovery_operators(const sym* code, 
				const sym* logicals, 
				error_model* noise);

/*
	decoder_create_tailored
	Base model constructor for a tailored decoder, allocates memory for the error model and sets the vtable up appropriately.
	:: sym** recovery_operators :: The set of recovery operators for the decoder
	:: const unsigned n_syndrome_bits :: The number of syndrome bits that will be communicated to the decoder
	Returns a pointer to a new error model object on the heap
*/
decoder* decoder_create_tailored(const sym* code, const sym* logicals, 	error_model* noise);

/*
	decoder_call_tailored
	Determines the correction procedure given a syndrome and a tailored decoder
	:: void* v_params:: The pointer to the params object 
	:: const sym* syndrome :: The  syndrome passed to the decoder
	Returns the correction suggested by the decoder
*/
sym* decoder_call_tailored(void* v_params, const sym* syndrome);

/*
	decoder_free_params_destabiliser
	Destructor a destabiliser decoder
	:: void* v_params :: The decoder params object to be freed
	Returns nothing
*/
void decoder_free_params_tailored(void* v_params);


//----------------------------------------------------------------------------------------
// The Tailored Decoder `Class'
//----------------------------------------------------------------------------------------

typedef struct {
	unsigned n_syndrome_bits;
	sym** recovery_operators;
} decoder_params_tailored_t;


//----------------------------------------------------------------------------------------
// Function Definitions
//----------------------------------------------------------------------------------------

/*
	decoder_create_tailored
	Base model constructor for a tailored decoder, allocates memory for the error model and sets the vtable up appropriately.
	:: sym** recovery_operators :: The set of recovery operators for the decoder
	:: const unsigned n_syndrome_bits :: The number of syndrome bits that will be communicated to the decoder
	Returns a pointer to a new error model object on the heap
*/
decoder* decoder_create_tailored(const sym* code, const sym* logicals, 	error_model* noise)
{
	decoder* d = decoder_create();
	decoder_params_tailored_t* dp = (decoder_params_tailored_t*)malloc(sizeof(decoder_params_tailored_t));

	dp->recovery_operators = tailor_recovery_operators(code, logicals, noise);
	dp->n_syndrome_bits = code->height;

	// Link the params to the decoder
	d->params = dp;

	// Setup the vtable
	d->call = decoder_call_tailored;
	d->param_free = decoder_free_params_tailored;
	return d;
}

/*
	decoder_call_tailored
	Determines the correction procedure given a syndrome and a tailored decoder
	:: void* v_params:: The pointer to the params object 
	:: const sym* syndrome :: The  syndrome passed to the decoder
	Returns the correction suggested by the decoder
*/
sym* decoder_call_tailored(void* v_params, const sym* syndrome)
{
	decoder_params_tailored_t* params = (decoder_params_tailored_t*)v_params;
	sym* recovery_operator = sym_copy(params->recovery_operators[sym_to_ll(syndrome)]);
	return recovery_operator;
}

/*
	decoder_free_params_destabiliser
	Destructor a destabiliser decoder
	:: void* v_params :: The decoder params object to be freed
	Returns nothing
*/
void decoder_free_params_tailored(void* v_params)
{
	decoder_params_tailored_t* params = (decoder_params_tailored_t*)v_params;

	for (unsigned long long i = 0; i < (1ull << (params->n_syndrome_bits)); i++)
	{
		
		if (NULL != params->recovery_operators[i])
		{
			sym_free(params->recovery_operators[i]);
		}
	}
	free(params->recovery_operators);
	free(params);
	return;
}

//----------------------------------------------------------------------------------------
// Tailoring the decoder
//----------------------------------------------------------------------------------------

/* 
	tailor_recovery_operators:
	Finds the best possible tailored decoder for a given QECC and error model
	:: const sym* code :: The error correcting code
	:: const sym* logicals :: Logical operators
	:: double (*error_model)(const sym*, void*) :: The error model
	:: void* model_data :: The data associated with the error model
	Returns an array of recovery operators where the binary representation of the syndrome gives the 
	associated recovery
*/
sym** tailor_recovery_operators(const sym* code, 
				const sym* logicals, 
				error_model* noise)
{
	// -------------------------------------
	// Initialisation
	// -------------------------------------

	// Some quality of life variables
	long long n_syndromes = (1ull << (code->height));
	long long n_logical_operations = (1ull << (logicals->length));

	// Build the decoder table, there should be a single decoding operation for each syndrome
	sym** tailored_decoder = (sym**)malloc(sizeof(sym*) * n_syndromes);

	// Initialise the recovery operators to prevent fragmentation
	sym_iter* syndromes = sym_iter_create(code->height);
	while (sym_iter_next(syndromes))
	{
		unsigned long long index = sym_to_ll(syndromes->state);
		tailored_decoder[index] = (sym*)sym_create(1, code->length);

		// Set the mem_size to 0 as a flag for undiscovered recovery operators
		//tailored_decoder[index]->mem_size = 0;
	}
	sym_iter_free(syndromes);

	// Initialise the probabilities and set them to 0 efficiently
	double p_options[n_syndromes][n_logical_operations];

	for (size_t i = 0; i < n_syndromes; i++)
	{
		memset(p_options[i], 0, sizeof(double) * n_logical_operations);
	}

	// Find the destabilisers
	decoder* destabilisers = decoder_create_destabiliser(code, logicals);


	// Store this for potential quality of life
	unsigned mem_size = ((decoder_params_destabiliser_t*)destabilisers->params)->destabilisers[0]->mem_size;

	// -----------------------------------
	// Determine the logical errors
	// By iterating through each possible physical error and by applying the destabilisers
	// determine the overall logical error produced by this correction procedure 
	// -----------------------------------

	// Iterate through errors and map back to the code-space
	sym_iter* physical_error = sym_iter_create(code->length);
	while (sym_iter_next(physical_error))
	{
		// Calculate the syndrome
		sym* syndrome = sym_syndrome(code, physical_error->state);
		
		// Get the recovery operator
		sym* recovery = decoder_call(destabilisers, syndrome);

		// If we haven't seen this recovery operator before, we save it
		if (!tailored_decoder[sym_to_ll(syndrome)]->mem_size)
		{
			// Reset the mem_size and perform this copy operation in place
			tailored_decoder[sym_to_ll(syndrome)]->mem_size = recovery->mem_size;
			sym_copy_in_place(tailored_decoder[sym_to_ll(syndrome)], recovery);
		}
		
		// Determine the state after correction
		sym* corrected = sym_add(recovery, physical_error->state);

		// Determine the overall logical state
		sym* logical_state = logical_error(logicals, corrected);

		// Calculate the probability of this particular error occurring and store it
		p_options[sym_to_ll(syndrome)][sym_to_ll(logical_state)] += error_model_call(noise, physical_error->state);

		// Free our memory in order to prevent leaks and fragmentation
		sym_free(logical_state);
		sym_free(corrected);
		sym_free(recovery);
		sym_free(syndrome);
	}
	sym_iter_free(physical_error);
	
	// ---------------------------------------------------
	// Calculate and store the optimal recovery operator
	// For each possible logical error associated with each syndrome, determine the best choice of logical correction
	// ---------------------------------------------------

	// Determine the anti-commutation relations between the logical operators
	decoder* logical_destabilisers = decoder_create_logical_destabiliser(logicals);

	// Setup the decoder	
	for (size_t i = 0; i < n_syndromes; i++)
	{
		// Set the default logical correction to none
		// This covers the case when that particular syndrome is never encountered
		double p_correction = p_options[i][0];
		unsigned r_operator = 0;
				
		// This syndrome was never encountered, we can skip searching the rest and just
		// correct the mem_size, p == 0
		if (!tailored_decoder[i]->mem_size) 
		{
			tailored_decoder[i]->mem_size = mem_size;
		}
		else // Syndrome was encountered, determine the optimal logical correction
		{
			for (size_t j = 1; j < n_logical_operations; j++)
			{
				// If the probability of this syndrome is greater than the current 
				// best found then change our optimal correction
				if (p_options[i][j] > p_correction)
				{
					p_correction = p_options[i][j];
					r_operator = j;
				}
			}
		}

		// Find the logical syndrome
		sym* logical_syndrome = ll_to_sym_t(r_operator, 1, logicals->length);

		// Using our logical syndromes and our knowledge of the best recovery
		sym* logical_recovery = decoder_call(logical_destabilisers, logical_syndrome);

		sym_add_in_place(tailored_decoder[i], logical_recovery);
	
		sym_free(logical_recovery);
		sym_free(logical_syndrome);
	}

	// ------------------------------------------
	// Cleanup
	// ------------------------------------------
	decoder_free(destabilisers);
	decoder_free(logical_destabilisers);

	return tailored_decoder;
}


#endif