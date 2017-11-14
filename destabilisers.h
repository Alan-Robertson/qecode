#ifndef DESTABILISERS
#define DESTABILISERS

#include <stdbool.h>
#include "sym.h"
#include "error.h"

// Given a stabiliser code and a set of logicals, find the destabilisers
sym* destabilisers_generate(
	const sym* code, 
	const sym* logicals);

bool destabilisers_backtrack(
	const sym* code, 
	const sym* logicals, 
	sym* destabilisers, 
	unsigned row);

bool destabiliser_is_destabiliser(
	const sym* code, 
	const sym* destabiliser, 
	unsigned row);


sym* destabilisers_low_weight_generate(
	const sym* code, 
	const sym* logicals);

bool destabilisers_low_weight_backtrack(
	const sym* code, 
	const sym* logicals, 
	sym* destabilisers, unsigned row);


// Recursive backtrack solution, this can almost certainly be improved upon
// Using binary symplectic elimination
sym* destabilisers_generate(const sym* code, const sym* logicals)
{
	sym* destabilisers = sym_create(code->height, code->length);
	if (destabilisers_backtrack(code, logicals, destabilisers, 0))
	{
		return destabilisers;
	}
	else
	{
		printf("Could not find a set of destabilisers for this code.\n");
		return NULL;
	}
}

// Recursive backtrack
bool destabilisers_backtrack(const sym* code, const sym* logicals, sym* destabilisers, unsigned row)
{
	// The base case
	if (row == code->height)
	{
		return true;
	}

	char tested_destabiliser_str[code->length/2 + 1];
	for (int i = 0; i < code->length / 2; i++)
	{
		tested_destabiliser_str[i] = 'I'; // Initialise error string to the identity
	}
	tested_destabiliser_str[code->length / 2] = '\0'; // Null terminator
	
	bool found_destabiliser = false;
	while (!found_destabiliser)
	{
		// Update the tested destabiliser string (All I cannot be a destabiliser)
		if (!error_inc(tested_destabiliser_str, code->length / 2))
		{
			// Tried all possible destabilisers, time to go back up.
			return false;
		} 

		// Get the sym representation
		sym* tested_destabiliser = error_str_to_sym(tested_destabiliser_str);

		// Check whether our proposal is a destabiliser
		if (destabiliser_is_destabiliser(
			code,
			tested_destabiliser,
			row))
		{
			memcpy(
				&(destabilisers->matrix[row]), 
				&(tested_destabiliser->matrix),
				sym_matrix_bytes(tested_destabiliser));

			found_destabiliser = destabilisers_backtrack(
				code,
				logicals,
				destabilisers,
				row + 1);
		}		
		// Could operate in-place instead, would likely be much faster
		sym_free(tested_destabiliser);
	}
	return found_destabiliser;
} 

bool destabiliser_is_destabiliser(
	const sym* code, 
	const sym* destabiliser, 
	unsigned row)
{
	// Check that it commutes with all bar one of the stabilisers
	for (int i = 0; i < code->height; i++)
	{
		unsigned anti_commutes = 0;
		for (int j = 0; j < code->length/2; j++)
		{
			if ( 
				(sym_get(code, i, j) & sym_get(destabiliser, 0, j + code->length/2)) 
				^ 
				(sym_get(code, i, j + code->length/2) & sym_get(destabiliser, 0, j))) // Check if element does not commute	
			{
				anti_commutes++;
			}
		}
		anti_commutes %= 2;

		// If it doesn't commute and it's not the destabiliser for that particular row
		// Or if it does commute and it is the destabiliser for that particular row
		// Then the candidate is not a destabiliser and returns false
		if ( anti_commutes && i != row || !anti_commutes && i == row) {
			return false;
		}
	}
	return true;
}


sym* destabilisers_low_weight_generate(const sym* code, const sym* logicals)
{
	sym* destabilisers = sym_create(code->height, code->length);
	if (destabilisers_low_weight_backtrack(code, logicals, destabilisers, 0))
	{
		return destabilisers;
	}
	else
	{
		printf("Could not find a set of destabilisers for this code.\n");
		return NULL;
	}
}

// Recursive backtrack
bool destabilisers_low_weight_backtrack(const sym* code, const sym* logicals, sym* destabilisers, unsigned row)
{
	// The base case
	if (row == code->height)
	{
		return true;
	}

	char tested_destabiliser_str[code->length/2 + 1];
	for (int i = 0; i < code->length / 2; i++)
	{
		tested_destabiliser_str[i] = 'I'; // Initialise error string to the identity
	}
	tested_destabiliser_str[code->length / 2] = '\0'; // Null terminator
	
	bool found_destabiliser = false;
	bool checked_all = false;
	unsigned curr_weight = 0;
	while (!checked_all)
	{
		// Update the tested destabiliser string (All I cannot be a destabiliser)
		if (!error_inc(tested_destabiliser_str, code->length / 2))
		{
			// Tried all possible destabilisers, time to go back up.
			checked_all = true;
		} 

		// Get the sym representation
		sym* tested_destabiliser = error_str_to_sym(tested_destabiliser_str);

		// Check whether our proposal is a destabiliser
		if (destabiliser_is_destabiliser(
			code,
			tested_destabiliser,
			row))
		{
			if (found_destabiliser) // if we've already found a destabiliser, compare weights
			{
				sym_print(tested_destabiliser);
				printf("%d\n", sym_weight(tested_destabiliser));
				if (sym_weight(tested_destabiliser) < curr_weight)
				{
					memcpy(
					&(destabilisers->matrix[row]), 
					&(tested_destabiliser->matrix),
					sym_matrix_bytes(tested_destabiliser));
				}
			}
			else 
			{ // Otherwise this is the first one and just add it
				curr_weight = sym_weight(tested_destabiliser);
				sym_print(tested_destabiliser);
				printf("%d\n", sym_weight(tested_destabiliser));
				found_destabiliser = true;
				memcpy(
					&(destabilisers->matrix[row]), 
					&(tested_destabiliser->matrix),
					sym_matrix_bytes(tested_destabiliser));
			}
		}		
		// Could operate in-place instead, would likely be much faster
		sym_free(tested_destabiliser);
	}

	if (found_destabiliser)
	{
		found_destabiliser = destabilisers_backtrack(
					code,
					logicals,
					destabilisers,
					row + 1);
	}
	return found_destabiliser;
} 


#endif