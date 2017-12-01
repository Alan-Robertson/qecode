#ifndef CHARACTERISATION
#define CHARACTERISATION

#include "sym.h"
#include "sym_iter.h"
#include "errors.h"
#include <math.h>


/* 
	sym_characterise:
	Given an error model, calculates the physical and logical krauss operators for a given code
	:: const sym* code :: A sym* object containing the stabiliser code
	:: const sym* logicals :: A sym* object containing the logical operators
	:: double (*error_model)(char*, void*) :: A function pointer to an error model that takes model data as the second argument 
	:: void* model_data :: Any model data that needs to be passed to the error model
	:: 
	Returns a double**, the first element of which is a pointer to the probabilities associated with each of the krauss operators
	on the physical error channel, the second element the krauss operators for the logical error channel.
*/
double* characterise_code(const sym* code, 
						const sym* logicals, 
						double (*error_model)(const sym*, void*), 
						void* model_data, 
						sym* (*decoder)(const sym*, void*),
						void* decoder_data)
{

	double* p_error_probabilities = (double*)calloc(pow(4 , code->length / 2), sizeof(double));

	sym_iter* physical_error = sym_iter_create(code->length);	
	do
	{
		// Calculate the probability of the error occurring
		double error_prob = error_model(physical_error->state, model_data);

		// What syndrome is caused by this error
		sym* syndrome = sym_syndrome(code, physical_error->state);

		// Use the decoder to determine the recovery operator
		sym* recovery_operator = decoder(syndrome, decoder_data);

		// 
		sym* corrected_physical_error = sym_add(physical_error->state, recovery_operator);

		char* corrected_string = error_sym_to_str(corrected_physical_error);
		p_error_probabilities[error_str_to_int(corrected_string, logicals->height / 2)] += error_prob;

		sym_free(syndrome);
		sym_free(recovery_operator);
		sym_free(corrected_physical_error);
		free(corrected_string);

	} while(sym_iter_next(physical_error));

	return p_error_probabilities;
}

double characterise_correct_error(const sym* code, const sym* logicals, // Stabilisers and logical operators
						double (*error_model)(const sym*, void*), void* model_data, // Error model and model data
						sym* (*decoder)(const sym*, void*), void* decoder_data, // Decoder and decoder data
						sym* physical_error) // The error to apply
{
	double error_prob = error_model(physical_error, model_data);

	sym* syndrome = sym_syndrome(code, physical_error);

	sym* recovery_operator = decoder(syndrome, decoder_data);

	sym* corrected_physical_error = sym_add(physical_error, recovery_operator);

	if (!sym_weight(corrected_physical_error))
	{
		return 0; // Error corrected
	}

	return error_prob;
}


void characterise_save(const double* probabilities, const size_t len, const char* filename)
{
	char error_string[len + 1];
	for (int i = 0; i < len; i++)
	{
		error_string[i] = 'I'; // Initialise error string to the identity, this will produce no logical error
	}
	error_string[len] = '\0';

	FILE *f;
	f = fopen(filename, "w");
	if (f == NULL) 
	{
		printf("Error when opening file.\n");
	}

	do
	{
		fprintf(f, "%s %f\n", error_string, probabilities[error_str_to_int(error_string, len)]);
	} while(error_inc(error_string, len));
	fclose(f);
	return;
}



// Deprecated
void characterise_free(double** characterisation)
{
	free(characterisation[0]);
	free(characterisation[1]);
	free(characterisation);
}

#endif
