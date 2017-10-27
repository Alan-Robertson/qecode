#ifndef ADAPTIVE_DECODER
#define ADAPTIVE_DECODER

#include "sym.h"
#include "errors.h"
#include "decoders.h"
#include <math.h>


typedef struct {
	unsigned n_qubits;
	sym* recovery_operators;
} adapted_decoder_data;

sym* decoder_adapted(const sym* syndrome, void* decoder_data)
{
	char* str_syndrome = error_sym_to_str(syndrome);
	sym* recovery_operator = sym_create(1, n_qubits);
	return ((adapted_decoder_data*)decoder_data)->recovery_operators[error_str_to_int(str_syndrome, syndrome->length/2)];
}

adapted_decoder_data* decoder_characterise(
	const sym* code, 
	const sym* logicals, 
	double (*error_model)(const sym*, void*), 
	void* model_data
{
	adapted_decoder_data* decoder = (adapted_decoder_data*)malloc(sizeof(adapted_decoder_data))
	decoder->recovery_operators = sym_create(pow(2, code->height), code->length);

	sym* destabilisers = sym_binary_symplectic_elimination(code, logicals);

	double* p_error_probabilities = (double*)calloc(pow(, code->length / 2), sizeof(double));

	char physical_error_string[code->length/2 + 1];
	for (int i = 0; i < code->length / 2; i++)
	{
		physical_error_string[i] = 'I'; // Initialise error string to the identity
	}
	physical_error_string[code->length / 2] = '\0'; // Null terminator
	
	do
	{
		sym* physical_error = error_str_to_sym(physical_error_string);

		double error_prob = error_model(physical_error, model_data);

		sym* syndrome = sym_syndrome(code, physical_error);

		sym* recovery_operator = decoder(syndrome, decoder_data);

		sym* corrected_physical_error = sym_add(physical_error, recovery_operator);

		char* corrected_string = error_sym_to_str(corrected_physical_error);

		p_error_probabilities[error_str_to_int(corrected_string, logicals->height / 2)] += error_prob;

		sym_free(physical_error);
		sym_free(syndrome);
		sym_free(recovery_operator);
		sym_free(corrected_physical_error);
		free(corrected_string);

	} while(error_inc(physical_error_string, code->length / 2));

	return p_error_probabilities;
}

#endif