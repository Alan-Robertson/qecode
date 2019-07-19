#ifndef CHARACTERISATION
#define CHARACTERISATION


#include <math.h>

#include "sym.h"
#include "sym_iter.h"
#include "error_models/error_models.h"
#include "decoders/decoders.h"
#include "circuits/error_probabilities.h"
#include "errors.h"


/* 
	characterise_code:
	Given an error model, calculates the physical and logical krauss operators for a given code
	:: const sym* code :: A sym* object containing the stabiliser code
	:: const sym* logicals :: A sym* object containing the logical operators
	:: double (*error_model)(char*, void*) :: A function pointer to an error model that takes model data as the second argument 
	:: void* model_data :: Any model data that needs to be passed to the error model
	:: 
	Returns a double*, the first element of which is a pointer to the probabilities associated with each of the krauss operators
	on the physical error channel
*/
double* characterise_code(const sym* code, 
						const sym* logicals, 
						error_model* noise_model, 
						decoder* decoding_operation)
{
	// Setup our array of logical error probabilities
	double* p_error_probabilities = error_probabilities_m(logicals->length);
	
	// Iterate through errors and map back to the code-space
	#ifdef CHARACTERISE_MAX_DEPTH
		sym_iter* physical_error = sym_iter_create_range(code->length, 0, 2 * CHARACTERISE_MAX_DEPTH + 1);
	#else
		sym_iter* physical_error = sym_iter_create(code->length);
	#endif
	while (sym_iter_next(physical_error))
	{
		// Calculate the syndrome
		sym* syndrome = sym_syndrome(code, physical_error->state);
			
		// Get the recovery operator
		sym* recovery = decoder_call(decoding_operation, syndrome);
		
		// Determine the state after correction
		sym* corrected = sym_add(recovery, physical_error->state);

		// Determine the overall logical state
		sym* logical_state = logical_error(logicals, corrected);

		// Store the probability
		p_error_probabilities[sym_to_ll(logical_state)] += error_model_call(noise_model, physical_error->state);
		
		// Free our memory
		sym_free(logical_state);
		sym_free(corrected);
		sym_free(recovery);
		sym_free(syndrome);
	}
	sym_iter_free(physical_error);

	return p_error_probabilities;
}


double* characterise_code_corrected(const sym* code, 
						const sym* logicals, 
						double* error_rates)
{
	// Setup our array of logical error probabilities
	double* p_error_probabilities = error_probabilities_m(logicals->length);

	// Iterate through errors and map back to the code-space
	#ifdef CHARACTERISE_MAX_DEPTH
		sym_iter* physical_error = sym_iter_create_range(code->length, 0, 2 * CHARACTERISE_MAX_DEPTH + 1);
	#else
		sym_iter* physical_error = sym_iter_create_n_qubits(code->n_qubits);
	#endif
	while (sym_iter_next(physical_error))
	{
		if (error_rates[sym_iter_ll_from_state(physical_error)] > 0)
		{
			// Calculate the syndrome
			sym* syndrome = sym_syndrome(code, physical_error->state);
			if (sym_is_empty(syndrome)) // Syndrome is 0, we are in the code space
			{
				// Determine the overall logical state
				sym* logical_state = logical_error(logicals, physical_error->state);

				// Store the probability
				p_error_probabilities[sym_to_ll(logical_state)] += error_rates[sym_to_ll(physical_error->state)];

				// Free our memory
				sym_free(logical_state);
			}
			sym_free(syndrome);
		}		
	}
	sym_iter_free(physical_error);

	return p_error_probabilities;
}

void characterise_save(const double* probabilities, const size_t n_qubits, const char* filename)
{	
	FILE *f;
	f = fopen(filename, "w");
	if (f == NULL) 
	{
		printf("Error when opening file.\n");
		return;
	}
	double s = 0;
	#ifdef CHARACTERISE_MAX_DEPTH
		sym_iter* physical_error = sym_iter_create_n_qubits_range(n_qubits, 0, CHARACTERISE_MAX_DEPTH);
	#else
		sym_iter* physical_error = sym_iter_create_n_qubits(n_qubits);
	#endif	
	while (sym_iter_next(physical_error))
	{
		char* error_string = error_sym_to_str(physical_error->state);
		fprintf(f, "%s %e\n", error_string, probabilities[sym_to_ll(physical_error->state)]);
		free(error_string);
		s += probabilities[sym_to_ll(physical_error->state)];
	}
	sym_iter_free(physical_error);
	fclose(f);
	return;
}

void characterise_print(const double* probabilities, const size_t n_qubits)
{	
	double s = 0;
	#ifdef CHARACTERISE_MAX_DEPTH
		sym_iter* physical_error = sym_iter_create_n_qubits_range(n_qubits, 0, CHARACTERISE_MAX_DEPTH);
	#else
		sym_iter* physical_error = sym_iter_create_n_qubits(n_qubits);
	#endif	
	while (sym_iter_next(physical_error))
	{
		char* error_string = error_sym_to_str(physical_error->state);
		printf("%s %e\n", error_string, probabilities[sym_to_ll(physical_error->state)]);
		free(error_string);
		s += probabilities[sym_to_ll(physical_error->state)];
	}
	sym_iter_free(physical_error);
	//printf("Sum of probabilities: %e\n", s);
	return;
}

double characterise_test(const double* probabilities, const size_t n_qubits)
{	
	double total = 0;
	#ifdef CHARACTERISE_MAX_DEPTH
		sym_iter* physical_error = sym_iter_create_n_qubits_range(n_qubits, 0, CHARACTERISE_MAX_DEPTH);
	#else
		sym_iter* physical_error = sym_iter_create_n_qubits(n_qubits);
	#endif	
	while (sym_iter_next(physical_error))
	{
		total += probabilities[sym_to_ll(physical_error->state)];
	}
	sym_iter_free(physical_error);
	return total;
}

#endif
