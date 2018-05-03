#ifndef ERROR_PROBABILITIES
#define ERROR_PROBABILITIES

double* error_probabilities_m(const size_t n_qubits)
{
	double* error_probs = (double*)calloc(1 << (2 * n_qubits), sizeof(double));
	return error_probs;
}

double* error_probabilities_zeros(const size_t n_qubits)
{
	double* error_probs = (double*)calloc(1 << (2 * n_qubits), sizeof(double));
	return error_probs;
}

double* error_probabilities_identity(const size_t n_qubits)
{
	double* error_probs = error_probabilities_zeros(n_qubits);
	error_probs[0] = 1; // Set the identity to 1	
	return error_probs;
}

double* error_probabilities_copy(const size_t n_qubits, double* error_probs)
{
	double* error_probs_cpy = error_probabilities_m(n_qubits);
	memcpy(error_probs_cpy, error_probs, (1 << (2 * n_qubits)) * sizeof(double));
	return error_probs_cpy;
}

void error_probabilities_free(double* error_probs)
{
	free(error_probs);
}

#endif