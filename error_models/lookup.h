#ifndef ERROR_MODEL_LOOKUP
#define ERROR_MODEL_LOOKUP

#include "error_models.h"

// Lookup Model Composition -------------------------------------------------------------------------------
 
typedef struct {
	unsigned int n_qubits;
	double* lookup_table;
} error_model_params_lookup_t;


error_model* error_model_create_lookup(unsigned int n_qubits, double* lookup_table);
double error_model_call_lookup(const sym* error, void* v_model_params);
void error_model_free_lookup(void* v_model_params);

// The lookup table is copied!
error_model* error_model_create_lookup(unsigned int n_qubits, double* lookup_table)
{
	error_model* m = error_model_create(sizeof(error_model_params_lookup_t));
	error_model_params_lookup_t* mp = (error_model_params_lookup_t*)malloc(sizeof(error_model_params_lookup_t));

	mp->n_qubits = n_qubits;

	mp->lookup_table = (double*)calloc(sizeof(double), (1 << (2 * mp->n_qubits)));
	memcpy(mp->lookup_table, lookup_table, 1 << (2 * mp->n_qubits));

	m->model_params = mp;
	m->model_call = error_model_call_lookup;
	m->param_free = error_model_free_lookup;
	return m;
}

double error_model_call_lookup(const sym* error, void* v_model_params)
{
	error_model_params_lookup_t* model_params = (error_model_params_lookup_t*)v_model_params;
	return model_params->lookup_table[sym_to_ll(error)];
}

void error_model_free_lookup(void* v_model_params)
{
	error_model_params_lookup_t* model_params = (error_model_params_lookup_t*)v_model_params;
	free(model_params->lookup_table);
	free(model_params);
	return;
}

#endif