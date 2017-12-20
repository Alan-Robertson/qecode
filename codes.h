#ifndef CODES
#define CODES

#include "sym.h"
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
		0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1
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
		0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 
		1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0
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
		1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 					// XXXXXXXXX
		0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1  					// ZZZZZZZZZ
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
		0, 0, 1, 0 																// ZI
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
		0, 1, 0, 0,																// IX
		0, 0, 1, 1																// ZZ
	};
	sym* s = sym_create_valued(2, 4, code);
	sym* t = sym_transpose(s);
	sym_free(s);
	return t;
}

sym* code_five_qubit()
{
	unsigned code[] = {
		1, 0, 0, 0, 1, 0, 1, 0, 1, 0,
		1, 1, 0, 0, 0, 0, 0, 1, 0, 1,
		0, 1, 1, 0, 0, 1, 0, 0, 1, 0,
		0, 0, 1, 1, 0, 0, 1, 0, 0, 1			
	};
	return sym_create_valued(4, 10, code);
}

sym* code_five_qubit_logicals()
{
	unsigned code[] = {
		1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 1, 1, 1, 1, 1
	};
	sym* s = sym_create_valued(2, 10, code);
	sym* t = sym_transpose(s);
	sym_free(s);
	return t;
}

sym* code_asymmetric_five()
{
	unsigned code[] = {
		0, 0, 0, 0, 0, 0, 1, 1, 0, 0,
		0, 0, 0, 0, 0, 1, 0, 1, 0, 0,
		0, 0, 0, 1, 1, 0, 0, 0, 0, 0,
		0, 0, 0, 1, 0, 0, 0, 1, 0, 0			
	};
	return sym_create_valued(4, 10, code);
}

sym* code_asymmetric_five_logicals()
{
	unsigned code[] = {
		0, 0, 0, 1, 1, 1, 1, 1, 0, 0,
		1, 1, 1, 0, 0, 0, 0, 0, 1, 1
	};
	sym* s = sym_create_valued(2, 10, code);
	sym* t = sym_transpose(s);
	sym_free(s);
	return t;
}

//-----------------------------------------------------------
// Random Codes
//-----------------------------------------------------------
// Based off Nielsen and Chuang pg 470

/*						
* 		    r 		[	I 	A_1 A_2 |	B 	0 	C 	]
*		n - k - r 	[	0	0	0	|	D 	I 	E 	]
*		
*	Set everything to random, only anti-commutations between A_2 and E, resolve that using D
*/
sym** code_random(const unsigned n, const unsigned k, const unsigned r)
{
	// Note this is only good up to (n - k - r) <= 8
	// Otherwise it gets a bit trickier, probably going to need a proper setup for this ;;
	unsigned long long seed = ((unsigned long long)rand()) + (((unsigned long long)rand()) << 16) + (((unsigned long long)rand()) << 32) + (((unsigned long long)rand()) << 48);

	sym* code = sym_create();
	sym* logicals = sym_create();

	// An Identity element in the top left
	for (size_t i = 0; i < r; i++)
	{
		sym_set(code, i, i, 1);
	}

	// The Identity element on the right
	unsigned eye_b_start_x = n + r;
	unsigned eye_b_start_y = r;
	unsigned eye_b_end_x = 2 * n - k;
	unsigned eye_b_end_y = n - k;
	for (int i = 0; i < n - k - r; i++)
	{
		sym_set(code, i + n + r, i + r, 1);
	}

	seed = ((unsigned long long)rand()) + (((unsigned long long)rand()) << 16) + (((unsigned long long)rand()) << 32) + (((unsigned long long)rand()) << 48);
	// The "A_1 Element"
	unsigned A1_start_x = r;
	unsigned A1_start_y = 0;
	unsigned A1_end_x = n - k;
	unsigned A1_end_y = r;
	for (size_t i = A1_start_y; i < A1_end_y; i++)
	{
		for (size_t j = A1_start_x; j < A1_end_x; j++)
		{
			sym_set(code, i, j, seed % 2);
			seed >>= 1;	
		}
	}

	seed = ((unsigned long long)rand()) + (((unsigned long long)rand()) << 16) + (((unsigned long long)rand()) << 32) + (((unsigned long long)rand()) << 48);
	// The "A_2 Element"
	unsigned A2_start_x = n - k;
	unsigned A2_start_y = 0;
	unsigned A2_end_x = n;
	unsigned A2_end_y = r;
	for (size_t i = A2_start_y; i < A2_end_y; i++)
	{
		for (size_t j = A2_start_x; j < A2_end_x; j++)
		{
			sym_set(code, i, j, seed % 2);
			seed >>= 1;	
		}
	}

	seed = ((unsigned long long)rand()) + (((unsigned long long)rand()) << 16) + (((unsigned long long)rand()) << 32) + (((unsigned long long)rand()) << 48);
	// The "C Element"
	unsigned C_start_x = 2 * n - k;
	unsigned C_start_y = 0;
	unsigned C_end_x = 2 * n;
	unsigned C_end_y = r;
	for (size_t i = C_start_y; i < C_end_y; i++)
	{
		for (size_t j = C_start_x; j < C_end_x; j++)
		{
			sym_set(code, i, j, seed % 2);
			seed >>= 1;	
		}
	}

	seed = ((unsigned long long)rand()) + (((unsigned long long)rand()) << 16) + (((unsigned long long)rand()) << 32) + (((unsigned long long)rand()) << 48);
	// The "E Element"
	unsigned E_start_x = 2 * n - k;
	unsigned E_start_y = r;
	unsigned E_end_x = 2 * n;
	unsigned E_end_y = n - k;
	for (size_t i = E_start_x; i < E_end_x; i++)
	{
		for (size_t j = E_start_y; j < E_end_y; j++)
		{
			sym_set(code, i, j, seed % 2);
			seed >>= 1;	
		}
	}

	seed = ((unsigned long long)rand()) + (((unsigned long long)rand()) << 16) + (((unsigned long long)rand()) << 32) + (((unsigned long long)rand()) << 48);
	unsigned long long seed_cpy = seed;

	// The B element
	unsigned B_start_x = n;
	unsigned B_start_y = 0;
	unsigned B_end_x = n + r;
	unsigned B_end_y = r;
	for (size_t i = B_start_x; i < B_end_x; i++)
	{
		for (size_t j = B_start_y; j < B_end_y; j++)
		{
			// If Antcommutes, then 1, else 0
			sym_set(code, i, j, seed % 2);
			seed >>= 1;
		}
	}

	// The D Element
	unsigned D_start_x = n;
	unsigned D_start_y = r;
	unsigned D_end_x = n + r;
	unsigned D_end_y = n - k;
	for (size_t i = D_start_x; i < D_end_x; i++)
	{
		for (size_t j = D_start_y; j < D_end_y; j++)
		{
			sym_set(code, i, j, seed % 2);
			seed >>= 1;	
		}
	}

	free(seed);
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

#endif