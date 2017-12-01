#ifndef DESTABILISERS
#define DESTABILISERS

#include <stdbool.h>
#include "sym.h"
#include "error.h"
#include "sym_iter.h"

// Given a stabiliser code and a set of logicals, find the destabilisers
sym** destabilisers_generate(
	const sym* code, 
	const sym* logicals);

bool destabilisers_backtrack(
	const sym* code, 
	const sym* logicals, 
	sym** destabilisers, 
	unsigned row);

bool destabilisers_is_destabiliser(
	const sym* code, 
	const sym* destabiliser, 
	unsigned row);

// Frees all elements in a destabiliser
void destabilisers_free(sym** d, unsigned length);

// Recursive backtrack solution, this can almost certainly be improved upon
// Using binary symplectic elimination
sym** destabilisers_generate(const sym* code, const sym* logicals)
{
	// Setup the array of destabilisers
	sym** destabilisers = (sym**)malloc(sizeof(sym*) * code->height); 
	for (int i = 0; i < code->height; i++)
	{
		destabilisers[i] = sym_create(1, code->length);
	}

	if (destabilisers_backtrack(code, logicals, destabilisers, 0))
	{
		return destabilisers;
	}
	else
	{
		printf("Could not find a set of destabilisers for this code.\n");
		return NULL;
	}
	// Should not reach this point
	return NULL;
}

// Recursive backtrack
bool destabilisers_backtrack(
	const sym* code, 
	const sym* logicals, 
	sym** destabilisers, 
	unsigned row)
{
	
	// The base case
	if (row == code->height)
	{
		return true;
	}

	bool found_destabiliser = false;
	sym_iter* iter = sym_iter_create(code->length);
	// The all identity string should never be a destabiliser
	while(sym_iter_next(iter) && !found_destabiliser)
	{

		// Check whether our proposal is a destabiliser
		if (destabilisers_is_destabiliser(
			code,
			iter->state,
			row))
		{ // If it is then copy it to our destabilisers
			sym_row_copy(destabilisers[row], iter->state, 0, 0);
			found_destabiliser = destabilisers_backtrack(
				code,
				logicals,
				destabilisers,
				row + 1);
		}		
	}
	sym_iter_free(iter);
	return found_destabiliser;
} 


bool destabilisers_is_destabiliser(
	const sym* code, 
	const sym* destabiliser, 
	unsigned row)
{

	sym* syndrome = sym_syndrome(code, destabiliser);
	for (int i = 0; i <= code->height; i++)
	{
		if (i != row)
		{
			if (sym_get(syndrome, i, 0) == 1)
			{
				sym_free(syndrome);
				return false;
			};
		}
		else
		{
			if (sym_get(syndrome, i, 0) == 0)
			{
				sym_free(syndrome);
				return false;
			}
		}
	}
	sym_free(syndrome);
	return true;
}

/*
	// Check that it commutes with all bar one of the stabilisers
	for (int i = 0; i < code->height; i++)
	{
		unsigned anti_commutes = 0;
		for (int j = 0; j < code->length/2; j++)
		{
			// First check if there is an X component in the code and a Z component in the destabiliser
			// Then if there is an X in the destabiliser and a Z in the code
			// Then if there are both of these then they are both Y's and are XOR'ed together.
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
*/


void destabilisers_free(sym** d, unsigned length)
{
	for (int i = 0; i < length; i++)
	{
		sym_free(d[i]);
	}
	return;
}

#endif