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
	const sym* logicals, 
	const sym* destabilisers, 
	const sym* destabiliser, 
	unsigned row);

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
			logicals,
			destabilisers,
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
	const sym* logicals, 
	const sym* destabilisers, 
	const sym* destabiliser, 
	unsigned row)
{
	// Check that it commutes with all bar one of the stabilisers
	for (int i = 0; i < code->height; i++)
	{
		unsigned anti_commutes = 0;
		for (int j = 0; j < code->length/2; j++)
		{
			if (sym_get(code, i, j) ^ sym_get(destabiliser, 0, j + code->length/2)) // Check if element does not commute	
			{
				anti_commutes++;
			}
		}
		// If it doesn't commute and it's not the destabiliser for that particular row
		// Or if it does commute and it is the destabiliser for that particular row
		// Then the candidate is not a destabiliser and returns false
			
		if (((anti_commutes % 2) && (i != row)) 
			|| 
			(!(anti_commutes % 2) && (i == row)))  
		{
			return false; 
		} 
	}

	// Check that it commutes with all of the current destabilisers
	for (size_t i = 0; i < row; i++)
	{
		unsigned commutes = 0;
		for (size_t j = 0; j < destabilisers->length; j++)
		{
			if (sym_get(destabilisers, i, j) ^ sym_get(destabiliser, 0, j)) // Check if element does not commute	
			{
				commutes++;
			}	
		}
		
		// If it doesn't commute then the candidate is not a good destabiliser
		if (commutes % 2) 
		{
			return false; 
		} 
	}

	// Meets all requirements
	return true;
}

#endif