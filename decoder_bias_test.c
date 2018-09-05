
#include "sym.h"
#include "codes/codes.h"
#include "decoders/tailored.h"
#include "error_models/iid.h"
#include "error_models/iid_biased.h"
#include "characterise.h"

#include "misc/progress_bar.h"

int main()
{	
	//double rate_min = 0.00000000001, rate_delta = 2;
	unsigned n_increments = 1000;
	double physical_error_rate = 0.005;

	double logical_rate[1000];

	unsigned n_qubits = 7, n_logicals = 1, distance = 3;
	unsigned n_codes_searched = 100000;

	sym* code = code_steane();
	sym* logicals = code_steane_logicals();

	progress_bar* b = progress_bar_create(n_increments, "Biased Decoder");
	for (unsigned i = 1; i <= n_increments; i++)
	{
		// Set the error rate
		double bias = i; 
		
		// Setup the error model
		error_model* noise_model = error_model_create_iid_biased_X(n_qubits, physical_error_rate, bias);

		// Tailor the Decoder
		decoder* tailored_decoder = decoder_create_tailored(code, logicals, noise_model);
		
		double* probabilities = characterise_code(code, logicals, noise_model, tailored_decoder);
		logical_rate[i] = probabilities[0];

		// Free allocated objects
		error_model_free(noise_model);
		free(probabilities);	
		progress_bar_update(b);
	}
	progress_bar_free(b);

	printf("Bias \t Logical Rate\n");
	for (unsigned i = 1; i <= n_increments; i++)
	{
		printf("%d \t %e ", i, 1.0 - logical_rate[i]);
		printf("\n");
	}

	return 0;	
}