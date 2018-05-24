#ifndef RANDOM_CODE_SEARCH
#define RANDOM_CODE_SEARCH

#include "sym.h"
#include "random_codes.h"
#include "misc/progress_bar.h"
#include "tailored.h"

// FUNCTION DECLARATIONS ----------------------------------------------------------------------------------------

random_code_return random_code_search(
	const unsigned n, 
	const unsigned k, 
	const unsigned r, 
	double (*error_model)(const sym*, void*),
	void* model_data);

// FUNCTION DEFINITIONS ----------------------------------------------------------------------------------------

random_code_return random_code_search_best_of_n_codes(
	const unsigned n, 
	const unsigned k, 
	const unsigned r, 
	double (*error_model)(const sym*, void*),
	void* model_data,
	const unsigned n_codes)
{
	double p_best = 0;
	sym* code_best = NULL, * logicals_best = NULL;

	for (size_t i = 0; i < n_codes; i++)
	{
		random_code_return rand_code = code_random(n, k, r); 
		sym* code = rand_code.code;
		sym* logicals = rand_code.logicals;

		double p_test = tailor_decoder_prob_only(code, logicals, error_model, model_data); 

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

double* random_code_search_stat_n_codes(
	const unsigned n, 
	const unsigned k, 
	const unsigned r, 
	double (*error_model)(const sym*, void*),
	void* model_data,
	const unsigned n_codes)
{
	double* code_stat = (double*)malloc(sizeof(double) * n_codes);

	for (size_t i = 0; i < n_codes; i++)
	{
		random_code_return rand_code = code_random(n, k, r); 
		sym* code = rand_code.code;
		sym* logicals = rand_code.logicals;

		code_stat[i] = tailor_decoder_prob_only(code, logicals, error_model, model_data);

		sym_free(code);
		sym_free(logicals);
	}
	return code_stat;
}



struct random_search_results
{
	double* probs;
	double p_best;
	sym* code;
	sym* logicals;
};

struct random_search_results random_code_search_best_of_n_codes_with_stats(
	const unsigned n, 
	const unsigned k, 
	const unsigned r, 
	double (*error_model)(const sym*, void*),
	void* model_data,
	const unsigned n_codes)
{
	double p_best = 0;
	sym* code_best = NULL, * logicals_best = NULL;
	double* code_stat = (double*)malloc(sizeof(double) * n_codes);

	char name[] = "Searching Random Codes: ";
	struct progress_bar bar = progress_bar_create(n_codes - 1, name);
	for (size_t i = 0; i < n_codes; i++)
	{
		progress_bar_update(&bar);

		random_code_return rand_code = code_random(n, k, r); 
		sym* code = rand_code.code;
		sym* logicals = rand_code.logicals;
		double p_test = tailor_decoder_prob_only_ignore_failures(code, logicals, error_model, model_data); 
		
		if (p_test < 0) // If we get a code without destabilisers due to some degeneracy, find a new code
		{
			sym_free(code);
			sym_free(logicals);
			i--;
			continue;
		}

		if (p_best < p_test)
		{
			p_best = p_test;
			code_stat[i] = p_test;
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
	printf("\n");
	struct random_search_results res = {code_stat, p_best, code_best, logicals_best};
	return res;
}

#endif