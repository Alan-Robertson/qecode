#ifndef ERRORS
#define ERRORS

#include "sym.h"

/* 
	error_create_simple:
	Creates a simple 1 qubit error
	:: const char pauli :: What error is being created, takes 'I', 'X', 'Y' or 'Z'
	:: const unsigned qubit :: Which qubit the error is being applied on
	:: const sym* code :: The code the error is being applied to
	Returns a sym* object representing the error string
*/
sym* error_create_simple(const char pauli, const unsigned qubit, const sym* code);

/* 
	error_add:
	Adds an error to an existing error string
	:: const char pauli :: What error is being created, takes 'I', 'X', 'Y' or 'Z'
	:: const unsigned qubit :: Which qubit the error is being applied on
	:: sym* error :: The error that this error is being added to
	Nothing returned, however modifies the contents of error
*/
void error_add(const char pauli, const unsigned qubit, sym* error);


sym* error_str_to_sym(const char* pauli_string);

bool error_inc(char* error, const size_t len);

char* error_sym_to_str(const sym* error);

int error_str_to_int(const char* error_string, const int len);

sym* error_int_to_sym(const unsigned* integer_arr, const size_t len);

#endif