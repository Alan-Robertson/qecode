#ifndef DESTABILISERS
#define DESTABILISERS

// LIBRARIES ----------------------------------------------------------------------------------------

#include <stdbool.h>
#include "sym.h"
#include "errors.h"
#include "logical.h"
#include "sym_iter.h"

// FUNCTION DECLARATIONS ----------------------------------------------------------------------------------------

/* 
    destabilisers_generate:
	Given a stabiliser code and a set of logicals, find the destabilisers
	:: const sym* code :: The stabiliser code to generate the destabilisers for
	:: const sym* logicals ::  The logicals associated with the stabiliser code
	Returns an array of pointers to sym objects that are the destabilisers ordered by stabiliser
	i.e: the ith stabiliser will have a destabiliser located at [i]
*/
sym** destabilisers_generate(
	const sym* code, 
	const sym* logicals);

/* 
    destabilisers_backtrack:
	A backtrack implementation that searches across all possible collections of paulis to find a valid set of destabilisers
	:: const sym* code :: The stabiliser code to generate the destabilisers for
	:: const sym* logicals ::  The logicals associated with the stabiliser code
	:: sym** destabilisers :: The currently determined destabilisers
	Returns true or false if the current branch of the backtrack has found a valid set of destabilisers
*/
bool destabilisers_backtrack(
	const sym* code, 
	const sym* logicals, 
	sym** destabilisers, 
	unsigned row);

/* 
    destabilisers_is_destabiliser:
	Checks whether a particular destabiliser candidate is actually a destabiliser
	:: const sym* code :: The stabiliser code to generate the destabilisers for
	:: const sym* logicals ::  The logicals associated with the stabiliser code
	:: sym** destabilisers :: The currently determined destabilisers
	:: const sym* destabiliser_candidate :: The candidate being tested
	:: const unsigned row :: The current row
	Returns true or false if the current candidate is or isn't a destabiliser
*/
bool destabilisers_is_destabiliser(
	const sym* code, 
	const sym* logials,
	sym** destabilisiers,
	const sym* destabiliser_candidate, 
	const unsigned row);

/* 
    destabilisers_print:
	Prints all elements in a destabiliser
	:: sym** d :: The set of destabilisers to print
	:: const unsigned length ::  The number of destabilisers in the set
	Returns nothing
*/
void destabilisers_print(sym** d, const unsigned length);

/* 
    destabilisers_free:
	Frees all elements in a destabiliser
	:: sym** d :: The set of destabilisers to free
	:: const unsigned length ::  The number of destabilisers in the set
	Returns nothing
*/
void destabilisers_free(sym** d, unsigned length);

// FUNCTION DEFINITIONS ----------------------------------------------------------------------------------------

// Recursive backtrack solution, this can almost certainly be improved upon
// Using binary symplectic elimination
/* 
    destabilisers_generate:
	Given a stabiliser code and a set of logicals, find the destabilisers
	:: const sym* code :: The stabiliser code to generate the destabilisers for
	:: const sym* logicals ::  The logicals associated with the stabiliser code
	Returns an array of pointers to sym objects that are the destabilisers ordered by stabiliser
	i.e: the ith stabiliser will have a destabiliser located at [i]
*/
sym** destabilisers_generate(
	const sym* code, 
	const sym* logicals)
{
	// Setup the array of destabilisers
	sym** destabilisers = (sym**)malloc(sizeof(sym*) * code->height); 
	for (int i = 0; i < code->height; i++)
	{
		destabilisers[i] = sym_create(1, code->length);
	}

	// Call the backtrack function, if it returns true then we have found destabilisers, otherwise we have not
	if (destabilisers_backtrack(code, logicals, destabilisers, 0))
	{
		// Return our correct destabilisers
		return destabilisers;
	}
	else
	{
		// No destabilisers found, this should be dealt with
		printf("Could not find a set of destabilisers for this code.\n");
		return NULL;
	}
	printf("Borked");
	// Should not reach this point
	return NULL;
}

/* 
    destabilisers_backtrack:
	A backtrack implementation that searches across all possible collections of paulis to find a valid set of destabilisers
	:: const sym* code :: The stabiliser code to generate the destabilisers for
	:: const sym* logicals ::  The logicals associated with the stabiliser code
	:: sym** destabilisers :: The currently determined destabilisers
	Returns true or false if the current branch of the backtrack has found a valid set of destabilisers
*/
bool destabilisers_backtrack(
	const sym* code, 
	const sym* logicals, 
	sym** destabilisers, 
	const unsigned row)
{
	
	// The base case
	if (row == code->height)
	{
		return true;
	}

	bool found_destabiliser = false;
	sym_iter* destabiliser_candidate = sym_iter_create(code->length);
	while(sym_iter_next(destabiliser_candidate) && !found_destabiliser)
	{

		// Check whether our proposal is a destabiliser
		if (destabilisers_is_destabiliser(
			code,
			logicals,
			destabilisers,
			destabiliser_candidate->state,
			row))
		{ // If it is a destabiliser, then copy it to our destabilisers
			sym_row_copy(destabilisers[row], destabiliser_candidate->state, 0, 0);
			found_destabiliser = destabilisers_backtrack(
				code,
				logicals,
				destabilisers,
				row + 1);
		}		
	}
	// Free up memory and return
	sym_iter_free(destabiliser_candidate);
	return found_destabiliser;
} 

/* 
    destabilisers_is_destabiliser:
	Checks whether a particular destabiliser candidate is actually a destabiliser
	:: const sym* code :: The stabiliser code to generate the destabilisers for
	:: const sym* logicals ::  The logicals associated with the stabiliser code
	:: sym** destabilisers :: The currently determined destabilisers
	:: const sym* destabiliser_candidate :: The candidate being tested
	:: const unsigned row :: The current row
	Returns true or false if the current candidate is or isn't a destabiliser
*/
bool destabilisers_is_destabiliser(
	const sym* code, 
	const sym* logicals,
	sym** destabilisers,
	const sym* destabiliser_candidate,
	unsigned row)
{
	// Check that it commutes with the logicals
	sym* logical_syndrome = logical_error(logicals, destabiliser_candidate);
	for (int i = 0; i < logicals->height; i++)
	{
		if (sym_get(logical_syndrome, 0, i))
		{
			sym_free(logical_syndrome);
			return false;
		}
	}
	sym_free(logical_syndrome);

	// Check that it commutes with all bar one of the stabilisers
	sym* syndrome = sym_syndrome(code, destabiliser_candidate);
	for (int i = 0; i < code->height; i++)
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

	// Check that it commutes with the other destabilisers
	for (int i = 0; i < row; i++)
	{
		sym* destabiliser_syndrome = sym_syndrome(destabilisers[i], destabiliser_candidate);
		
		if (sym_get(destabiliser_syndrome, 0, 0) == 1)
		{
			sym_free(destabiliser_syndrome);
			return false;
		}
		sym_free(destabiliser_syndrome);
	}

	// All checks passed, it is a destabiliser, return it
	return true;
}

/* 
    destabilisers_print:
	Prints all elements in a destabiliser
	:: sym** d :: The set of destabilisers to print
	:: const unsigned length ::  The number of destabilisers in the set
	Returns nothing
*/
void destabilisers_print(sym** d, const unsigned length)
{
	for (int i = 0; i < length; i++)
	{
		sym_print(d[i]);
	}
	return;
}

/* 
    destabilisers_free:
	Frees all elements in a destabiliser
	:: sym** d :: The set of destabilisers to free
	:: const unsigned length ::  The number of destabilisers in the set
	Returns nothing
*/
void destabilisers_free(sym** d, const unsigned length)
{
	for (int i = 0; i < length; i++)
	{
		sym_free(d[i]);
	}
	free(d);
	return;
}

#endif