#ifndef CODES_LOW_WEIGHT
#define CODES_LOW_WEIGHT

#include <stdlib.h>
#include "../misc/heapsort.h"
#include "../tableau.h"
#include "../sym_iter.h"

// FUNCTION DECLARATIONS ----------------------------------------------------------------------------------------
/*
 *	lowest_weight_rep:
 *	Finds the lowest weight representation of a given stabiliser code
 *	:: const sym* s :: Pointer to the stabiliser code
 *	Returns the lowest weight representation
 */
sym* lowest_weight_rep(const sym* code);

/*
 *	low_weight_find_all_generators:
 *	Finds all possible generators for the given stabiliser code
 *	:: const sym* code :: Pointer to the stabiliser code
 *	Returns a pointer to an array of all generators
 */
sym** low_weight_find_all_generators(const sym* code);

/*
 *	low_weight_find_code_from_generators:
 *	Finds a code from a list of generators
 *	:: const sym* code :: Pointer to the stabiliser code
 *  :: const sym** generators :: Pointer to an array of generators
 *	Returns a new stabiliser code built from the generators with the same size and rank as the original code, it may also in fact be just the original code
 */
sym* low_weight_find_code_from_generators(const sym* code, const sym** generators)

// FUNCTION DEFINITIONS ----------------------------------------------------------------------------------------
/*
 *	lowest_weight_rep:
 *	Finds the lowest weight representation of a given stabiliser code
 *	:: const sym* code :: Pointer to the stabiliser code
 *	Returns the lowest weight representation
 */
sym* lowest_weight_rep(const sym* code)
{
	// Get an array of all the generators
	sym** all_generators = low_weight_find_all_generators(code);

	// Sort the generator array
	unsigned n_generators = (1ull << code->height) - 1; 
	low_weight_sort_generators(generators, n_generators);

	// Find a code using the sorted generators
	sym* lowest_weight_representation = low_weight_find_code_from_generators(code, generators);

	// Return our new low weight code
	return lowest_weight_representation;
}

/*
 *	low_weight_find_all_generators:
 *	Finds all possible generators for the given stabiliser code
 *	:: const sym* code :: Pointer to the stabiliser code
 *	Returns a pointer to an array of all generators
 */
sym** low_weight_find_all_generators(const sym* code)
{
	unsigned n_generators = (1ull << code->height) - 1;
	sym** initial_generators = (sym**)malloc(sizeof(sym*) * code->height);
	sym** generators = (sym**)malloc(sizeof(sym*) * n_generators);

	// Setup the initial generators
	for (size_t i = 0; i < code->height; i++)
	{
		initial_generators[i] = sym_create(1, code->length);
		sym_row_copy(initial_generators[i], code, 0, i);
	}

	// Use a bitmask from the integer numbers to iterate over all possible generators
	for (size_t i = 0; i < n_generators; i++)
	{
		uint32_t bitmask = i + 1;
		generators[i] = sym_create(1, code->length);
		for (size_t j = 0; j < code->height; j++)
		{
			if ((1 << j) & bitmask)
			{
				sym_add_in_place(generators[i], initial_generators[j]);
			}
		}
	}

	// Cleanup initial generators
	for (size_t i = 0; i < code->height; i++)
	{
		sym_free(initial_generators[i]);
	}
	free(initial_generators);

	return generators;
}

// Weight comparison function
int32_t low_weight_sym_weight_compare(const void* generator_a, const void* generator_b)
{
	int32_t weight_a = sym_weight(*(sym**)generator_a);
	int32_t weight_b = sym_weight(*(sym**)generator_b);
	return weight_a - weight_b;
}

/*
 *	low_weight_sort_generators:
 *	Sort a list of generators by Pauli weight
 *  :: sym** generators :: Pointer to an array of generators
 *  :: const unsigned n_generators :: The number of generators in the array
 *	The sort operation is performed in place, nothing is returned
 */
void low_weight_sort_generators(sym** generators, const unsigned n_generators)
{
	qsort(generators, n_generators, sizeof(sym*), sym_weight_compare);

	// Sort the generators using heapsort
	//heapsort(generators, n_generators, sizeof(sym*), sym_weight_compare);
	return;
}

/*
 *	low_weight_find_code_from_generators:
 *	Finds a code from a list of generators
 *	:: const sym* code :: Pointer to the stabiliser code
 *  :: const sym** generators :: Pointer to an array of generators
 *	Returns a new stabiliser code built from the generators with the same size and rank as the original code, it may also in fact be just the original code
 */
sym* low_weight_find_code_from_generators(const sym* code, const sym** generators)
{
	sym_iter* siter = sym_iter_create_range(code->length, code->length + 1, code->length + 1);
	sym* code_candidate = sym_create(code->height, code->length);

	uint32_t rank_found = 0;
	while (sym_iter_next(siter) && rank_found != n_codewords)
	{
		// Build a new candidate code
		uint32_t stabilisers_picked = 0;
		long long bitmask = sym_iter_value(siter);
		for (uint64_t i = 0; i < n_codewords + 1)
		{
			if ( (1 << i) & bitmask )
			{
				sym_row_copy(code_candidate, generators[i], stabilisers_picked, 0);
				stabilisers_picked++;
			}
		}

		// Check the rank
		rank_found = low_weight_code_rank(code_candidate);
	}
	sym_iter_free(siter);

	return code_candidate;
}


uint32_t low_weight_code_rank(const sym* code)
{
	sym* code_cpy = sym_copy(code);
	uint32_t max_rank = 0;

	// Check the rank of the code in S_X
	for (size_t i = 0; i < code_cpy->height && max_rank == 0; i++)
	{
		bool rank_deficient = true;
		for (size_t j = 0; j < code_cpy->length / 2 && rank_deficient; j++)
		{
			if (1 == sym_get(code_cpy, i, j))
			{
				rank_deficient = false;
			}
		}

		if (true == rank_deficient)
		{
			max_rank = i;
		}
	}

	// CNOT S_Z subset k to Identity via Gaussian elimination
	for (size_t i = max_rank; i < code_cpy->height; i++)
	{
		// Ensure that [i,i] is 1
		if ( 0 == sym_get(code_cpy, i, i + code_cpy->length / 2) )
		{
			bool pivot_found = false;
			for (size_t j = 0; j < code_cpy->length / 2 && !pivot_found; j++)
			{
				if (1 == sym_get(code_cpy, i, j + code_cpy->length / 2)) 
				{
					tableau_cnot(code_cpy, i, j);
					pivot_found = true;
				}
			}	
		}
		
		// Use i,i to pivot and eliminate the rest of the row
		for (size_t j = max_rank; j < code_cpy->length / 2; j++)
		{
			if (i != j && 1 == sym_get(code_cpy, i, j + code_cpy->length / 2))
			{
				tableau_cnot(code_cpyu, j, i);
			}
		}
	}
	
	// Hadamard on k to map the identity over to X
	for (size_t i = max_rank; i < code_cpy->length / 2; i++)
	{
		tableau_hadamard(code_cpy, i);
	}

	// The code should now have maximal rank in S_X, whatever that rank is.

	max_rank = 0;
	// CNOT S_X to Identity via Gaussian elimination
	for (size_t i = 0 ; i < code_cpy->height; i++)
	{
		// Ensure that [i,i] is 1
		if ( 0 == sym_get(code_cpy, i + , i) )
		{
			bool pivot_found = false;
			for (size_t j = i; j < code_cpyu->length / 2 && !pivot_found; j++)
			{
				if (1 == sym_get(code_cpy, i , j)) 
				{
					tableau_cnot(code_cpy, j, i);
					pivot_found = true;
					max_rank++;
				}
			}	
		}
		
		// Use i,i to pivot and eliminate the rest of the row
		for (size_t j = 0; j < code_cpy->length / 2 && pivot_found; j++)
		{
			if (i != j && 1 == sym_get(code_cpy, i, j))
			{
				tableau_cnot(code_cpy, i, j);
			}
		}
	}

	return max_rank;
}


#endif
