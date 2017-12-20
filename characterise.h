#ifndef CHARACTERISATION
#define CHARACTERISATION

#include "sym.h"
#include "sym_iter.h"
#include "errors.h"
#include <math.h>

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
						double (*error_model)(const sym*, void*), 
						void* model_data, 
						sym* (*decoder)(const sym*, void*),
						void* decoder_data)
{
	double* p_error_probabilities = (double*)calloc(1ull << (logicals->length), sizeof(double));



	// Iterate through errors and map back to the code-space
	sym_iter* physical_error = sym_iter_create(code->length);
	while (sym_iter_next(physical_error))
	{
		// Calculate the syndrome
		sym* syndrome = sym_syndrome(code, physical_error->state);
		
		// Get the recovery operator
		sym* recovery = decoder(syndrome, decoder_data);

		// Determine the state after correction
		sym* corrected = sym_add(recovery, physical_error->state);

		// Determine the overall logical state
		sym* logical_state = logical_error(logicals, corrected);

		// Store the probability
		p_error_probabilities[sym_to_ll(logical_state)] += error_model(physical_error->state, model_data);

		// Free our memory
		sym_free(logical_state);
		sym_free(corrected);
		sym_free(recovery);
		sym_free(syndrome);
	}
	sym_iter_free(physical_error);

	return p_error_probabilities;
}

void characterise_save(const double* probabilities, const size_t length, const char* filename)
{	
	FILE *f;
	f = fopen(filename, "w");
	if (f == NULL) 
	{
		printf("Error when opening file.\n");
		return;
	}
	double s = 0;
	int i = 0;
	sym_iter* physical_error = sym_iter_create(length);	
	while (sym_iter_next(physical_error))
	{
		char* error_string = error_sym_to_str(physical_error->state);
		fprintf(f, "%s %f\n", error_string, probabilities[i]);
		free(error_string);
		s += probabilities[i];
		i++;
	}
	sym_iter_free(physical_error);
	fclose(f);
	printf("Sum of probabilities: %e\n", s);
	return;
}

void characterise_print(const double* probabilities, const size_t length)
{	
	double s = 0;
	sym_iter* physical_error = sym_iter_create(length);	
	while (sym_iter_next(physical_error))
	{
		char* error_string = error_sym_to_str(physical_error->state);
		printf("%s %e\n", error_string, probabilities[sym_to_ll(physical_error->state)]);
		free(error_string);
		s += probabilities[sym_to_ll(physical_error->state)];
	}
	sym_iter_free(physical_error);
	printf("Sum of probabilities: %e\n", s);
	return;
}

#endif
