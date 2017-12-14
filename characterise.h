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

	double* p_error_probabilities = (double*)calloc(pow(4 , code->length / 2), sizeof(double));

	int i = 0;
	sym_iter* physical_error = sym_iter_create(code->length);	
	while (sym_iter_next(physical_error)) {
		// Calculate the probability of the error occurring
		double error_prob = error_model(physical_error->state, model_data);

		// What syndrome is caused by this error
		sym* syndrome = sym_syndrome(code, physical_error->state);

		// Use the decoder to determine the recovery operator
		sym* recovery_operator = decoder(syndrome, decoder_data);

		// 
		sym* corrected_physical_error = sym_add(physical_error->state, recovery_operator);

		p_error_probabilities[i] += error_prob;

		sym_free(syndrome);
		sym_free(recovery_operator);
		sym_free(corrected_physical_error);
		i++;
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
		fprintf(f, "%s %e\n", error_string, probabilities[i]);
		free(error_string);
		s += probabilities[i];
		i++;
	}
	sym_iter_free(physical_error);
	fclose(f);
	printf("Sum of probabilities: %e\n", s);
	return;
}

#endif
