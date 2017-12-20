#ifndef TAILORED_PAR
#define TAILORED_PAR
#include "sym.h"
#include "sym_iter.h"
#include "decoders.h"
#include "logical.h"
#include "pthread.h"


// WORK IN PROGRESS!!!

// Flyweight pattern for passing single instances to each thread without copying all the pointers every time
typedef struct {
	const sym* code; 
	const sym* logicals;
	sym*** tailored_decoder;
	sym*** destabilisers;
	double* p_options;
	const long long unsigned* n_syndromes;
	const long long unsigned* n_logical_operations;
	double (*error_model)(const sym*, void*);
	void* model_data;
	sym*** logical_destabilisers;
} decoder_flyweight;

// Struct telling each thread what to operate over
typedef struct {
	unsigned long long start;
	unsigned long long end;
	decoder_flyweight* flyweight;
} decoder_logical_struct;

void *decoder_logical_errors(void* data)
{
	// Recast from void* to decoder_logical_struct
	decoder_logical_struct* pthread_data = (decoder_logical_struct*)data;

	sym_iter* physical_error = sym_iter_create(pthread_data->flyweight->code->length);
	sym_iter_state_from_ll(physical_error, pthread_data->start);

	while (sym_iter_next(physical_error) && sym_iter_ll_from_state(physical_error) <= pthread_data->end)
	{
		// Calculate the syndrome
		sym* syndrome = sym_syndrome(pthread_data->flyweight->code, physical_error->state);
		
		// Get the recovery operator
		sym* recovery = decoder_destabiliser(syndrome, *(pthread_data->flyweight->destabilisers));

		// If we haven't seen this recovery operator before, we save it
		// Using atomics for this prevents multiple threads from trying to write to the same object
		if (recovery->mem_size == __atomic_fetch_add(&((*(pthread_data->flyweight->tailored_decoder))[sym_to_ll(syndrome)]->mem_size), recovery->mem_size, __ATOMIC_SEQ_CST))
		{
			// Reset the mem_size and perform this copy operation in place
			// This prevents fragmentation of heap memory
			sym_copy_in_place((*(pthread_data->flyweight->tailored_decoder))[sym_to_ll(syndrome)], recovery);
		} else {
			// Reset the fetch operation, this ensures that if the element had already been incremented that it now has the correct memory size
			__atomic_fetch_sub(&((*(pthread_data->flyweight->tailored_decoder))[sym_to_ll(syndrome)]->mem_size), recovery->mem_size, __ATOMIC_SEQ_CST);
		}
		
		// Determine the state after correction
		sym* corrected = sym_add(recovery, physical_error->state);

		// Determine the overall logical state
		sym* logical_state = logical_error(corrected, pthread_data->flyweight->logicals);

		// Calculate the probability of this particular error occurring and store it
		__atomic_fetch_add(pthread_data->flyweight->p_options + (sym_to_ll(syndrome) * *(pthread_data->flyweight->n_syndromes) + sym_to_ll(logical_state)), pthread_data->flyweight->error_model(physical_error->state, pthread_data->flyweight->model_data) , __ATOMIC_SEQ_CST);

		// Free our memory in order to prevent leaks and fragmentation
		sym_free(logical_state);
		sym_free(corrected);
		sym_free(recovery);
		sym_free(syndrome);
	}
	sym_iter_free(physical_error);
	return NULL;
}

sym** decoder_tailor_pthread(const sym* code, 
				const sym* logicals, 
				double (*error_model)(const sym*, void*), 
				void* model_data,
				unsigned n_threads)
{
	// -------------------------------------
	// Initialisation
	// -------------------------------------

	// Some quality of life variables
	const long long unsigned n_syndromes = (1 << (code->height));
	const long long unsigned n_logical_operations = (1 << (logicals->length));

	// Build the decoder table, there should be a single decoding operation for each syndrome
	sym** tailored_decoder = (sym**)malloc(sizeof(sym*) * n_syndromes);

	// Initialise the recovery operators to prevent fragmentation
	sym_iter* syndromes = sym_iter_create(code->height);
	while(sym_iter_next(syndromes))
	{
		unsigned long long index = sym_to_ll(syndromes->state);
		tailored_decoder[index] = (sym*)sym_create(1, code->length);

		// Set the mem_size to 0 as a flag for undiscovered recovery operators
		tailored_decoder[index]->mem_size = 0;
	}
	sym_iter_free(syndromes);
	// Initialise the probabilities and set them to 0 efficiently
	double p_options[n_syndromes][n_logical_operations];
	memset(&p_options, 0, sizeof(double) * n_syndromes * n_logical_operations);
	
	// Find the destabilisers
	sym** destabilisers = destabilisers_generate(code, logicals);

	// Store this for potential quality of life
	unsigned mem_size = destabilisers[0]->mem_size;

	// -----------------------------------
	// Determine the logical errors
	// By iterating through each possible physical error and by applying the destabilisers
	// determine the overall logical error produced by this correction procedure 
	// -----------------------------------

	// Iterate through errors and map back to the code-space
	
	pthread_t* pthreads = (pthread_t*)malloc(sizeof(pthread_t) * n_threads);
	decoder_logical_struct* s = (decoder_logical_struct*)malloc(sizeof(decoder_logical_struct) * n_threads);
	unsigned long long e_per_thread = (1 << code->length) / n_threads;

	decoder_flyweight f = {code, logicals, &tailored_decoder, &destabilisers, &(p_options[0][0]), &n_syndromes, &n_logical_operations, error_model, model_data, NULL};

	for (int i = 0; i < n_threads; i++)
	{		
		s[i].start = i * e_per_thread;
		s[i].end = ((i + 1 < n_threads) ? (i + 1) * e_per_thread : 1 << code->length); 
		s[i].flyweight = &f;
		pthread_create(pthreads + i, NULL, decoder_logical_errors, (void*)(s + i));
	}

	for (int i = 0; i < n_threads; i++)
	{
		pthread_join(pthreads[i], NULL);
	}
	free(s);
	free(pthreads);

	// ---------------------------------------------------
	// Calculate and store the optimal recovery operator
	// For each possible logical error associated with each syndrome, determine the best choice of logical correction
	// ---------------------------------------------------

	sym** logical_destabilisers = logical_as_destabilisers(logicals);
	f.logical_destabilisers = &logical_destabilisers;

	// Setup the decoder
	// Assuming n_syndromes is sufficiently small that there's no real advantage to parallelizing it.	
	for (size_t i = 0; i < n_syndromes; i++) 
	{
		// Set the default logical correction to none
		// This covers the case when that particular syndrome is never encountered
		double p_correction = p_options[i][0];
		unsigned r_operator = 0;

		// This syndrome was never encountered, we can skip searching the rest and just
		// correct the mem_size
		if (!tailored_decoder[i]->mem_size) 
		{
			tailored_decoder[i]->mem_size = mem_size;
		}
		else // Syndrome was encountered, determine the optimal logical correction
		{
			for (size_t j = 1; j < n_logical_operations; j++)
			{
				if (p_options[i][j] > p_correction)
				{
					p_correction = p_options[i][j];
					r_operator = j;
				}
			}
		}
		// Find the logical syndrome
		sym* logical_syndrome = ll_to_sym(r_operator, 1, code->height);

		// Using our logical syndromes and our knowledge of the best 
		sym* logical_recovery = decoder_destabiliser(logical_syndrome, (void*)&logical_destabilisers);
		sym_add_in_place(tailored_decoder[i], logical_recovery);

		sym_free(logical_recovery);
		sym_free(logical_syndrome);
	}

	// ------------------------------------------
	// Cleanup
	// ------------------------------------------

	destabilisers_free(logical_destabilisers, logicals->length);
	destabilisers_free(destabilisers, code->height);

	return tailored_decoder;
}
#endif