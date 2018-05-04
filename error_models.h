#ifndef ERROR_MODELS
#define ERROR_MODELS

#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdarg.h>
#include "errors.h"

// Error model template ------------------------------------------------------------------------------------------------
// Follow this for creating new error models
/*
	struct model_params_<model_name> 
	{
		model params
		...
	};

	// Constructor
	error_model* error_model_create_<model_name>(model_params...)
	{
		error_model* m = error_model_create(); // Sets default arguments
		struct model_params_<model_name>* mp = (struct model_params_<model_name>*)malloc(sizeof(struct model_params_<model_name>));
		//Fill in the model params
		
		// Set the params and call function
		m->model_params = mp;
		m->model_call = error_model_call_<model_name>;
		
		// Optional destructor!
		m->model_free = error_model_free_<model_name>;
		return m;
	}

	// Destructor
	// There is already a default destructor, if you do not need to allocate any heap memory within your 
	// model params, then this should be sufficient
	// Otherwise you will need to create your own destructor and set m->param_free to point to it.
	// The destructor should take the template: 
	void error_model_param_free_<model_name>(void* model_params);

	// Caller
	unsigned double error_model_call_<model_name>(sym* error, void* v_m);
	{
		// Recast
		model_params* m = (model_params*)v_m;
		
		//Evaluate the error string
		//If you need to view the error in a string format use the error_sym_to_str() function

		return <double: probability of the error occurring>
	}*/


//----------------------------------------------------------------------------------------
// The Error Model Base 'class'
//----------------------------------------------------------------------------------------

// DECLARATIONS ----------------------------------------------------------------------------------------

typedef double (*error_model_f)(const sym*, void*);
typedef void (*param_free_f)(void*);

// Polymorphic error model
typedef struct error_model
{
	// Model parameters
	void* model_params;
	// Overload these functions
	error_model_f model_call; // Called to calculate the error probability
	param_free_f param_free; // Called to free the model parameters
}

// FUNCTION DEFINITIONS ----------------------------------------------------------------------------------------

// Default constructor method for creating a new error model
error_model* error_model_create()
{
	error_model* m = (error_model*)malloc(sizeof(error_model));

	m->param_free = error_model_param_free_default;

	return m;
}

// Freeing the model and the associated parameters
void error_model_free(error_model* m)
{
	if (NULL != m)
	{
		if (NULL != m->model_params)
		{
			error_model_param_free(m);
		}
		free(m);
	}
	return;
}

// Default paramater free
void error_model_param_free_default(void* model_params)
{
	free(model_params);
	return;
}

// DISPATCH METHODS ------------------------------------------------------------------------------------------------

// Dispatch method for calling the error model probability
double error_model_prob(error_model m*, const sym* error)
{
	return m->prob_error(error, m->model_params);
}

// Dispatch method for calling parameter free
void error_model_param_free(error_model* m)
{
	m->param_free(m->model_params);
}


//----------------------------------------------------------------------------------------
// Inheriting Error Models
//----------------------------------------------------------------------------------------

// IID ERROR MODEL ------------------------------------------------------------------------------------------------

// Model params
struct {
	double p_error;
	unsigned int n_qubits;
} model_params_iid;

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
	struct iid_model_params* model_params = (struct iid_model_params*)v_model_params;
	
	unsigned int weight = sym_weight(error);

	return pow(model_params->p_error / 3, weight) * pow(1 - model_params->p_error, model_params->n_qubits - weight);
}

// BIASED IID ERROR MODEL FAMILY ------------------------------------------------------------------------------------------------
struct {
	double p_error;
	unsigned int n_qubits;
	double bias;
} model_params_iid_biased;

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
	model_params_iid* mp = (model_params_iid_biased*)malloc(sizeof(model_params_iid));

	mp->p_error = p_error;
	mp->n_qubits = n_qubits;

	m->model_params = mp;

	return m;
}

// Model Constructors
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

	m->model_call = error_model_iid_biased_X;

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

	m->model_call = error_model_iid_biased_Y;

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

	m->model_call = error_model_iid_biased_Z;

	return m;
}

// Model Callers
double error_model_call_iid_biased_X(const sym* error, void* v_model_params)
{
	// Recast
	struct model_params_iid_biased* model_params = (struct model_params_iid_biased*)v_model_params;
	
	unsigned int weight = sym_weight(error);
	unsigned int x_weight = sym_weight_X(error);
	
	return (pow(model_params->p_error * model_params->bias / (model_params->bias + 2.0), x_weight)  // p_x
		* pow(model_params->p_error / (model_params->bias + 2.0), weight - x_weight)  // p_z, p_y
		* pow(1 - model_params->p_error, model_params->n_qubits - weight));           // p_i
}


double error_model_call_iid_biased_Y(const sym* error, void* v_model_params)
{
	// Recast
	struct model_params_iid_biased* model_params = (struct model_params_iid_biased*)v_model_params;
	
	unsigned int weight = sym_weight(error);
	unsigned int y_weight = sym_weight_Y(error);
	
	return (pow(model_params->p_error * model_params->bias / (model_params->bias + 2.0), y_weight)  // p_x
		* pow(model_params->p_error / (model_params->bias + 2.0), weight - y_weight)  // p_z, p_y
		* pow(1 - model_params->p_error, model_params->n_qubits - weight));           // p_i
}


double error_model_iid_biased_Z(const sym* error, void* v_model_params)
{
	// Recast
	struct model_params_iid_biased* model_params = (struct model_params_iid_biased*)v_model_params;
	
	unsigned int weight = sym_weight(error);
	unsigned int z_weight = sym_weight_Z(error);
	
	return (pow(model_params->p_error * model_params->bias / (model_params->bias + 2.0), z_weight)  // p_x
		* pow(model_params->p_error / (model_params->bias + 2.0), weight - z_weight)  // p_z, p_y
		* pow(1 - model_params->p_error, model_params->n_qubits - weight));           // p_i
}


// Trivial Bit flip model ------------------------------------------------------------------------------------
// Error only occurs on the first bit

struct {
	double p_error;
} model_params_bit_flip_trivial;

// Model Constructor
error_model* error_model_create_bit_flip_trivial(const double p_error)
{	
	error_model* m = error_model_create();
	struct model_params_bit_flip_trivial* mp = (struct model_params_bit_flip_trivial*)malloc(sizeof(model_params_iid));

	mp->p_error = p_error;

	m->model_call = error_model_bit_flip_trivial;
	m->model_params = mp;

	return m;
}

double error_model_bit_flip_trivial(const sym* error, void* v_model_params)
{
	// Recast
	struct model_params_bit_flip_trivial* model_params = (struct model_params_bit_flip_trivial*)v_model_params;
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

struct {
	double p_error;
	int n_qubits;
} model_params_bit_flip;

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

// Spatially Asymmetric Noise -------------------------------------------------------------------------------

struct {
	unsigned n_bitflip_qubits;
	double p_bitflip;
	unsigned n_phaseflip_qubits;
	double p_phaseflip;
} spatially_asymmetric_model_params;

double error_model_spatially_asymmetric(const sym* error, void* v_model_params)
{
	struct spatially_asymmetric_model_params* model_params = (struct spatially_asymmetric_model_params*) v_model_params;
	if (sym_weight_Y(error) > 0) // No Y errors
	{
		return 0;
	}

	for (int i = 0; i < model_params->n_bitflip_qubits; i++)
	{
		if (sym_get(error, 0, i+(model_params->n_bitflip_qubits + model_params->n_phaseflip_qubits)))
		{
			return 0;
		}
	}

	for (int i = 0; i < model_params->n_phaseflip_qubits; i++)
	{
		if (sym_get(error, 0, i+(model_params->n_bitflip_qubits)))
		{
			return 0;
		}
	}

 	unsigned x_weight = sym_weight_X(error);
 	unsigned z_weight = sym_weight_Z(error);

	return pow(1.0 - (model_params->p_bitflip), model_params->n_bitflip_qubits - x_weight) * pow((model_params->p_bitflip), x_weight) * pow(1.0 - (model_params->p_phaseflip), model_params->n_phaseflip_qubits - z_weight) * pow(model_params->p_phaseflip, z_weight);

}

// Multi Model Composition -------------------------------------------------------------------------------
 
struct {
	unsigned n_models;
	unsigned* model_split;
	error_model_f* error_models;
	void** error_models_params;
} multi_composition_error_model_params;

double error_model_multi_composition(const sym* error, void* v_model_params)
{
	double prob = 1;
	unsigned current_qubit = 0;
	struct multi_composition_error_model_params* model_params = (struct multi_composition_error_model_params*)v_model_params;

	for (size_t i = 0; i < model_params->n_models; i++)
	{
		sym* partial_error = sym_create(1, model_params->model_split[i] * 2);

		for (size_t j = 0; j < model_params->model_split[i]; j++)
		{
			sym_set(partial_error, 0, j, sym_get(error, 0, current_qubit + j));
			sym_set(partial_error, 0, j + model_params->model_split[i], sym_get(error, 0, (error->length / 2) + current_qubit + j));
		}

		prob *= model_params->error_models[i](partial_error, model_params->error_models_params[i]);
		current_qubit += model_params->model_split[i];
		
		sym_free(partial_error);
	}	
	return prob;
}

error_model_create_multi_composition()
{

}


multi_composition_error_model_params error_model_multi_builder(unsigned n_models, ...)
{
	struct multi_composition_error_model_params model_params;
	model_params.n_models = n_models;
	model_params.model_split = (unsigned*)malloc(sizeof(unsigned) * model_params.n_models);
	model_params.error_models = (error_model_f*)malloc(sizeof(error_model_f) * model_params.n_models);
	model_params.error_models_params = (void**)malloc(sizeof(void*) * model_params.n_models);

	unsigned counter = 0;

	va_list argv;
	va_start(argv, n_models);

	enum mode {MODEL_SPLIT_e, ERROR_MODELS_e, MODEL_params_e, TERM_e};
	unsigned current_mode = MODEL_SPLIT_e;

	while (current_mode != TERM_e)
	{
		switch (current_mode)
		{
			case MODEL_SPLIT_e:
				model_params.model_split[counter] = va_arg(argv, unsigned);
			break;

			case ERROR_MODELS_e:
				model_params.error_models[counter] = va_arg(argv, error_model_f);
			break;

			case MODEL_params_e:
				model_params.error_models_params[counter] = va_arg(argv, void*);
			break;
		}

		counter++;

		if (counter == model_params.n_models)
		{
			current_mode++;
			counter = 0;
		}
	}
	va_end(argv);
	return model_params;
}

void error_model_multi_free(void* v_model_params)
{
	struct multi_composition_error_model_params* model_params = (struct multi_composition_error_model_params*)v_model_params;
	free(model_params->model_split);
	free(model_params->error_models);
	free(model_params->error_models_params);
	return;
}

// Lookup Model Composition -------------------------------------------------------------------------------
 
struct {
	double* lookup_table;
} lookup_error_model_params;

double error_model_lookup(const sym* error, void* v_model_params)
{
	struct lookup_error_model_params* model_params = (lookup_error_model_params*)v_model_params;
	return model_params->lookup_table[sym_to_ll(error)];
}


#endif
