#ifndef CODEWORD
#define CODEWORD

#include "sym.h"
#include "circuit.h"


sym* codeword_find(const sym* code, const sym* logicals)
{
	sym* codeword = NULL;
	bool found_codeword = false;
	sym_iter* codeword_candidate = sym_iter_create(code->length);
	while(sym_iter_next(codeword_candidate) && !found_codeword)
	{
		// Check whether our proposal is a codeword
		if (codeword_is_codeword(
			code,
			logicals,
			codeword_candidate->state))
		{
			codeword = sym_copy(codeword_candidate->state);
		}
	}
	sym_iter_free(codeword_candidate);
	return codeword;
}

bool codeword_is_codeword(
	const sym* code, 
	const sym* logicals,
	const sym* codeword_candidate)
{
	// Check that it commutes with the logicals (+1 eigenstate)
	sym* logical_syndrome = logical_error(logicals, codeword_candidate);
	for (int i = 0; i <= logicals->height; i++)
	{
		if (sym_get(logical_syndrome, 0, i))
		{
			sym_free(logical_syndrome);
			return false;
		}
	}
	sym_free(logical_syndrome);

	// Check that it commutes with all of the stabilisers
	sym* syndrome = sym_syndrome(code, codeword_candidate);
	for (int i = 0; i <= code->height; i++)
	{
		if (sym_get(syndrome, i, 0) == 1)
			{
				sym_free(syndrome);
				return false;
			};
		}
	}
	sym_free(syndrome);

	// All checks passed, it is a codeword, return it
	return true;
}

#endif