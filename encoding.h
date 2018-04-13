#ifndef ENCODING
#define ENCODING

#include "circuit.h"
#include "destabilisers.h"
#include "tableau.h"
#include "sym_debug.h"


sym* encoding_circuit(const sym* code, const sym* logicals, sym** destabilisers)
{
	// Get a tableau
	sym* tableau = tableau_create(code, logicals, destabilisers);
	check_commutation(tableau);
	
	printf("#####\n");

	tableau_rank_deficient(tableau);
	
	check_commutation(tableau);
	return tableau;
	// CNOT S_X to Identity via Gaussian elimination
	for (size_t i = 0 ; i < tableau->height / 2; i++)
	{
		// Ensure that [i,i] is 1
		if ( 0 == sym_get(tableau, i + tableau->height / 2, i) )
		{
			bool pivot_found = false;
			for (size_t j = i; j < tableau->length / 2 && !pivot_found; j++)
			{
				if (1 == sym_get(tableau, i + tableau->height / 2, j)) 
				{
					tableau_CNOT(tableau, j, i);
					pivot_found = true;
				}
			}	
		}
		
		// Use i,i to pivot and eliminate the rest of the row
		for (size_t j = 0; j < tableau->length / 2; j++)
		{
			if (i != j && 1 == sym_get(tableau, i + tableau->height / 2, j))
			{
				tableau_CNOT(tableau, i, j);
			}
		}
	}


	return tableau;

	// Find S_Z + A = M M^T
	// On the other hand, these bloody matricies aren't symmetric, let's just phase in the identity...
	// Or skip this bit for now...
	/*for (size_t i = 0 ; i < tableau->height / 2; i++)
	{
		// Ensure that [i,i] is 1
		if ( 0 == sym_get(tableau, i + tableau->height / 2, i + tableau->height / 2) )
		{
			tableau_phase(tableau, i);
		}
	}*/

	// Extend the tableau to full rank in D_X
	for (size_t i = 0; i < tableau->height / 2; i++)
	{
		bool rank_deficient = true;
		for (size_t j = 0; j < tableau->length / 2 && rank_deficient; j++)
		{
			if (1 == sym_get(tableau, i, j))
			{
				rank_deficient = false;
			}
		}

		printf("%d\n", (int)rank_deficient);

		// If it's not rank deficient, this loop will be skipped
		for (size_t j = i; j < tableau->length / 2 && rank_deficient; j++)
		{
			if (1 == sym_get(tableau, i, j + tableau->length / 2))
			{
				printf("Swapped %d %d\n", (int)i, (int)j);
				rank_deficient = false;
				tableau_hadamard(tableau, j);
			}
		}
	}

	// CNOT D_X to Identity via Gaussian elimination
	for (size_t i = 0 ; i < tableau->height / 2; i++)
	{
		// Ensure that [i,i] is 1
		if ( 0 == sym_get(tableau, i, i) )
		{
			bool pivot_found = false;
			for (size_t j = i; j < tableau->length / 2 && !pivot_found; j++)
			{
				if (1 == sym_get(tableau, i, j)) 
				{
					tableau_CNOT(tableau, j, i);
					pivot_found = true;
				}
			}	
		}
		
		// Use i,i to pivot and eliminate the rest of the row
		for (size_t j = 0; j < tableau->length / 2; j++)
		{
			if (i != j && 1 == sym_get(tableau, i, j))
			{
				tableau_CNOT(tableau, i, j);
			}
		}
	}


	return tableau;
}




#endif