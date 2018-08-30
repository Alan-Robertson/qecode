#ifndef ERROR_MODEL_IID_BIASED
#define ERROR_MODEL_IID_BIASED

#include "error_models.h"

//----------------------------------------------------------------------------------------
// Inheriting Error Models
//----------------------------------------------------------------------------------------

// IID ERROR MODEL ------------------------------------------------------------------------------------------------

typedef struct {
	double p_error;
	unsigned int n_qubits;
	double bias;
}  model_params_iid_biased ;

// DECLARATIONS ------------------------------------------------------------------------------------------------

// Model Constructors
/*
	error_model_create_iid_biased
	Base model constructor for all biased iid error models
	:: const double p_error :: Probability of a physical error
	:: const unsigned n_qubits :: Number of physical qubits
	:: const double bias :: The bias in the type of error
	Returns a pointer to a new error model object on the heap
*/
error_model* error_model_create_iid_biased(const unsigned int n_qubits, const double p_error, const double bias);

/*
	error_model_create_iid_biased_X
	Base model constructor for X biased iid error models
	:: const double p_error :: Probability of a physical error
	:: const unsigned n_qubits :: Number of physical qubits
	:: const double bias :: The bias towards X errors
	Returns a pointer to a new error model object on the heap
*/
error_model* error_model_create_iid_biased_X(const unsigned int n_qubits, const double p_error, const double bias);

/*
	error_model_create_iid_biased_Y
	Base model constructor for Y biased iid error models
	:: const double p_error :: Probability of a physical error
	:: const unsigned n_qubits :: Number of physical qubits
	:: const double bias :: The bias towards Y errors
	Returns a pointer to a new error model object on the heap
*/
error_model* error_model_create_iid_biased_Y(const unsigned int n_qubits, const double p_error, const double bias);

/*
	error_model_create_iid_biased_Z
	Base model constructor for Z biased iid error models
	:: const double p_error :: Probability of a physical error
	:: const unsigned n_qubits :: Number of physical qubits
	:: const double bias :: The bias towards Z errors
	Returns a pointer to a new error model object on the heap
*/
error_model* error_model_create_iid_biased_Z(const unsigned int n_qubits, const double p_error, const double bias);

// Model Callers
double error_model_call_iid_biased_X(const sym* error, void* v_model_params);
double error_model_call_iid_biased_Y(const sym* error, void* v_model_params);
double error_model_call_iid_biased_Z(const sym* error, void* v_model_params);



// BIASED IID ERROR MODEL FAMILY ------------------------------------------------------------------------------------------------


// Model Constructors
/*
	error_model_create_iid_biased
	Base model constructor for all biased iid error models
	:: const double p_error :: Probability of a physical error
	:: const unsigned n_qubits :: Number of physical qubits
	:: const double bias :: The bias in the type of error
	Returns a pointer to a new error model object on the heap
*/
error_model* error_model_create_iid_biased(const unsigned int n_qubits, const double p_error, const double bias)
{	
	error_model* m = error_model_create(sizeof(model_params_iid_biased));
	model_params_iid_biased* mp = (model_params_iid_biased*)malloc(sizeof(model_params_iid_biased));

	mp->p_error = p_error;
	mp->n_qubits = n_qubits;
	mp->bias = bias;

	m->params = mp;

	return m;
}

/*
	error_model_create_iid_biased_X
	Base model constructor for X biased iid error models
	:: const double p_error :: Probability of a physical error
	:: const unsigned n_qubits :: Number of physical qubits
	:: const double bias :: The bias towards X errors
	Returns a pointer to a new error model object on the heap
*/
error_model* error_model_create_iid_biased_X(const unsigned int n_qubits, const double p_error, const double bias)
{	
	error_model* m = error_model_create_iid_biased(n_qubits, p_error, bias);

	m->call = error_model_call_iid_biased_X;

	return m;
}

/*
	error_model_create_iid_biased_Y
	Base model constructor for Y biased iid error models
	:: const double p_error :: Probability of a physical error
	:: const unsigned n_qubits :: Number of physical qubits
	:: const double bias :: The bias towards Y errors
	Returns a pointer to a new error model object on the heap
*/
error_model* error_model_create_iid_biased_Y(const unsigned int n_qubits, const double p_error, const double bias)
{	
	error_model* m = error_model_create_iid_biased(n_qubits, p_error, bias);

	m->call = error_model_call_iid_biased_Y;

	return m;
}

/*
	error_model_create_iid_biased_Z
	Base model constructor for Z biased iid error models
	:: const double p_error :: Probability of a physical error
	:: const unsigned n_qubits :: Number of physical qubits
	:: const double bias :: The bias towards Z errors
	Returns a pointer to a new error model object on the heap
*/
error_model* error_model_create_iid_biased_Z(const unsigned int n_qubits, const double p_error, const double bias)
{	
	error_model* m = error_model_create_iid_biased(n_qubits, p_error, bias);

	m->call = error_model_call_iid_biased_Z;

	return m;
}

// Model Callers
double error_model_call_iid_biased_X(const sym* error, void* v_model_params)
{
	// Recast
	model_params_iid_biased* model_params = (model_params_iid_biased*)v_model_params;
	
	unsigned int weight = sym_weight(error);
	unsigned int x_weight = sym_weight_X(error);

	double p_b = model_params->p_error / (1.0 + 2.0 / model_params->bias);
	double p_nb = model_params->p_error / (2.0 +  model_params->bias);
	
	return (pow(p_b, x_weight)  // p_x
		* pow(p_nb, weight - x_weight)  // p_z, p_y
		* pow(1 - model_params->p_error, model_params->n_qubits - weight));           // p_i
}


double error_model_call_iid_biased_Y(const sym* error, void* v_model_params)
{
	// Recast
	model_params_iid_biased* model_params = (model_params_iid_biased*)v_model_params;
	
	unsigned int weight = sym_weight(error);
	unsigned int y_weight = sym_weight_Y(error);

	double p_b = model_params->p_error / (1.0 + 2.0 / model_params->bias);
	double p_nb = model_params->p_error / (2.0 +  model_params->bias);
	
		return (pow(p_b, y_weight)  // p_y
		* pow(p_nb, weight - y_weight)  // p_z, p_x
		* pow(1 - model_params->p_error, model_params->n_qubits - weight)); 
}


double error_model_call_iid_biased_Z(const sym* error, void* v_model_params)
{
	// Recast
	model_params_iid_biased* model_params = (model_params_iid_biased*)v_model_params;
	
	unsigned int weight = sym_weight(error);
	unsigned int z_weight = sym_weight_Z(error);

	double p_b = model_params->p_error / (1.0 + 2.0 / model_params->bias);
	double p_nb = model_params->p_error / (2.0 +  model_params->bias);
	
	return (pow(p_b, z_weight)  // p_z
		* pow(p_nb, weight - z_weight)  // p_y, p_x
		* pow(1 - model_params->p_error, model_params->n_qubits - weight)); 
}

#endif