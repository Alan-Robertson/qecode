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

#endif