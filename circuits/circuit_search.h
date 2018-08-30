#ifndef CIRCUIT_SEARCH
#define CIRCUIT_SEARCH

#include "../sym.h"

#include "../gates/gates.h"
#include "circuit.h"

#include "../misc/progress_bar.h"

#include "../codes/codes.h"
#include "../codes/random_codes.h"
#include "../codes/random_code_search.h"

#include "../error_models/error_models.h"
#include "../error_models/lookup.h"

#include "../decoders/decoders.h"


typedef circuit* (*stabiliser_circuit_f)(const sym* code, const sym* logicals, gate* cnot, gate* hadamard, gate* phase);

random_code_return circuit_search_stabiliser(
	const unsigned n, 
	const unsigned k, 
	const unsigned r, 
	const unsigned n_codes,
	stabiliser_circuit_f stabiliser_circuit_builder,
	gate* cnot,
	gate* hadamard,
	gate* phase,
	gate* noise)
{
	double p_best = 0;
	sym* code_best = NULL, * logicals_best = NULL;

	for (size_t i = 0; i < n_codes; i++)
	{

		sym* code;
		sym* logicals;

		// Ensure that we can actually find destabilisers
		circuit* encode = NULL;
		while (NULL == encode)
		{
			random_code_return rand_code = code_random(n, k, r); 
			code = rand_code.code;
			logicals = rand_code.logicals;

			encode = stabiliser_circuit_builder(code, logicals, cnot, hadamard, phase);

			if (encode == NULL)
			{
				sym_free(code);
				sym_free(logicals);
			}
		}
		
		// This block should be changed to some input distribution
		double* initial_error_rate = (double*)malloc(sizeof(double) * (1 << (code->length)));
		memset(initial_error_rate, 0, (1 << (code->length)) * sizeof(double));
		initial_error_rate[0] = 1; // Set the identity to 1

		double* error_rates = circuit_run(encode, initial_error_rate, noise);
		free(initial_error_rate);
		
		error_model* md = error_model_create_lookup(n, error_rates);
		free(error_rates);
		
		double p_test = tailored_prob(code, logicals, md); 

		error_model_free(md);

		if (p_best < p_test)
		{
			p_best = p_test;
			if (NULL != code_best)
			{
				sym_free(code_best);
				sym_free(logicals_best);	
			}
			code_best = code;
			logicals_best = logicals;
		}
		else
		{
			sym_free(code);
			sym_free(logicals);
		}
	}

	random_code_return best_random_code = {code_best, logicals_best, p_best};
	return best_random_code;
}

#endif