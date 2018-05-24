#ifndef CODES_LOW_WEIGHT
#define CODES_LOW_WEIGHT

#include <stdlib.h>
#include "../misc/heapsort.h"

sym** find_all_generators(sym* code);
void sort_generators(sym** generators, unsigned n_generators);

int32_t sym_weight_compare(const void* generator_a, const void* generator_b);


sym* lowest_weight_rep(sym* code)
{
	unsigned n_generators = (1ull << code->height) - 1; 

	sym** all_generators = low_weight_find_all_generators(code);

	// Sort the generators
	low_weight_sort_generators(generators, n_generators);

	//
	sym* lowest_weight_representation = find


	return lowest_weight_representation;
}



sym** low_weight_find_all_generators(sym* code)
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

void low_weight_sort_generators(sym** generators, unsigned n_generators)
{
	qsort(generators, n_generators, sizeof(sym*), sym_weight_compare);

	// Sort the generators using heapsort
	//heapsort(generators, n_generators, sizeof(sym*), sym_weight_compare);
	return;
}

#endif
