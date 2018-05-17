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

// FUNCTION DEFINITIONS ----------------------------------------------------------------------------------------
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
//----------------------------------------------------------------------------------------
// Tailoring the decoder
//----------------------------------------------------------------------------------------

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
/*sym** tailor_recovery_operators(const sym* code, 
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
}*/

//----------------------------------------------------------------------------------------
// Tailoring the decoder
//----------------------------------------------------------------------------------------

/* 
	tailor_prob:
	Calculates the probability of recovery using a tailored decoder
	:: const sym* code :: The error correcting code
	:: const sym* logicals :: Logical operators
	:: double (*error_model)(const sym*, void*) :: The error model
	:: void* model_data :: The data associated with the error model
	Returns the probability of a logical recovery
*/
double tailored_prob(const sym* code, 
				const sym* logicals, 
				error_model* noise)
{
	// -------------------------------------
	// Initialisation
	// -------------------------------------
	double p_recovery = 0.0;

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

		sym* recovery_plus_error = sym_add(recovery, physical_error->state);

		sym* new_syndrome = sym_syndrome(code, recovery_plus_error);

		sym_free(recovery_plus_error);
		sym_free(new_syndrome);

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
		p_recovery += p_correction;
	}
	
	// ------------------------------------------
	// Cleanup
	// ------------------------------------------
	decoder_free(destabilisers);

	for (size_t i = 0; i < n_syndromes; i++)
	{
		sym_free(tailored_decoder[i]);
	}
	free(tailored_decoder);

	return p_recovery;
}

#endif