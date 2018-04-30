#ifndef ENCODING
#define ENCODING

#include "circuit.h"
#include "destabilisers.h"
#include "tableau.h"
#include "sym_debug.h"


void encoding_rank_deficient(sym* tableau, circuit* encode, gate* cnot, gate* hadamard);

circuit* encoding_circuit(const sym* code, const sym* logicals, sym** destabilisers, gate* cnot, gate* hadamard, gate* phase)
{
	circuit* encode = circuit_create(code->length / 2);

	// Get a tableau
	sym* tableau = tableau_create(code, logicals, destabilisers);

	// Could not create tableau
	if (NULL == tableau)
	{
		return NULL;
	}

	encoding_rank_deficient(tableau, encode, cnot, hadamard);

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
					circuit_add_gate(encode, cnot, j, i);
					tableau_cnot(tableau, j, i);
					pivot_found = true;
				}
			}	
		}
		
		// Use i,i to pivot and eliminate the rest of the row
		for (size_t j = 0; j < tableau->length / 2; j++)
		{
			if (i != j && 1 == sym_get(tableau, i + tableau->height / 2, j))
			{
				circuit_add_gate(encode, cnot, i, j);
				tableau_cnot(tableau, i, j);
			}
		}
	}

	// Find S_Z + A = M M^T.
	for (size_t i = 0; i < tableau->height / 2; i++)
	{
		// Ensure that [i,i] is 1
		if ( 0 == sym_get(tableau, i + tableau->height / 2, i + tableau->height / 2) )
		{
			circuit_add_gate(encode, phase, i);
			tableau_phase(tableau, i);
		}
		
		// Eliminate the upper triangular section of the matrix
		for (size_t j = i + 1; j < tableau->length / 2; j++)
		{	
			if ( 1 == sym_get(tableau, i + tableau->height / 2, j + tableau->height / 2) )
			{
				circuit_add_gate(encode, cnot, j, i);
				tableau_cnot(tableau, j, i);
			}
		}
	}

	// Phase on all qubits
	for(size_t i = 0; i < tableau->length / 2; i++)
	{
		circuit_add_gate(encode, phase, i);
		tableau_phase(tableau, i);
	}

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
					circuit_add_gate(encode, cnot, j, i);
					tableau_cnot(tableau, j, i);
					pivot_found = true;
				}
			}	
		}
		
		// Use i,i to pivot and eliminate the rest of the row
		for (size_t j = 0; j < tableau->length / 2; j++)
		{
			if (i != j && 1 == sym_get(tableau, i + tableau->height / 2, j))
			{
				circuit_add_gate(encode, cnot, i, j);
				tableau_cnot(tableau, i, j);
			}
		}
	}

	// Hadamards on all qubits
	for(size_t i = 0; i < tableau->length / 2; i++)
	{
		circuit_add_gate(encode, hadamard, i);
		tableau_hadamard(tableau, i);
	}


	// Find D_Z + A = M M^T.
	for (size_t i = 0; i < tableau->height / 2; i++)
	{
		// Ensure that [i,i] is 1
		if ( 0 == sym_get(tableau, i, i + tableau->height / 2) )
		{
			circuit_add_gate(encode, phase, i);
			tableau_phase(tableau, i);
		}

		for (size_t j = i + 1; j < tableau->length / 2; j++)
		{	
			// Ensure that [i,i] is 1
			if ( 1 == sym_get(tableau, i, j + tableau->height / 2) )
			{

					circuit_add_gate(encode, cnot, j, j);
					tableau_cnot(tableau, j, i);
			}
		}
	}

	// Phase on all qubits
	for(size_t i = 0; i < tableau->length / 2; i++)
	{
		circuit_add_gate(encode, phase, i);
		tableau_phase(tableau, i);
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
					circuit_add_gate(encode, cnot, j, i);
					tableau_cnot(tableau, j, i);
					pivot_found = true;
				}
			}	
		}
		
		// Use i,i to pivot and eliminate the rest of the row
		for (size_t j = 0; j < tableau->length / 2; j++)
		{
			if (i != j && 1 == sym_get(tableau, i, j))
			{
				circuit_add_gate(encode, cnot, i, j);
				tableau_cnot(tableau, i, j);
			}
		}
	}

	sym_free(tableau);
	return encode;
}


void encoding_rank_deficient(sym* tableau, circuit* encode, gate* cnot, gate* hadamard)
{
	
	unsigned max_rank = 0;

	// Check that the tableau is full rank in S_X
	for (size_t i = 0; i < tableau->height / 2 && max_rank == 0; i++)
	{
		bool rank_deficient = true;
		for (size_t j = 0; j < tableau->length / 2 && rank_deficient; j++)
		{
			if (1 == sym_get(tableau, i + tableau->height / 2, j))
			{
				rank_deficient = false;
			}
		}

		if (true == rank_deficient)
		{
			max_rank = i;
		}

	}

	if (max_rank == 0)
	{
		return;
	}

	// CNOT S_Z subset k to Identity via Gaussian elimination
	for (size_t i = max_rank; i < tableau->height / 2; i++)
	{
		// Ensure that [i,i] is 1
		if ( 0 == sym_get(tableau, i + tableau->height / 2, i + tableau->height / 2) )
		{
			bool pivot_found = false;
			for (size_t j = 0; j < tableau->length / 2 && !pivot_found; j++)
			{
				if (1 == sym_get(tableau, i + tableau->height / 2, j + tableau->length / 2)) 
				{
					circuit_add_gate(encode, cnot, i, j);
					tableau_cnot(tableau, i, j);
					pivot_found = true;
				}
			}	
		}
		
		
		// Use i,i to pivot and eliminate the rest of the row
		for (size_t j = max_rank; j < tableau->length / 2; j++)
		{
			if (i != j && 1 == sym_get(tableau, i + tableau->height / 2, j + tableau->length / 2))
			{
				circuit_add_gate(encode, cnot, j, i);
				tableau_cnot(tableau, j, i);
			}
		}
	}
	

	// Hadamard on k to map the identity over to X
	for (size_t i = max_rank; i < tableau->height / 2; i++)
	{
		circuit_add_gate(encode, hadamard, i);
		tableau_hadamard(tableau, i);
	}
	return;
}




#endif