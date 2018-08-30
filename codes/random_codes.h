#ifndef RANDOM_CODES
#define RANDOM_CODES

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../sym.h"
#include "../logical.h"
#include "../misc/rand_bytes.h"

// STRUCT OBJECTS ----------------------------------------------------------------------------------------

/*
	random_code_return:
	A struct that contains both the code and the logicals for a random code
	:: sym* code :: The random QECC
	:: sym* logicals :: The associated logical operators
*/
typedef struct 
{
	sym* code;
	sym* logicals;
	double p_correction;
} random_code_return;

// FUNCTION DECLARATIONS ----------------------------------------------------------------------------------------
/* 
    code_random:
	Creates a random QECC code with the specified properties
	:: const unsigned n :: The number of qubits
	:: const unsigned k :: The number of logical operators
	:: const unsigned r :: The distance of the code (should be minimum 3)
	Returns a 'random_code_return' struct containing a pointer to the code and another to the logicals
*/
random_code_return code_random(const unsigned n, const unsigned k, const unsigned r);

/* 
    random_code_test:
	Tests the validity of a random code, currently not completely implemented
	:: const sym* code :: The code to test
	:: const sym* logicals :: The associated logicals
	No return object
*/
void random_code_test(const sym* code, const sym* logicals);

// FUNCTION DEFINITIONS ----------------------------------------------------------------------------------------

//-----------------------------------------------------------
// Random Codes
//-----------------------------------------------------------
// Based off Nielsen and Chuang pg 470

/*						
* 		    r 		[	I 	A_1 A_2 |	B 	0 	C 	]
*		n - k - r 	[	0	0	0	|	D 	I 	E 	]
*		
*	Set everything to random, only anti-commutations between D and I,  A_2 and E, and A_1 and I, resolve these using D and B
* 	D resolves the commutation relations between the last n-k-r stabilisers and the first r, while B resolves commutation relations 
*	within the first r stabilisers. In each case the first I block in the top left can be leveraged against B and D for predicable
* 	relations. If stabiliser j anti-commutes with stabiliser i, simply flip the [i, j]th element in the B or the D block as appropriate 
*
*	code_random:
*	Creates a random QECC code with the specified properties
*	:: const unsigned n :: The number of qubits
*	:: const unsigned k :: The number of logical operators
*	:: const unsigned r :: The distance of the code (should be minimum 3)
*	Returns a 'random_code_return' struct containing a pointer to the code and another to the logicals
*/
random_code_return code_random(const unsigned n, const unsigned k, const unsigned r)
{
	srand(time(NULL));

	sym* code = sym_create(n - k, 2 * n);
	sym* logicals = sym_create(2 * n, 2 * k);

	random_code_return return_object;
	return_object.code = code;
	return_object.logicals = logicals;

	rand_bytes random_gen = rand_bytes_create(code->mem_size + 6); // This should be more than we need

	// The Identity element in the top left
	/*						
	* 		[	I 	0	0	|	0 	0 	0 	]
	*		[	0	0	0	|	0 	0 	0 	]
	*/
	for (size_t i = 0; i < r; i++)
	{
		sym_set(code, i, i, 1);
	}

	// The Identity element on the right
	/*						
	* 		[	I 	0	0	|	0 	0 	0 	]
	*		[	0	0	0	|	0 	I 	0 	]
	*/
	unsigned eye_b_start_x = n + r;
	unsigned eye_b_start_y = r;
	unsigned eye_b_end_x = 2 * n - k;
	unsigned eye_b_end_y = n - k;
	for (size_t i = 0; i < n - k - r; i++)
	{
		sym_set(code, i + r, i + n + r, 1);
	}

	
	// The "A_1 Element"
	/*						
	* 		[	I 	A_1	0	|	0 	0 	0 	]
	*		[	0	0	0	|	0 	I 	0 	]
	*/
	unsigned A1_start_x = r;
	unsigned A1_start_y = 0;
	unsigned A1_end_x = n - k;
	unsigned A1_end_y = r;

	BYTE* seed = rand_bytes_consume(random_gen, (A1_end_x - A1_start_x) * (A1_end_y - A1_start_y) / 8 + 1);

	for (size_t i = A1_start_y; i < A1_end_y; i++)
	{
		for (size_t j = A1_start_x; j < A1_end_x; j++)
		{
			sym_set(code, i, j, !!(*(seed + ((i * (A1_end_x - A1_start_x)) + j) / 8) & 1 << (((i * (A1_end_x - A1_start_x)) + j) % 8)));	
		}
	}

	// The "A_2 Element"
	/*						
	* 		[	I 	A_1	A_2	|	0 	0 	0 	]
	*		[	0	0	0	|	0 	I 	0 	]
	*/
	unsigned A2_start_x = n - k;
	unsigned A2_start_y = 0;
	unsigned A2_end_x = n;
	unsigned A2_end_y = r;

	seed = rand_bytes_consume(random_gen, (A2_end_x - A2_start_x) * (A2_end_y - A2_start_y) / 8 + 1);

	for (size_t i = A2_start_y; i < A2_end_y; i++)
	{
		for (size_t j = A2_start_x; j < A2_end_x; j++)
		{
			sym_set(code, i, j, !!(*(seed + ((i * (A2_end_x - A2_start_x)) + j) / 8) & 1 << (((i * (A2_end_x - A2_start_x)) + j) % 8)));	
		}
	}

	// The "C Element"
	/*						
	* 		[	I 	A_1	A_2	|	0 	0 	C 	]
	*		[	0	0	0	|	0 	I 	0 	]
	*/
	unsigned C_start_x = 2 * n - k;
	unsigned C_start_y = 0;
	unsigned C_end_x = 2 * n;
	unsigned C_end_y = r;

	seed = rand_bytes_consume(random_gen, (C_end_x - C_start_x) * (C_end_y - C_start_y) / 8 + 1);

	for (size_t i = C_start_y; i < C_end_y; i++)
	{
		for (size_t j = C_start_x; j < C_end_x; j++)
		{
			sym_set(code, i, j, !!(*(seed + ((i * (C_end_x - C_start_x)) + j) / 8) & 1 << (((i * (C_end_x - C_start_x)) + j) % 8)));	
		}
	}

	// The "E Element"
	/*						
	* 		[	I 	A_1	A_2	|	0 	0 	C 	]
	*		[	0	0	0	|	0 	I 	E 	]
	*/
	unsigned E_start_x = 2 * n - k;
	unsigned E_start_y = r;
	unsigned E_end_x = 2 * n;
	unsigned E_end_y = n - k;

	seed = rand_bytes_consume(random_gen, (E_end_x - E_start_x) * (E_end_y - E_start_y) / 8 + 1);

	for (size_t i = E_start_y; i < E_end_y; i++)
	{
		for (size_t j = E_start_x; j < E_end_x; j++)
		{
			sym_set(code, i, j, !!(*(seed + ((i * (E_end_x - E_start_x)) + j) / 8) & 1 << (((i * (E_end_x - E_start_x)) + j) % 8)));
		}
	}

	// Set the diagonal of the E block to all 0's
	for (size_t i = E_start_y; i < E_start_y + k; i++)
	{
		sym_set(code, i, E_start_x + i, 0);
	}

	// Setting D and B to the same portion of the bit stream requires that we first find which of the two is larger
	unsigned B_start_x = n;
	unsigned B_start_y = 0;
	unsigned B_end_x = n + r;
	unsigned B_end_y = r;

	unsigned D_start_x = n;
	unsigned D_start_y = r;
	unsigned D_end_x = n + r;
	unsigned D_end_y = n - k;


	if ((B_end_x - B_start_x) * (B_end_y - B_start_y) > (D_end_x - D_start_x) * (D_end_y - D_start_y))
	{
		seed = rand_bytes_consume(random_gen, (B_end_x - B_start_x) * (B_end_y - B_start_y) / 8 + 1);
	} 
	else
	{
		seed = rand_bytes_consume(random_gen, (D_end_x - D_start_x) * (D_end_y - D_start_y) / 8 + 1);
	}

	// The D Element
	/*						
	* 		[	I 	A_1	A_2	|	0 	0 	C 	]
	*		[	0	0	0	|	D 	I 	E 	]
	*/
	for (size_t i = D_start_y; i < D_end_y; i++)
	{
		for (size_t j = D_start_x; j < D_end_x; j++)
		{
			sym_set(code, i, j, !!(*(seed + ((i * (D_end_x - D_start_x)) + j) / 8) & 1 << (((i * (D_end_x - D_start_x)) + j) % 8)));	
		}
	}

	// Now fix bits to ensure commutation between the last n - k - r stabilisers and the first r stabilisers,
	// As the last n - k - r all only contain Z elements, they are guaranteed to commute with each other. 
	// The identity block in the top left of the X elements ensures that in order to guarantee commutation we only need to flip
	// The corresponding [i, i]th bit in the D block.
	// The identity elements also guarantee the uniqueness of the stabilisers
	for (size_t i = D_start_y; i < D_end_y; i++)
	{
		for (size_t j = A1_start_y; j < A1_end_y; j++)
		{
			// Determine if the rows anti-commute
			unsigned anti_commutes = 0;
			for (size_t k = 0; k < n; k++)
			{
				anti_commutes += sym_get(code, i, n + k) && sym_get(code, j, k);
			}
			// If they do, then flip the corresponding bit 
			if (anti_commutes % 2)
			{
				sym_set(code, i, n + j, !sym_get(code, i, n + j));
			}
		}
	}

	// The B element
	/*						
	* 		[	I 	A_1	A_2	|	B 	0 	C 	]
	*		[	0	0	0	|	D 	I 	E 	]
	*/
	for (size_t i = B_start_y; i < B_end_y; i++)
	{
		for (size_t j = B_start_x; j < B_end_x; j++)
		{
			sym_set(code, i, j, !!(*(seed + ((i * (B_end_x - B_start_x)) + j) / 8) & 1 << (((i * (B_end_x - B_start_x)) + j) % 8)));
		}
	}

	// Now fix bits to ensure commutation between the last n - k - r stabilisers and the first r stabilisers,
	// As the last n - k - r all only contain Z elements, they are guaranteed to commute with each other. 
	// The identity block in the top left of the X elements ensures that in order to guarantee commutation we only need to flip
	// The corresponding [i, i]th bit in the B block.
	for (size_t i = B_start_y; i < B_end_y; i++)
	{
		for (size_t j = A1_start_y; j < A1_end_y; j++)
		{
			// Determine if the rows anti-commute
			unsigned anti_commutes = 0;
			for (size_t k = 0; k < n; k++)
			{
				anti_commutes += sym_get(code, i, n + k) && sym_get(code, j, k);
			}
			// If they do, then flip the corresponding bit 
			if (anti_commutes % 2)
			{
				sym_set(code, i, n + j, !sym_get(code, i, n + j));
			}
		}
	}

	// Next, the logicals
	/*
	* [0 | 0]
	* [E | 0]
	* [I | 0]
	* -------
	* [C | A_2]
	* [0 | 0]
	* [0 | I]
	*/
	// Logical X's
	// This is the E block
	for (size_t i = E_start_y; i < E_end_y; i++)
	{
		for (size_t j = 0; j < k; j++)
		{
			sym_set(logicals, i, j, sym_get(code, i, E_start_x + j));
		}
	}

	// Identity Matrix
	for (size_t i = 0; i < n - E_end_y; i++)
	{
		sym_set(logicals, E_end_y + i, i, 1);
	}

	// This is the C block
	for (size_t i = 0; i < r; i++)
	{
		for (size_t j = 0; j < k; j++)
		{
			sym_set(logicals, i + n, j, sym_get(code, i, 2 * n - k + j));
		}
	}

	// Logical Z's
	// This is the A2 block
	for (size_t i = A2_start_y; i < A2_end_y; i++)
	{
		for (size_t j = A2_start_x; j < A2_end_x; j++)
		{
			sym_set(logicals, i + n, j - A2_start_x + k, sym_get(code, i, j));
		}
	}

	// Other Identity matrix
	for (size_t i = 0; i < k; i++)
	{
		sym_set(logicals, 2 * n - k + i, k + i, 1);
	}

	rand_bytes_free(random_gen);
	return return_object;
}

void random_code_test(const sym* code, const sym* logicals)
{
	// Check that it all works...
	sym* test = sym_create(1, code->height);
	for (size_t i = 0; i < code->height; i++)
	{
		sym_row_copy(test, code, 0, i);
		sym* commutes = logical_error(logicals, test);
		for (size_t i = 0; i < commutes->length; i++)
		{
			if (1 == i)
			{
				printf("DOES NOT COMMUTE WITH LOGICALS!\n");
			}
		}
		sym_free(commutes);
	} 
	sym_free(test);
}

#endif