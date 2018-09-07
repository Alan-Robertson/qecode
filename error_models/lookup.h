#ifndef ERROR_MODEL_LOOKUP
#define ERROR_MODEL_LOOKUP

#include "error_models.h"
#include "../circuits/error_probabilities.h"

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

	mp->lookup_table = error_probabilities_copy(mp->n_qubits, lookup_table);

	m->params = mp;
	m->call = error_model_call_lookup;
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
	if (model_params->lookup_table != NULL)
	{
		free(model_params->lookup_table);
	}

	if (model_params != NULL)
	{
		free(model_params);
	}
	return;
}

#endif