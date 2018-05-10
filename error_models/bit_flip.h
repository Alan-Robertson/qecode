#ifndef ERROR_MODEL_BIT_FLIP
#define ERROR_MODEL_BIT_FLIP

#include "error_models.h"

// Trivial Bit flip model ------------------------------------------------------------------------------------
// Error only occurs on the first bit
struct model_params_bit_flip_trivial {
	double p_error;
};

struct {
	double p_error;
	int n_qubits;
} model_params_bit_flip;

/*
	error_model_create_bit_flip_trivial
	Model constructor for the trivial bit flip error model
	:: const double p_error :: Probability of an X error on the first qubit
	Returns a pointer to a new error model object on the heap
*/
error_model* error_model_create_bit_flip_trivial(const double p_error)

/*
	error_model_create_bit_flip_trivial
	Model constructor for the trivial bit flip error model
	:: const double p_error :: Probability of an X error on the first qubit
	Returns a pointer to a new error model object on the heap
*/
error_model* error_model_create_bit_flip_trivial(const double p_error)
{	
	error_model* m = error_model_create(sizeof(model_params_bit_flip_trivial));
	struct model_params_bit_flip_trivial* mp = (struct model_params_bit_flip_trivial*)malloc(sizeof(model_params_iid));

	mp->p_error = p_error;

	m->model_call = error_model_call_bit_flip_trivial;
	m->model_params = mp;

	return m;
}

double error_model_call_bit_flip_trivial(const sym* error, void* v_model_params)
{
	// Recast
	struct model_params_bit_flip_trivial* model_params = (struct model_params_bit_flip_trivial*)v_model_params;
	
	// Check the error string
	char* error_string = error_sym_to_str(error);
	if (!strcmp(error_string, "II"))
	{
		return (1.0 - model_params->p_error);
	}

	if (!strcmp(error_string, "XI"))
	{
		return model_params->p_error;
	}
	return 0;
}

/*
	error_model_create_bit_flip
	Model constructor for the bit flip error model
	:: const unsigned n_qubits :: The number of physical qubits
	:: const double p_error :: Probability of an X error
	Returns a pointer to a new error model object on the heap
*/
error_model* error_model_create_bit_flip(const unsigned n_qubits, const double p_error)
{	
	error_model* m = error_model_create(sizeof(model_params_bit_flip));
	struct model_params_bit_flip_trivial* mp = (struct model_params_bit_flip_trivial*)malloc(sizeof(model_params_iid));

	mp->n_qubits = n_qubits;
	mp->p_error = p_error;

	m->model_call = error_model_call_bit_flip_trivial;
	m->model_params = mp;

	return m;
}

double error_model_bit_flip(const sym* error, void* v_model_params)
{
	// Recast
	struct bit_flip_model_params* model_params = (struct bit_flip_model_params*)v_model_params;
	char* error_string = (char*)error;

	unsigned int weight = sym_weight(error);
	unsigned int x_weight = sym_weight_X(error);

	if (weight == x_weight) // No non-x errors allowed
	{
		return pow(model_params->p_error, weight) * pow(1 - model_params->p_error, model_params->n_qubits - weight);
	}
	return 0;
}

#endif