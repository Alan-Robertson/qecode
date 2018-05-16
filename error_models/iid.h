#ifndef ERROR_MODEL_IID
#define ERROR_MODEL_IID

#include "error_models.h"

//----------------------------------------------------------------------------------------
// Inheriting Error Models
//----------------------------------------------------------------------------------------

// IID ERROR MODEL ------------------------------------------------------------------------------------------------

// Model params
typedef struct {
	double p_error;
	unsigned int n_qubits;
} model_params_iid ;

// DECLARATIONS ------------------------------------------------------------------------------------------------

/*
	error_model_create_iid
	Base model constructor for iid error models
	:: const double p_error :: Probability of a physical error
	:: const unsigned n_qubits :: Number of physical qubits
	Returns a pointer to a new error model object on the heap
*/
error_model* error_model_create_iid(const unsigned int n_qubitsm, const double p_error);

// Model Call
double error_model_call_iid(const sym* error, void* v_model_params);


// DEFINITIONS ------------------------------------------------------------------------------------------------

/*
	error_model_create_iid
	Base model constructor for iid error models
	:: const double p_error :: Probability of a physical error
	:: const unsigned n_qubits :: Number of physical qubits
	Returns a pointer to a new error model object on the heap
*/
error_model* error_model_create_iid(const unsigned int n_qubits, const double p_error)
{	
	error_model* m = error_model_create(sizeof(model_params_iid));
	model_params_iid* mp = (model_params_iid*)malloc(sizeof(model_params_iid));

	mp->p_error = p_error;
	mp->n_qubits = n_qubits;

	m->call = error_model_call_iid;
	m->params = mp;

	return m;
}

// Model Call
double error_model_call_iid(const sym* error, void* v_model_params)
{
	// Recast
	model_params_iid* model_params = (model_params_iid*)v_model_params;
	
	unsigned int weight = sym_weight(error);

	return pow(model_params->p_error / 3, weight) * pow(1 - model_params->p_error, model_params->n_qubits - weight);
}

#endif