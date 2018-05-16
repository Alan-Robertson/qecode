#ifndef CIRCUIT_SEARCH
#define CIRCUIT_SEARCH

#include "sym.h"
#include "random_codes.h"
#include "tailored.h"
#include "progress_bar.h"
#include "codes.h"
#include "error_models.h"
#include "tailored.h"
#include "decoders.h"
#include "characterise.h"
#include "gates.h"
#include "circuit.h"
#include "random_code_search.h"

typedef circuit* (*stabiliser_circuit_f)(const sym* code, const sym* logicals, sym** destabilisers, gate* cnot, gate* hadamard, gate* phase);

random_code_return circuit_search_stabiliser(
	const unsigned n, 
	const unsigned k, 
	const unsigned r, 
	const unsigned n_codes,
	stabiliser_circuit_f stabiliser_circuit_builder,
	gate* cnot,
	gate* hadamard,
	gate* phase)
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

			encode = stabiliser_circuit_builder(code, logicals, NULL, cnot, hadamard, phase);

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

		double* error_rates = circuit_run(encode, initial_error_rate);
		free(initial_error_rate);
		
		lookup_error_model_data md;
		md.lookup_table = error_rates;
		double p_test = tailor_decoder_prob_only(code, logicals, error_model_lookup, &md); 
		free(error_rates);

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

	random_code_return best_random_code = {code_best, logicals_best};
	return best_random_code;
}

#endif