
#include "../sym.h"
#include "../codes/codes.h"
#include "../codes/candidate_codes.h"
#include "../decoders/tailored.h"
#include "../error_models/iid.h"
#include "../error_models/iid_biased.h"
#include "../characterise.h"
#include "../misc/progress_bar.h"

int main()
{	
	unsigned n_increments = 10;
	double physical_error_rate = 0.0001;
	double perror_init = 0.2;
	double perror_delta = 0.5;

	double logical_rate[24];

	unsigned n_qubits = 7, n_logicals = 1, distance = 3;

	sym* code = code_steane();
	sym* logicals = code_steane_logicals();

	double perror = perror_init;
	progress_bar* p = progress_bar_create(n_increments, "Decoder Bias Test");
	for (unsigned i = 0; i < n_increments; i++)
	{
		// Set the error rate
		perror *= perror_delta; 
		
		// Setup the error model
		error_model* noise_model = error_model_create_iid(n_qubits, perror);

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
	perror = perror_init;
	printf("((");
	for (unsigned i = 0; i < n_increments; i++)
	{
		perror *= perror_delta;
		printf("(%e, %.15e)", perror, 1.0 - logical_rate[i]);
		if (i != n_increments - 1)
		{
			printf(",\n");
		}
	}
	printf("))\n");

	return 0;	
}
