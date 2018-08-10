
#include "sym.h"
#include "codes/codes.h"
#include "decoders/tailored.h"
#include "error_models/iid.h"
#include "characterise.h"

int main()
{	
	double rate_min = 0.00000000001, rate_delta = 2;
	unsigned n_increments = 35;

	double logical_rate[35];

	unsigned n_qubits = 7, n_logicals = 1, distance = 3;
	unsigned n_codes_searched = 100000;

	sym* code = code_steane();
	sym* logicals = code_steane_logicals();

	for (unsigned i = 0; i < n_increments; i++)
	{
		// Set the error rate
		double error_rate = rate_min * pow(rate_delta, i); 
		
		// Setup the error model
		error_model* noise_model = error_model_create_iid(n_qubits, error_rate);

		// Tailor the Decoder
		decoder* tailored_decoder = decoder_create_tailored(code, logicals, noise_model);
		
		double* probabilities = characterise_code(code, logicals, noise_model, tailored_decoder);
		logical_rate[i] = probabilities[0];

		// Free allocated objects
		error_model_free(noise_model);
		free(probabilities);	
	}

	printf("Physical Rate \t Logical Rate\n");
	for (unsigned i = 0; i < n_increments; i++)
	{
		printf("%e \t %e ", rate_min * pow(rate_delta, i), logical_rate[i]);
		printf("\n");
	}

	return 0;	
}