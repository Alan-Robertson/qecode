// Lookup Model Composition -------------------------------------------------------------------------------
 
struct error_model_params_lookup {
	unsigned int n_qubits;
	double* lookup_table;
};

error_model* error_model_create_lookup(unsigned int n_qubits, double* lookup_table);
double error_model_call_lookup(const sym* error, void* v_model_params);
void error_model_free_lookup(void* v_model_params);

// The lookup table is copied!
error_model* error_model_create_lookup(unsigned int n_qubits, double* lookup_table)
{
	error_model* m = error_model_create();
	struct error_model_params_lookup* mp = (struct error_model_params_lookup*)malloc(sizeof(error_model_params_lookup));

	mp->n_qubits = n_qubits;

	mp->lookup_table = (double*)malloc(sizeof(double) * (1 << (2 * mp->n_qubits)));
	memcpy(mp->lookup_table, lookup_table, 1 << (2 * mp->n_qubits));

	m->model_call = error_model_call_lookup;
	m->param_free = error_model_free_lookup;
	return m;
}

double error_model_call_lookup(const sym* error, void* v_model_params)
{
	struct lookup_error_model_params* model_params = (lookup_error_model_params*)v_model_params;
	return model_params->lookup_table[sym_to_ll(error)];
}

void error_model_free_lookup(void* v_model_params)
{
	struct error_model_params_lookup* model_params = (struct error_model_params_lookup*)v_model_params;
	free(model_params->lookup_table);
	free(model_params);
	return;
}