#ifndef TABLEAU
#define TABLEAU

// This is a "hidden" column containing the phase information
#define TABLEAU_PHASE_COL (tableau->length)

#include "destabilisers.h"

/*
 * tableau_create
 * Creates a tableau representation of the stabilisers and destabilisers as per 10.1103/PhysRevA.70.052328
 * :: const sym* code :: The sym object for the error correcting code
 * :: const sym* logicals :: The sym object for the associated logicals
 * :: const sym** destabilisers :: The array of sym objects for the associated destabilisers
 * Returns a sym* object containing the tableau
 */
sym* tableau_create(const sym* code, const sym* logicals);

/*
 * tableau_extend: 
 * Extends the tableau to fill the full 2n by 2n matrix
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

void tableau_rank_deficient(sym* tableau);

/*
 * tableau_cnot
 * Performs a CNOT gate on the tableau
 * :: sym* tableau :: The tableau object being operated on
 * :: const unsigned control :: The control qubit in the CNOT gate
 * :: const unsigned target :: The target qubit in the CNOT gate
 * Operations are performed in place, nothing is returned
 */
void tableau_cnot(sym* tableau, const unsigned control, const unsigned target);
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
sym* tableau_create(const sym* code, const sym* logicals)
{

	sym** destabilisers = destabilisers_generate(code, logicals);

	// Could not find destabilisers
	if (NULL == destabilisers)
	{
		return NULL;
	}

	// One extra column to track the phases
	sym* tableau = sym_create(code->length, code->length);


	// Create the tableau object
	for (size_t i = 0; i < code->height; i++)
	{
		sym_row_copy(tableau, destabilisers[i], i, 0);	
		
		sym_row_copy(tableau, code, i + tableau->height/2, i);
	
	}

	// Add the additional stabilisers and destabilisers
	tableau_extend(tableau, logicals, code->height);

	// Free the destabilisers
	for (uint32_t i = 0; i < code->height; i++)
	{
		sym_free(destabilisers[i]);
	}
	free(destabilisers);

	// Add a phase tracking column
	//tableau_add_phase_col(tableau);

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
	sym* t = sym_transpose(logicals);

	for (size_t i = 0; i < t->height / 2; i++)
	{
		// Copy the Z to the destabilisers
		sym_row_copy(tableau, t, tableau->length/2 - (i+1), i);

		// Copy the X to the stabilisers
		sym_row_copy(tableau, t, tableau->length - (i+1), i  + t->height/2);
	}

	sym_free(t);
	return;
	// If you want to search for a low weight stabiliser use this, however it may not be linearly independent
	// And if it's not then this isn't going to work
	/*if (n_stabilisers < tableau->height / 2)
	{
		bool result = tableau_backtrack_stabiliser(tableau, logicals, n_stabilisers);

		if (result == false) 
		{
			printf("Tableau could not be completed!\n");
		}
	}*/
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

	// Check that it anti-commutes with pairwise with the appropriate stabiliser
	if (0 == sym_row_commutes(tableau, destabiliser_candidate, tableau->length / 2 +  n_stabiliser, 0))
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
void tableau_cnot(sym* tableau, const unsigned control, const unsigned target)
{
	if (control > tableau->length / 2 
		|| target > tableau->length / 2)
	{
		printf("Invalid operation attempted\n");
	}

	for (size_t i = 0; i < tableau->height; i++)
	{
		// Set the target column to the control column XORed with the target column for both X and Z sub-matricies
		sym_set(tableau, i, target, sym_get(tableau, i, target ) ^ sym_get(tableau, i, control));

		sym_set(tableau, i, control + tableau->length / 2, sym_get(tableau, i, target  + tableau->length / 2 ) ^ sym_get(tableau, i, control + tableau->length / 2));
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

/*
 * tableau_add_phase_col
 * Adds a column to track phase information on the tableau
 * :: sym* tableau :: The tableau object being operated on
 * Operations are performed in place, nothing is returned
 * You don't really need this if you don't care about the phases
 */
void tableau_add_phase_col(sym* tableau)
{
	// Create a new tableau
	sym* tableau_new = sym_create(tableau->height, tableau->length + 1);
	
	for (uint32_t i = 0; i < tableau->height; i++)
	{
		for (uint32_t j = 0; j < tableau->length; j++)
		{
			sym_set(tableau_new, i, j, sym_get(tableau, i, j));
		}
	}

	// Swap the old matrix to the new sym object and free it
	BYTE* old_matrix = tableau->matrix;
	tableau->matrix = tableau_new->matrix;
	tableau_new->matrix = old_matrix;
	sym_free(tableau_new);

	return;
}

#endif
