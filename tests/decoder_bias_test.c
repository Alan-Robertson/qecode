
#include "../sym.h"
#include "../codes/codes.h"
#include "../decoders/tailored.h"
#include "../error_models/iid.h"
#include "../error_models/iid_biased.h"
#include "../error_models/weight_one.h"
#include "../characterise.h"
#include "../misc/progress_bar.h"

int main()
{	
	unsigned n_increments = 24;
	double physical_error_rate = 0.0001;

	double logical_rate[24];

	sym* code = code_steane();
	sym* logicals = code_steane_logicals();

    unsigned n_qubits = code->n_qubits, n_logicals = logicals->length / 2, distance = 3;

	double bias = 0.5;
	progress_bar* p = progress_bar_create(n_increments, "Decoder Bias Test");
	for (unsigned i = 0; i < n_increments; i++)
	{
		// Set the error rate
		bias *= 2; 
		
		// Setup the error model
		//error_model* noise_model = error_model_create_iid_biased_X(n_qubits, physical_error_rate, bias);
        error_model* noise_model = error_model_create_weight_one(n_qubits, physical_error_rate);

		// Tailor the Decoder
		decoder* tailored_decoder = decoder_create_tailored(code, logicals, noise_model);
		
		double* probabilities = characterise_code(code, logicals, noise_model, tailored_decoder);
		
		logical_rate[i] = probabilities[0];
		
		// Free allocated objects
		error_model_free(noise_model);
		decoder_free(tailored_decoder);
		free(probabilities);	

		progress_bar_update(p);
	}
	progress_bar_free(p);

	printf("Bias \t Logical Rate\n");
	bias = 0.5;
	printf("((");
	for (unsigned i = 0; i < n_increments; i++)
	{
		bias *= 2;
		printf("(%f, %.15f)", bias, logical_rate[i]);
		if (i != n_increments - 1)
		{
			printf(",\n");
		}
	}
	printf("))\n");

	return 0;	
}