#ifndef TABLEAU
#define TABLEAU

#include "destabilisers.h"

/*
 * tableau_create
 * Creates a tableau representation of the stabilisers and destabilisers as per 10.1103/PhysRevA.70.052328
 * :: const sym* code :: The sym object for the error correcting code
 * :: const sym* logicals :: The sym object for the associated logicals
 * :: const sym** destabilisers :: The array of sym objects for the associated destabilisers
 * Returns a sym* object containing the tableau
 */
sym* tableau_create(const sym* code, const sym* logicals, sym** destabilisers);

/*
 * tableau_extend: 
 * Extends the tableau to fill the full 2n by 2n matrix, this function is a wrapper for the backtrack methods 
 * used to find the stabilisers and associated destabilisers in turn
 * :: sym* tableau :: The current tableau object containing the stabilisers and destabilisers 
 * :: sym* logicals :: The logicals associated with the code
 * :: const unsigned n_stabilisers :: The current number of stabilisers
 * Operation is performed in place, nothing returned
 */
void tableau_extend(sym* tableau, const sym* logicals, const unsigned n_stabilisers);

bool tableau_backtrack_stabiliser(
	sym* tableau,
	const sym* logicals, 
	const unsigned stabiliser);

bool tableau_backtrack_destabiliser(
	sym* tableau,
	const sym* logicals, 
	const unsigned stabiliser);

bool tableau_is_stabiliser(
	const sym* stabiliser_candidate,
	const sym* tableau,
	const sym* logicals,
	const unsigned n_stabiliser);
bool tableau_is_destabiliser(
	const sym* destabiliser_candidate,
	const sym* tableau,
	const sym* logicals,
	const unsigned n_stabiliser);

/*
 * tableau_CNOT
 * Performs a CNOT gate on the tableau
 * :: sym* tableau :: The tableau object being operated on
 * :: const unsigned control :: The control qubit in the CNOT gate
 * :: const unsigned target :: The target qubit in the CNOT gate
 * Operations are performed in place, nothing is returned
 */
void tableau_CNOT(sym* tableau, const unsigned control, const unsigned target);
/*
 * tableau_hadamard
 * Performs a Hadamard gate on the tableau
 * :: sym* tableau :: The tableau object being operated on
 * :: const unsigned target :: The target qubit in the Hadamard gate
 * Operations are performed in place, nothing is returned
 */
void tableau_hadamard(sym* tableau, const unsigned target);
/*
 * tableau_phase
 * Performs a phase gate on the tableau
 * :: sym* tableau :: The tableau object being operated on
 * :: const unsigned target :: The target qubit in the phase gate
 * Operations are performed in place, nothing is returned
 */
void tableau_phase(sym* tableau, const unsigned target);

/*
 * tableau_create
 * Creates a tableau representation of the stabilisers and destabilisers as per 10.1103/PhysRevA.70.052328
 * :: const sym* code :: The sym object for the error correcting code
 * :: const sym* logicals :: The sym object for the associated logicals
 * :: const sym** destabilisers :: The array of sym objects for the associated destabilisers
 * Returns a sym* object containing the tableau
 */
sym* tableau_create(const sym* code, const sym* logicals, sym** destabilisers)
{
	// If the destabilisers havent been generated, generate them
	bool free_destabilisers = false;
	if (NULL == destabilisers)
	{
		destabilisers = destabilisers_generate(code, logicals);
		free_destabilisers = true;
	}


	sym* tableau = sym_create(code->length, code->length);

	// Create the tableau object
	for (size_t i = 0; i < code->height; i++)
	{
		//sym_row_copy(tableau, destabilisers[i], i, 0);	
		
		sym_row_copy(tableau, code, i + tableau->height/2, i);

		
	}

	// Add the additional stabilisers and destabilisers
	tableau_extend(tableau, logicals, code->height);

	// If the destabilisers were generated within this function, free them
	if (free_destabilisers)
	{
		destabilisers_free(destabilisers, code->height);
	}
	return tableau;
}

/*
 * tableau_extend: 
 * Extends the tableau to fill the full 2n by 2n matrix, this function is a wrapper for the backtrack methods 
 * used to find the stabilisers and associated destabilisers in turn
 * :: sym* tableau :: The current tableau object containing the stabilisers and destabilisers 
 * :: sym* logicals :: The logicals associated with the code
 * :: const unsigned n_stabilisers :: The current number of stabilisers
 * Operation is performed in place, nothing returned
 */
void tableau_extend(sym* tableau, const sym* logicals, const unsigned n_stabilisers)
{
	if (n_stabilisers < tableau->height / 2)
	{
		bool result = tableau_backtrack_stabiliser(tableau, logicals, n_stabilisers);

		if (result == false) 
		{
			printf("Tableau could not be completed!\n");
		}
	}
}

bool tableau_backtrack_stabiliser(
	sym* tableau,
	const sym* logicals, 
	const unsigned stabiliser)
{
	// The base case 
	if (stabiliser == tableau->height / 2)
	{
		return true;
	}

	bool found_stabiliser = false;
	sym_iter* stabiliser_candidate = sym_iter_create(tableau->length);
	while(sym_iter_next(stabiliser_candidate) && !found_stabiliser)
	{
		// Check whether our proposal is a stabiliser
		if (tableau_is_stabiliser(
			stabiliser_candidate->state,
			tableau,
			logicals,
			stabiliser))
		{ // If it is a stabiliser, then copy it to our tableau
			sym_row_copy(tableau, stabiliser_candidate->state, tableau->height / 2 + stabiliser, 0);
			// Backtrack to find the associated destabiliser
			found_stabiliser = tableau_backtrack_destabiliser(
				tableau,
				logicals,
				stabiliser);
		}		
	}
	// Free up memory and return
	sym_iter_free(stabiliser_candidate);
	return found_stabiliser;
} 

bool tableau_backtrack_destabiliser(
	sym* tableau,
	const sym* logicals, 
	const unsigned stabiliser)
{
	// The base case occurs with the destabilisers
	bool found_destabiliser = false;
	sym_iter* destabiliser_candidate = sym_iter_create(tableau->length);
	while(sym_iter_next(destabiliser_candidate) && !found_destabiliser)
	{
		// Check whether our proposal is a destabiliser
		if (tableau_is_destabiliser(
			destabiliser_candidate->state,
			tableau,
			logicals,
			stabiliser))
		{ // If it is a destabiliser, then copy it to our tableau
			sym_row_copy(tableau, destabiliser_candidate->state,  stabiliser, 0);
			// Backtrack to find the next stabiliser
			found_destabiliser = tableau_backtrack_stabiliser(
				tableau,
				logicals,
				stabiliser + 1);
		}		
	}
	// Free up memory and return
	sym_iter_free(destabiliser_candidate);
	return found_destabiliser;
} 


/* 
 *  tableau_extend:
 *	Extends a code to a full 2n x 2n tableau
 *	:: sym* tableau :: The current tableau
 *	:: const sym* logicals ::  The logicals associated with the stabiliser code
 *	:: const n_stabiliser :: What stabiliser is currently being checked
 *	Returns true or false if the current candidate is or isn't a destabiliser
 */
bool tableau_is_stabiliser(
	const sym* stabiliser_candidate,
	const sym* tableau,
	const sym* logicals,
	const unsigned n_stabiliser)
{
	// Check that it commutes with the logicals
	for (size_t i = 0; i < logicals->length; i++)
	{
		if (1 == sym_row_column_commutes(stabiliser_candidate, logicals, 0, i))
		{
			return false;
		}
	}

	// Check that it commutes with the current stabilisers
	for (size_t i = tableau->length / 2; i < tableau->length / 2 + n_stabiliser; i++)
	{
		if (1 == sym_row_commutes(tableau, stabiliser_candidate, i, 0))
		{
			return false;
		}
	}

	// Check that it commutes with the current destabilisers
	for (size_t i = 0; i < n_stabiliser; i++)
	{
		if (1 == sym_row_commutes(tableau, stabiliser_candidate, i, 0))
		{
			return false;
		}
	}

	// All checks passed, it is a stabiliser, return true
	return true;
}

bool tableau_is_destabiliser(
	const sym* destabiliser_candidate,
	const sym* tableau,
	const sym* logicals,
	const unsigned n_stabiliser)
{
	
	// Check that it commutes with the logicals
	for (size_t i = 0; i < logicals->length; i++)
	{
		if (1 == sym_row_column_commutes(destabiliser_candidate, logicals, 0, i))
		{
			return false;
		}
	}

	// Check that it commutes with the current stabilisers
	for (size_t i = tableau->length / 2; i < tableau->length / 2 + n_stabiliser - 1; i++)
	{
		if (1 == sym_row_commutes(tableau, destabiliser_candidate, i, 0))
		{
			return false;
		}
	}

	// Check that it anti-commutes with pairwise with the appropriate destabiliser
	if (0 == sym_row_commutes(tableau, destabiliser_candidate, n_stabiliser, 0))
	{
		return false;
	}

	// Check that it commutes with the current destabilisers
	for (size_t i = 0; i < n_stabiliser; i++)
	{
		if (1 == sym_row_commutes(tableau, destabiliser_candidate, i, 0))
		{
			return false;
		}
	}

	// All checks passed, it is a destabiliser, return true
	return true;
}

/*
 * tableau_CNOT
 * Performs a CNOT gate on the tableau
 * :: sym* tableau :: The tableau object being operated on
 * :: const unsigned control :: The control qubit in the CNOT gate
 * :: const unsigned target :: The target qubit in the CNOT gate
 * Operations are performed in place, nothing is returned
 */
void tableau_CNOT(sym* tableau, const unsigned control, const unsigned target)
{
	if (control > tableau->length / 2 
		|| target > tableau->length / 2)
	{
		printf("Invalid operation attempted\n");
	}

	printf("%u %u\n", target, control);
	for (size_t i = 0; i < tableau->height; i++)
	{
		sym_set(tableau, i, target, sym_get(tableau, i, control) ^ sym_get(tableau,i,target));
		sym_set(tableau, i, target + tableau->length / 2, sym_get(tableau, i, control + tableau->length / 2) ^ sym_get(tableau, i, target + tableau->length/2));
	}
    return;
}

/*
 * tableau_hadamard
 * Performs a Hadamard gate on the tableau
 * :: sym* tableau :: The tableau object being operated on
 * :: const unsigned target :: The target qubit in the Hadamard gate
 * Operations are performed in place, nothing is returned
 */
void tableau_hadamard(sym* tableau, const unsigned target)
{
	for (size_t i = 0; i < tableau->height; i++)
	{
		sym_set(tableau, i, target, sym_get(tableau, i, target) ^ sym_get(tableau, i, target + tableau->length / 2));
		sym_set(tableau, i, target + tableau->length / 2, sym_get(tableau, i, target) ^ sym_get(tableau, i, target + tableau->length / 2));
		sym_set(tableau, i, target, sym_get(tableau, i, target) ^ sym_get(tableau, i, target + tableau->length / 2));
	}

    return;	
}

/*
 * tableau_phase
 * Performs a phase gate on the tableau
 * :: sym* tableau :: The tableau object being operated on
 * :: const unsigned target :: The target qubit in the phase gate
 * Operations are performed in place, nothing is returned
 */
void tableau_phase(sym* tableau, const unsigned target)
{
	for (size_t i = 0; i < tableau->height; i++)
	{
		sym_set(tableau, i, target + tableau->length / 2, sym_get(tableau, i, target) ^ sym_get(tableau, i, target + tableau->length / 2));
	}
	return;
}




#endif
