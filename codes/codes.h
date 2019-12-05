#ifndef CODES
#define CODES

#include "../sym.h"
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>

/* 
	Lists of stabiliser codes
*/

// Steane code
sym* code_steane();
sym* code_steane_logicals();

// Gottesman's [8,3,3] code
sym* code_8_3_3_gottesman();
sym* code_8_3_3_gottesman_logicals();

// Shor code
sym* code_shor();
sym* code_shor_logicals();

// Two qubit bit flip code
sym* code_two_qubit();
sym* code_two_qubit_logicals();

/* 
    sym_code_steane:
	Creates a new symplectic matrix object containing the 7 qubit Steane code
	Returns a heap pointer to the new sym object
*/
sym* code_steane()
{
	unsigned steane[] = 
	{
		1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 
		0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 
		0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1
	};
	return sym_create_valued(6, 14, steane);
}

/* 
    sym_code_steane:
	Creates a new symplectic matrix object containing the logical operators for the 7 qubit Steane code
	Returns a heap pointer to the new sym object
*/
sym* code_steane_logicals()
{
	unsigned code[] = 
	{
		1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 
		0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1
	};
	sym* s = sym_create_valued(2, 14, code);
	sym* t = sym_transpose(s);
	sym_free(s);
	return t;
}

/* 
    sym_code_8_3_3:
	Creates a new symplectic matrix object containing a representation of the [8,3,3] code
	Returns a heap pointer to the new sym object
*/
sym* code_8_3_3()
{
	unsigned code[] = 
	{
	1, 0, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 1,
      	0, 1, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 1, 0,
      	0, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 0, 1, 1, 1, 0,
      	0, 0, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 0, 0, 1,
      	0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1
	};
	return sym_create_valued(5, 16, code);
}

/* 
    sym_code_8_3_3_gottesman:
	Creates a new symplectic matrix object containing a representation of the [8,3,3] code
	Returns a heap pointer to the new sym object
*/
sym* code_8_3_3_gottesman()
{
	unsigned code[] = 
	{
		1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0,
      	0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1,
      	0, 1, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1,
      	0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 1,
      	0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1
	};
	return sym_create_valued(5, 16, code);
}

/* 
    sym_code_8_3_3_gottesman_logicals:
	Creates a new symplectic matrix object containing the logicals operators of a representation of the [8,3,3] code
	Returns a heap pointer to the new sym object
*/
sym* code_8_3_3_gottesman_logicals()
{
	unsigned code[] = 
	{
		1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 
		1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 
		1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 
		0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1 
	};
	sym* s =  sym_create_valued(6, 16, code);
	sym* t = sym_transpose(s);
	sym_free(s);
	return t;
}

sym* code_11_1_5_gottesman()
{
	unsigned code[] = 
	{
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
		1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 0, 1,
		0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0,
		0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0,
		1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 0, 0,
		0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0,
		0, 0, 0, 1, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0,
		0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 1, 0, 1,
		1, 0, 1, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0
	};
	return sym_create_valued(5, 16, code);

}

sym* code_11_1_5_gottesman_logicals()
{
	unsigned code[] = 
	{
		0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1
	};
	sym* s =  sym_create_valued(6, 16, code);
	sym* t = sym_transpose(s);
	sym_free(s);
	return t;
}


/* 
    sym_code_shor:
	Creates a new symplectic matrix object containing the stabilisers for the 9 qubit Shor code
	Returns a heap pointer to the new sym object
*/
sym* code_shor()
{
	unsigned code[] = 
	{
		0, 0, 0, 0, 0, 0, 0, 0, 0,  1, 1, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0,  1, 0, 1, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 1, 1, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 1, 0, 1, 0, 0, 0, 
		0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 1, 1, 0, 
		0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 1, 1, 
		1, 1, 1, 1, 1, 1, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,
		1, 1, 1, 0, 0, 0, 1, 1, 1,  0, 0, 0, 0, 0, 0, 0, 0, 0
	};
	return sym_create_valued(8, 18, code);
}

/* 
    sym_code_shor:
	Creates a new symplectic matrix object containing the logical operators for the 9 qubit Shor code
	Returns a heap pointer to the new sym object
*/
sym* code_shor_logicals()
{
	unsigned code[] = {
		1, 1, 1, 1, 1, 1, 1, 1, 1,  0, 0, 0, 0, 0, 0, 0, 0, 0, 					// XXXXXXXXX
		0, 0, 0, 0, 0, 0, 0, 0, 0,  1, 1, 1, 1, 1, 1, 1, 1, 1  					// ZZZZZZZZZ
	};
	sym* s = sym_create_valued(2, 18, code);
	sym* t = sym_transpose(s);
	sym_free(s);
	return t;
}

/* 
    sym_code_shor:
	Creates a new symplectic matrix object containing the stabilisers for the 2 qubit bit flip code
	Returns a heap pointer to the new sym object
*/
sym* code_two_qubit()
{
	unsigned code[] = { 
		0, 0,  1, 0 																// ZI
	};
	return sym_create_valued(1, 4, code);
}

/* 
    sym_code_shor:
	Creates a new symplectic matrix object containing the logical operators for the 2 qubit bit flip
	Returns a heap pointer to the new sym object
*/
sym* code_two_qubit_logicals()
{
	unsigned code[] = {
		0, 1,  0, 0,																// IX
		0, 0,  0, 1																// IZ
	};
	sym* s = sym_create_valued(2, 4, code);
	sym* t = sym_transpose(s);
	sym_free(s);
	return t;
}

sym* code_three_qubit()
{
	unsigned code[] = { 
		1, 1, 0,  0, 0, 0, 														// XXI
		0, 1, 1,  0, 0, 0														// IXX
	};
	return sym_create_valued(2, 6, code);
}

sym* code_three_qubit_logicals()
{
	unsigned code[] = { 
		0, 1, 0,  0, 0, 0, 														// IXI
		0, 0, 0,  1, 1, 1														// ZZZ
	};
	sym* s = sym_create_valued(2, 6, code);
	sym* t = sym_transpose(s);
	sym_free(s);
	return t;
}

sym* code_five_qubit()
{
	unsigned code[] = {
		1, 0, 0, 0, 1,  0, 1, 0, 1, 0,
		1, 1, 0, 0, 0,  0, 0, 1, 0, 1,
		0, 1, 1, 0, 0,  1, 0, 0, 1, 0,
		0, 0, 1, 1, 0,  0, 1, 0, 0, 1			
	};
	return sym_create_valued(4, 10, code);
}

sym* code_five_qubit_logicals()
{
	unsigned code[] = {
		1, 1, 1, 1, 1,  0, 0, 0, 0, 0,
		0, 0, 0, 0, 0,  1, 1, 1, 1, 1
	};
	sym* s = sym_create_valued(2, 10, code);
	sym* t = sym_transpose(s);
	sym_free(s);
	return t;
}


// Need to remember where this code came from, looks like the logicals are LD on the stabilisers
sym* code_asymmetric_five()
{
	unsigned code[] = {
		0, 0, 0, 0, 0,  0, 1, 1, 0, 0,
		0, 0, 0, 0, 0,  1, 0, 1, 0, 0,
		0, 0, 0, 1, 1,  0, 0, 0, 0, 0,
		0, 0, 0, 1, 0,  0, 0, 1, 0, 0			
	};
	return sym_create_valued(4, 10, code);
}

sym* code_asymmetric_five_logicals()
{
	unsigned code[] = {
		0, 0, 0, 1, 1,  1, 1, 1, 0, 0,
		1, 1, 1, 0, 0,  0, 0, 0, 1, 1
	};
	sym* s = sym_create_valued(2, 10, code);
	sym* t = sym_transpose(s);
	sym_free(s);
	return t;
}

sym* code_cyclic_seven()
{
	unsigned code[] = {
		1, 0, 0, 0, 0, 1, 0,  0, 1, 0, 0, 1, 0, 0,
		0, 1, 0, 0, 0, 0, 1,  0, 0, 1, 0, 0, 1, 0,
		1, 0, 1, 0, 0, 0, 0,  0, 0, 0, 1, 0, 0, 1,
		0, 1, 0, 1, 0, 0, 0,  1, 0, 0, 0, 1, 0, 0,
		0, 0, 1, 0, 1, 0, 0,  0, 1, 0, 0, 0, 1, 0,
		0, 0, 0, 1, 0, 1, 0,  0, 0, 1, 0, 0, 0, 1				
	};
	return sym_create_valued(6, 14, code);
}

sym* code_cyclic_seven_logicals()
{
	unsigned code[] = {
		1, 1, 1, 1, 1, 1, 1,  0, 0, 0, 0, 0, 0, 0, 
		0, 0, 0, 0, 0, 0, 0,  1, 1, 1, 1, 1, 1, 1
	};
	sym* s = sym_create_valued(2, 14, code);
	sym* t = sym_transpose(s);
	sym_free(s);
	return t;
}


sym* code_random_candidate_seven()
{
	unsigned candidate[] = {
                1,0,0,1,0,1,1 ,0,0,0,0,0,0,0, 
                0,1,0,1,1,0,1 ,0,0,0,0,0,0,0, 
                0,0,1,0,1,1,0 ,1,1,0,0,0,0,1, 
                0,0,0,0,0,0,0 ,0,0,0,1,0,0,1, 
                0,0,0,0,0,0,0 ,0,1,1,0,1,0,0, 
                0,0,0,0,0,0,0 ,1,0,1,0,0,1,0    
        };

       return sym_create_valued(6, 14, candidate);
}

sym* code_random_candidate_seven_logicals()
{
	 unsigned candidate_logicals[] = 
        {
                0 ,0, 
                0 ,0, 
                0 ,0, 
                1 ,0, 
                0 ,0, 
                0 ,0, 
                1 ,0, 
                0 ,1, 
                0 ,1, 
                1 ,0, 
                0 ,0, 
                0 ,0, 
                0 ,0, 
                0 ,1
        };
        return sym_create_valued(14, 2, candidate_logicals);
}

//-----------------------------------------------------------

/* Helper function for debugging */
void code_bitstring(unsigned bit) 
{
	for (int i = 7; i >= 0; i--){
		int d = !!(bit & (1 << i));
		printf("%d", d);
	}
	printf("\n");
}


sym* code_symplectic(const unsigned size)
{
	sym* s = sym_create(size, size);
	for (int i = 0; i < size; i++)
	{
			sym_set(s, (i + size / 2) % size, i,  1);
	}
	return s;
}


sym* code_in_place_transpose(sym* s)
{
	sym* a = sym_transpose(s);
	free(s);
	return a;
}

bool code_stabiliser_commutes(sym* a){

	sym* tran = sym_transpose(a);
	sym* mult = sym_multiply(a, tran);

	sym_free(tran);

	for (int i = 0; i < mult->mem_size; i++)
	{
		if (mult->matrix[i] != (char)0)
		{
			sym_free(mult);
			return false;
		}
	}

	sym_free(mult);
	return true;
}


void code_commutation_print(sym* pauli_string, sym* code)
{
	for (uint32_t i = 0; i < code->height; i++)
	{
			sym* tmp = sym_create(1, code->length);
			
			sym_row_copy(tmp, code, 0, i);
			
			printf("%u\t", sym_row_commutes(pauli_string, code, 0, i));
			sym_print_pauli(tmp);

			sym_free(tmp);
	}
	return;
}

#endif