#ifndef LOGICALS
#define LOGICALS

#include "sym.h"

// FUNCTION DECLARATIONS ----------------------------------------------------------------------------------------

/*
	logical_error:
	Applies an error to a given set of logical operators and returns the associated logical corrections
	:: const sym* code :: The logical operators being acted on
	:: const sym* error :: The error being applied
	Returns null if the error does not match the physical dimensions of the code, or if a pointer is invalid 
	Else it returns a pointer to the map of applied logicals
*/
sym* logical_error(const sym* logicals, const sym* error);

/*
	logical_as_destabilisers:
	Converts a set of logical operators to the same format as used by the destabilisers
	:: const sym* logicals :: The logical operators in question
	Returns an array of pointers to sym objects that are the destabilisers ordered by stabiliser
	i.e: the ith stabiliser will have a destabiliser located at [i]
*/
sym** logical_as_destabilisers(const sym* logicals);

// FUNCTION DEFINITIONS ----------------------------------------------------------------------------------------

/*
	sym_logical_error:
	Applies an error to a given set of logical operators and returns the associated logical corrections
	:: const sym* code :: The logical operators being acted on
	:: const sym* error :: The error being applied
	Returns null if the error does not match the physical dimensions of the code, or if a pointer is invalid 
	Else it returns a pointer to the map of applied logicals
*/
	/* Matrix Multiplication version:
		sym* symplectic = sym_code_symplectic(error->length);
		sym* mult = sym_multiply(error, symplectic);
		sym* l_map = sym_multiply(mult, logicals);

		sym_free(symplectic);
		sym_free(mult);
		return l_map;
	*/
sym* logical_error(const sym* logicals, const sym* error)
{
	// Sanity checking for the objects being passed to this function
	if ( logicals == NULL 
		|| error == NULL 
		|| error->length != logicals->height
		|| logicals->length % 2 != 0)
	{
		printf("Null pointer exception or matrices of incompatible sizes\n");
		return NULL;
	}
	// Helper variable
	const int half_length = logicals->height / 2;

	// Where we are going to store our logical syndromes
	sym* l_map = sym_create(1, logicals->length);

	// Calculate the logical syndromes
	for (int i = 0; i <= error->length; i++)
	{
		if (sym_get(error, 0, i)) // If there is no error on this qubit, skip it
		{
			for (int j = 0; j <= logicals->length; j++)
			{
				ELEMENT_XOR(l_map, 0, j, ELEMENT_GET(logicals, (i + half_length) % logicals->height, j) & ELEMENT_GET(error, 0, i));
			}
		}
	}
	
	// And return them
	return l_map;
}

/*
	logical_as_destabilisers:
	Converts a set of logical operators to the same format as used by the destabilisers
	:: const sym* logicals :: The logical operators in question
	Returns an array of pointers to sym objects that are the destabilisers ordered by stabiliser
	i.e: the ith stabiliser will have a destabiliser located at [i]
	This object should be freed using destabiliser_free
*/
sym** logical_as_destabilisers(const sym* logicals)
{
	// The destabiliser object we are preparing
	sym** logical_destabilisers = (sym**)malloc(sizeof(sym*) * logicals->length);

	// The transpose of our logicals 
	sym* t = sym_transpose(logicals);

	// For each row in the transpose find the logical operator that anti-commutes with it
	for (int i = 0; i < t->height; i++)
	{
		logical_destabilisers[i] = sym_create(1, t->length);
		sym_row_copy(logical_destabilisers[i], t, 0, i);

		// Get the syndrome for that row
		sym* syndrome = sym_syndrome(t, logical_destabilisers[i]);
		// Find the row that it anti-commutes with
		for (int j = 0; j < syndrome->height; j++)
		{
			// If they anti-commute then set that as the destabiliser
			if (1 == sym_get(syndrome, j, 0))
			{
				sym_row_copy(logical_destabilisers[i], t, 0, j);
			}
		}
		sym_free(syndrome);
	}

	sym_free(t);
	return logical_destabilisers;
}

#endif