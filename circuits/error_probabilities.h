#ifndef ERROR_PROBABILITIES
#define ERROR_PROBABILITIES

#include "../sym_iter.h"

// ----------------------------------------------------------------------------------------
// FUNCTION DECLARATIONS
// ----------------------------------------------------------------------------------------

/*
 * error_probabilities_m 
 * Allocates an array of doubles for storing a probability distribution of pauli errors
 * :: const size_t n_qubits :: The number of qubits that this error probability distribution covers
 * Returns an array of zeros
 */
double* error_probabilities_m(const size_t n_qubits);

/*
 * error_probabilities_zeros
 * Allocates an array of doubles for storing a probability distribution of pauli errors
 * :: const size_t n_qubits :: The number of qubits that this error probability distribution covers
 * Returns an array of zeros
 */
double* error_probabilities_zeros(const size_t n_qubits);

/*
 * error_probabilities_identity
 * Allocates an array of doubles for storing a probability distribution of pauli errors, the identity element is given a probability of 1
 * :: const size_t n_qubits :: The number of qubits that this error probability distribution covers
 * Returns an array of zeros with the first element set to 1.0
 */
double* error_probabilities_identity(const size_t n_qubits);

/*
 * error_probabilities_copy
 * Allocates an array of doubles for storing a probability distribution of pauli errors, copies the values from another array to this one
 * :: const size_t n_qubits :: The number of qubits that this error probability distribution covers
 * :: double* error_probs :: The array to copy
 * Returns a pointer to a copy of the array passed
 */
double* error_probabilities_copy(const size_t n_qubits, double* error_probs);

/*
 * error_probabilities_step
 * Steps an probability distribution up or down over some number of qubits while ensuring that the distribution remains normalised 
 * :: double* error_probs :: The initial distribution
 * :: const size_t n_qubits_initial :: The number of qubits in the initial distribution
 * :: const size_t n_qubits_final :: The number of qubits in the final distribution
 * Returns an array of probabilities
 */
double* error_probabilities_step(double* error_probs, const uint32_t n_qubits_initial, const uint32_t n_qubits_final);

/*
 * error_probabilities_step_up
 * Steps an probability distribution up over some number of qubits while ensuring that the distribution remains normalised 
 * :: double* error_probs :: The initial distribution
 * :: const size_t n_qubits_initial :: The number of qubits in the initial distribution
 * :: const size_t n_qubits_final :: The number of qubits in the final distribution
 * Returns an array of probabilities
 */
double* error_probabilities_step_up(double* error_probs, const uint32_t n_qubits_initial, const uint32_t n_qubits_final);

/*
 * error_probabilities_step_down
 * Steps an probability distribution down over some number of qubits while ensuring that the distribution remains normalised 
 * :: double* error_probs :: The initial distribution
 * :: const size_t n_qubits_initial :: The number of qubits in the initial distribution
 * :: const size_t n_qubits_final :: The number of qubits in the final distribution
 * Returns an array of probabilities
 */
double* error_probabilities_step_down(double* error_probs, const uint32_t n_qubits_initial, const uint32_t n_qubits_final);

/*
 * error_probabilities_free 
 * Frees the array of errors
 * :: double* error_probs :: The distribution of errors to be freed
 * Returns nothing
 */
void error_probabilities_free(double* error_probs);


uint64_t error_probabilities_bytes_in_table(const uint32_t n_qubits);
uint64_t error_probabilities_entries_in_table(const uint32_t n_qubits);

// ----------------------------------------------------------------------------------------
// FUNCTION DEFINITIONS
// ----------------------------------------------------------------------------------------

/*
 * error_probabilities_m 
 * Allocates an array of doubles for storing a probability distribution of pauli errors
 * :: const size_t n_elements :: The number of elements that this error probability distribution covers
 * Returns an array of zeros
 */
double* error_probabilities_m(const size_t n_elements)
{
	// This differs from the below function by a factor of 2
	double* error_probs = (double*)calloc(1 << (n_elements), sizeof(double));
	return error_probs;
}

uint64_t error_probabilities_bytes_in_table(const uint32_t n_qubits)
{
	return error_probabilities_entries_in_table(n_qubits) * sizeof(double);
}

uint64_t error_probabilities_entries_in_table(const uint32_t n_qubits)
{
	return 1ull << (2 * n_qubits);
}

/*
 * error_probabilities_zeros
 * Allocates an array of doubles for storing a probability distribution of pauli errors
 * :: const size_t n_qubits :: The number of qubits that this error probability distribution covers
 * Returns an array of zeros
 */
double* error_probabilities_zeros(const size_t n_qubits)
{
	double* error_probs = (double*)calloc(error_probabilities_entries_in_table(n_qubits), sizeof(double));
	return error_probs;
}

/*
 * error_probabilities_identity
 * Allocates an array of doubles for storing a probability distribution of pauli errors, the identity element is given a probability of 1
 * :: const size_t n_qubits :: The number of qubits that this error probability distribution covers
 * Returns an array of zeros with the first element set to 1.0
 */
double* error_probabilities_identity(const size_t n_qubits)
{
	double* error_probs = error_probabilities_zeros(n_qubits);
	error_probs[0] = 1.0; // Set the identity to 1	
	return error_probs;
}

/*
 * error_probabilities_copy
 * Allocates an array of doubles for storing a probability distribution of pauli errors, copies the values from another array to this one
 * :: const size_t n_qubits :: The number of qubits that this error probability distribution covers
 * :: double* error_probs :: The array to copy
 * Returns a pointer to a copy of the array passed
 */
double* error_probabilities_copy(const size_t n_qubits, double* error_probs)
{
	double* error_probs_cpy = error_probabilities_zeros(n_qubits);

	memcpy(error_probs_cpy, error_probs, error_probabilities_bytes_in_table(n_qubits));

	return error_probs_cpy;
}

/*
 * error_probabilities_step
 * Steps an probability distribution up or down over some number of qubits while ensuring that the distribution remains normalised 
 * :: double* error_probs :: The initial distribution
 * :: const size_t n_qubits_initial :: The number of qubits in the initial distribution
 * :: const size_t n_qubits_final :: The number of qubits in the final distribution
 * Returns an array of probabilities
 */
double* error_probabilities_step(double* error_probs, const uint32_t n_qubits_initial, const uint32_t n_qubits_final)
{
	if (n_qubits_initial > n_qubits_final)
	{
		return error_probabilities_step_down(error_probs, n_qubits_initial, n_qubits_final);
	}

	if (n_qubits_initial < n_qubits_final)
	{
		return error_probabilities_step_up(error_probs, n_qubits_initial, n_qubits_final);
	}

	return error_probs;
}

/*
 * error_probabilities_step_up
 * Steps an probability distribution up over some number of qubits while ensuring that the distribution remains normalised 
 * :: double* error_probs :: The initial distribution
 * :: const size_t n_qubits_initial :: The number of qubits in the initial distribution
 * :: const size_t n_qubits_final :: The number of qubits in the final distribution
 * Returns an array of probabilities
 */
double* error_probabilities_step_up(double* error_probs, const uint32_t n_qubits_initial, const uint32_t n_qubits_final)
{
	double* expanded_error_probs = error_probabilities_zeros(n_qubits_final);

	// Copy the errors from the initial buffer to our larger buffer
	sym_iter* cpy_iter = sym_iter_create_n_qubits(n_qubits_initial);
	sym_iter* target_buffer = sym_iter_create_n_qubits(n_qubits_final);
	while (sym_iter_next(cpy_iter))
	{
		if (error_probs[sym_iter_ll_from_state(cpy_iter)] > 0)
		{
			// Copy the state to the target buffer
			for (uint32_t i = 0; i < n_qubits_initial; i++)
			{ // All other values in the target buffer should be zero
				sym_set_X(target_buffer->state, 0, i, sym_get_X(cpy_iter->state, 0, i)); // X elements
 				sym_set_Z(target_buffer->state, 0, i, sym_get_Z(cpy_iter->state, 0, i)); // Z elements
			} 

			// We've manually changed the iterator, need to update it
			sym_iter_update(target_buffer);

			// Set the value in the new buffer
			expanded_error_probs[sym_iter_ll_from_state(target_buffer)] += error_probs[sym_iter_ll_from_state(cpy_iter)];
		}
	}
	sym_iter_free(target_buffer);
	sym_iter_free(cpy_iter);

	return expanded_error_probs;
}

/*
 * error_probabilities_step_down
 * Steps an probability distribution down over some number of qubits while ensuring that the distribution remains normalised 
 * :: double* error_probs :: The initial distribution
 * :: const size_t n_qubits_initial :: The number of qubits in the initial distribution
 * :: const size_t n_qubits_final :: The number of qubits in the final distribution
 * Returns an array of probabilities
 */
double* error_probabilities_step_down(double* error_probs, const uint32_t n_qubits_initial, const uint32_t n_qubits_final)
{
	double* contracted_error_probs = error_probabilities_zeros(n_qubits_final);

	// Copy the errors from the initial buffer to our larger buffer
	sym_iter* cpy_iter = sym_iter_create_n_qubits(n_qubits_initial);
	sym_iter* target_buffer = sym_iter_create_n_qubits(n_qubits_final);
	while (sym_iter_next(cpy_iter))
	{
		if (error_probs[sym_iter_ll_from_state(cpy_iter)] > 0)
		{
			// Copy the state to the target buffer
			for (uint32_t i = 0; i < n_qubits_final; i++)
			{ // All other values in the target buffer should be zero
				sym_set_X(target_buffer->state, 0, i, sym_get_X(cpy_iter->state, 0, i)); // X elements
 				sym_set_Z(target_buffer->state, 0, i, sym_get_Z(cpy_iter->state, 0, i)); // Z elements
			}

			// We've manually changed the iterator, need to update it
			sym_iter_update(target_buffer);

			// Set the value in the new buffer
			contracted_error_probs[sym_iter_ll_from_state(target_buffer)] += error_probs[sym_iter_ll_from_state(cpy_iter)];
		}
	}
	sym_iter_free(target_buffer);
	sym_iter_free(cpy_iter);

	return contracted_error_probs;
}


/*
 * error_probabilities_free 
 * Frees the array of errors
 * :: double* error_probs :: The distribution of errors to be freed
 * Returns nothing
 */
void error_probabilities_free(double* error_probs)
{
	free(error_probs);
}

#endif