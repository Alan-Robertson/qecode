//#include <iostream>
#include "codes.h"
#include "error_models.h"
#include "destabilisers.h"
#include "sym_iter.h"
#include "tailored.h"
#include "decoders.h"
#include "dmatrix.h"
#include "characterise.h"
#include "random_code_search.h"
#include "channel.h"


/*
 IF YOU SEE A RECOVERABLE MEMORY LEAK of 72,704 bytes in 1 block, it's caused by iostream
*/

int main()
{	
	/*
		Error Model
	*/
	unsigned n_qubits = 5, n_logicals = 1, distance = 3;
	unsigned n_codes_searched = 100;
	// Setup the error model
	error_model_f error_model = error_model_iid;

	// Setup the model data
	iid_model_data model_data;
	model_data.n_qubits = n_qubits;
	model_data.p_error = 0.1;

	/*
		QECC
	*/
	struct random_search_results r = random_code_search_best_of_n_codes_with_stats(
		n_qubits, 
		n_logicals,
		distance,
		error_model,
		(void*)&model_data,
		n_codes_searched); 

	sym* code = r.code;
	sym* logicals = r.logicals;

	sym** decoder_data = tailor_decoder(code, logicals, error_model, (void*)&model_data);
	decoder_f decoder = decoder_tailored;

	/*
		Printing and Cleanup
	*/

	double average = 0;

	for (size_t i = 0; i < n_codes_searched; i++)
	{
		average += r.probs[i];
	}
	average /= n_codes_searched;
		
	printf(" Codes Searched: %d\n Average correction probability: %e\n Best performance: %e\n", n_codes_searched, average, r.p_best);

	sym_print(code);
	sym_print(logicals);

	printf("Logical Error Channel\n");
	MatrixXcd lc = channel_logical(code, logicals, error_model, (void*)&model_data, decoder, (void*)&decoder_data);
	std::cout << lc << std::endl;

	// Free allocated objects
	sym_free(code);
	sym_free(logicals);
	free(r.probs);
	return 0;	
}
