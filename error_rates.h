#ifndef ERROR_RATES
#define ERROR_RATES

double* error_rates_m(const size_t n_qubits)
{
	double* error_rate = (double*)calloc(1 << (2 * n_qubits), sizeof(double));
	return error_rate;
}

double* error_rates_zeros(const size_t n_qubits)
{
	double* error_rate = (double*)calloc(1 << (2 * n_qubits), sizeof(double));
	return error_rate;
}

double* error_rates_identity(const size_t n_qubits)
{
	double* error_rate = error_rates_zeros(n_qubits);
	error_rate[0] = 1; // Set the identity to 1	
	return error_rate;
}

double* error_rates_copy(const size_t n_qubits, double* error_rates)
{
	double* error_rates_cpy = error_rates_m(n_qubits);
	memcpy(error_rates_cpy, error_rates, (1 << (2 * n_qubits)) * sizeof(double));
	return error_rates_cpy;
}

void error_rates_free(double* error_rate)
{
	free(error_rate);
}

#endif