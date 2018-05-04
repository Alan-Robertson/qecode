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

typedef struct {
	double p_error;
	unsigned int n_qubits;
	double bias;
}  model_params_iid_biased ;

// DECLARATIONS ------------------------------------------------------------------------------------------------

/*
	error_model_create_iid
	Base model constructor for iid error models
	:: const double p_error :: Probability of a physical error
	:: const unsigned n_qubits :: Number of physical qubits
	Returns a pointer to a new error model object on the heap
*/
error_model* error_model_create_iid(const double p_error, const unsigned int n_qubits);

// Model Call
double error_model_call_iid(const sym* error, void* v_model_params);


// Model Constructors
/*
	error_model_create_iid_biased
	Base model constructor for all biased iid error models
	:: const double p_error :: Probability of a physical error
	:: const unsigned n_qubits :: Number of physical qubits
	:: const double bias :: The bias in the type of error
	Returns a pointer to a new error model object on the heap
*/
error_model* error_model_create_iid_biased(const double p_error, const unsigned int n_qubits, const double bias);

/*
	error_model_create_iid_biased_X
	Base model constructor for X biased iid error models
	:: const double p_error :: Probability of a physical error
	:: const unsigned n_qubits :: Number of physical qubits
	:: const double bias :: The bias towards X errors
	Returns a pointer to a new error model object on the heap
*/
error_model* error_model_create_iid_biased_X(const double p_error, const unsigned int n_qubits, const double bias);

/*
	error_model_create_iid_biased_Y
	Base model constructor for Y biased iid error models
	:: const double p_error :: Probability of a physical error
	:: const unsigned n_qubits :: Number of physical qubits
	:: const double bias :: The bias towards Y errors
	Returns a pointer to a new error model object on the heap
*/
error_model* error_model_create_iid_biased_Y(const double p_error, const unsigned int n_qubits, const double bias);

/*
	error_model_create_iid_biased_Z
	Base model constructor for Z biased iid error models
	:: const double p_error :: Probability of a physical error
	:: const unsigned n_qubits :: Number of physical qubits
	:: const double bias :: The bias towards Z errors
	Returns a pointer to a new error model object on the heap
*/
error_model* error_model_create_iid_biased_Z(const double p_error, const unsigned int n_qubits, const double bias);

// Model Callers
double error_model_call_iid_biased_X(const sym* error, void* v_model_params);
double error_model_call_iid_biased_Y(const sym* error, void* v_model_params);
double error_model_call_iid_biased_Z(const sym* error, void* v_model_params);


// DEFINITIONS ------------------------------------------------------------------------------------------------

/*
	error_model_create_iid
	Base model constructor for iid error models
	:: const double p_error :: Probability of a physical error
	:: const unsigned n_qubits :: Number of physical qubits
	Returns a pointer to a new error model object on the heap
*/
error_model* error_model_create_iid(const double p_error, const unsigned int n_qubits)
{	
	error_model* m = error_model_create();
	model_params_iid* mp = (model_params_iid*)malloc(sizeof(model_params_iid));

	mp->p_error = p_error;
	mp->n_qubits = n_qubits;

	m->model_call = error_model_call_iid;
	m->model_params = mp;

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
error_model* error_model_create_iid_biased(const double p_error, const unsigned int n_qubits, const double bias)
{	
	error_model* m = error_model_create();
	model_params_iid_biased* mp = (model_params_iid_biased*)malloc(sizeof(model_params_iid_biased));

	mp->p_error = p_error;
	mp->n_qubits = n_qubits;

	m->model_params = mp;

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
error_model* error_model_create_iid_biased_X(const double p_error, const unsigned int n_qubits, const double bias)
{	
	error_model* m = error_model_create_iid_biased(p_error, n_qubits, bias);

	m->model_call = error_model_call_iid_biased_X;

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
error_model* error_model_create_iid_biased_Y(const double p_error, const unsigned int n_qubits, const double bias)
{	
	error_model* m = error_model_create_iid_biased(p_error, n_qubits, bias);

	m->model_call = error_model_call_iid_biased_Y;

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
error_model* error_model_create_iid_biased_Z(const double p_error, const unsigned int n_qubits, const double bias)
{	
	error_model* m = error_model_create_iid_biased(p_error, n_qubits, bias);

	m->model_call = error_model_call_iid_biased_Z;

	return m;
}

// Model Callers
double error_model_call_iid_biased_X(const sym* error, void* v_model_params)
{
	// Recast
	model_params_iid_biased* model_params = (model_params_iid_biased*)v_model_params;
	
	unsigned int weight = sym_weight(error);
	unsigned int x_weight = sym_weight_X(error);
	
	return (pow(model_params->p_error * model_params->bias / (model_params->bias + 2.0), x_weight)  // p_x
		* pow(model_params->p_error / (model_params->bias + 2.0), weight - x_weight)  // p_z, p_y
		* pow(1 - model_params->p_error, model_params->n_qubits - weight));           // p_i
}


double error_model_call_iid_biased_Y(const sym* error, void* v_model_params)
{
	// Recast
	model_params_iid_biased* model_params = (model_params_iid_biased*)v_model_params;
	
	unsigned int weight = sym_weight(error);
	unsigned int y_weight = sym_weight_Y(error);
	
	return (pow(model_params->p_error * model_params->bias / (model_params->bias + 2.0), y_weight)  // p_x
		* pow(model_params->p_error / (model_params->bias + 2.0), weight - y_weight)  // p_z, p_y
		* pow(1 - model_params->p_error, model_params->n_qubits - weight));           // p_i
}


double error_model_call_iid_biased_Z(const sym* error, void* v_model_params)
{
	// Recast
	model_params_iid_biased* model_params = (model_params_iid_biased*)v_model_params;
	
	unsigned int weight = sym_weight(error);
	unsigned int z_weight = sym_weight_Z(error);
	
	return (pow(model_params->p_error * model_params->bias / (model_params->bias + 2.0), z_weight)  // p_x
		* pow(model_params->p_error / (model_params->bias + 2.0), weight - z_weight)  // p_z, p_y
		* pow(1 - model_params->p_error, model_params->n_qubits - weight));           // p_i
}

#endif
