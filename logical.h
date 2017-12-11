#ifndef LOGICALS
#define LOGICALS

#include "sym.h"

/*
	sym_logical_error:
	Applies an error to a given set of logical operators and returns the associated logical corrections
	:: const sym* code :: The logical operators being acted on
	:: const sym* error :: The error being applied
	Returns null if the error does not match the physical dimensions of the code, or if a pointer is invalid 
	Else it returns a pointer to the map of applied logicals
*/
sym* logical_error(const sym* error, const sym* logicals);

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
sym* logical_error(const sym* error, const sym* logicals)
{
	if ( logicals == NULL 
		|| error == NULL 
		|| error->length != logicals->height
		|| logicals->length % 2 != 0)
	{
		printf("Null pointer exception or matrices of incompatible sizes\n");
		return NULL;
	}

	const int half_length = logicals->height / 2;

	sym* l_map = sym_create(1, logicals->length);

	for (int i = 0; i <= error->length; i++)
	{
		if (ELEMENT_GET(error, 0, i)) // If there is no error on this qubit, skip it
		{
			for (int j = 0; j <= logicals->length; j++)
			{
				ELEMENT_XOR(l_map, 0, j, ELEMENT_GET(logicals, (i + half_length) % logicals->height, j) & ELEMENT_GET(error, 0, i));
			}
		}
	}
	
	return l_map;
}


sym** logical_as_destabilisers(const sym* logicals)
{
	sym** logical_destabilisers = (sym**)malloc(sizeof(sym*) * logicals->length);
	sym* t = sym_transpose(logicals);

	for (int i = 0; i < t->height; i++)
	{
		logical_destabilisers[i] = sym_create(1, t->length);
		sym_row_copy(logical_destabilisers[i], t, 0, i);
	}

	sym_free(t);
	return logical_destabilisers;
}

#endif