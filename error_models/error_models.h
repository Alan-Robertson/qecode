#ifndef ERROR_MODELS
#define ERROR_MODELS

#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdarg.h>
#include "../errors.h"

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
typedef struct {
	// Model parameters
	void* model_params;
	// Overload these functions
	error_model_f model_call; // Called to calculate the error probability
	param_free_f param_free; // Called to free the model parameters
} error_model;

/*
	error_model_create
	Base model constructor for error models, no arguments,
	Allocates memory for the error model and sets the default destructor
	Returns a pointer to a new error model object on the heap
*/
error_model* error_model_create();

/*
	error_model_free
	Destructor for error models, frees the error model and any 
	Associated parameters
	:: error_model* m :: The error model object to be freed
	Returns nothing
*/
void error_model_free(error_model* m);


/*
	error_model_param_free_default
	Default destructor for error model parameters
	Use this if none of the parameters have been allocated to heap memory
	Else implement your own method and set error_model->param_free to point to it
	:: error_model* m :: The error model object whose parameters are to be freed
	Returns nothing
*/
void error_model_param_free_default(void* model_params);

/*
	error_model_prob
	Dispatch method to call the error model's probability function
	:: error_model* m :: The error model object 
	:: const sym* error :: The error
	Returns the probability with which this error occurs under the given error model
*/
double error_model_prob(error_model* m, const sym* error);

/*
	error_model_param_free
	Dispatch method for freeing the parameters of an error model
	:: error_model* m :: The error model object whose parameters are to be freed
	Returns nothing
*/
void error_model_param_free(error_model* m);

// FUNCTION DEFINITIONS ----------------------------------------------------------------------------------------

// Default constructor method for creating a new error model
/*
	error_model_create
	Base model constructor for error models, no arguments,
	Allocates memory for the error model and sets the default destructor
	Returns a pointer to a new error model object on the heap
*/
error_model* error_model_create()
{
	error_model* m = (error_model*)malloc(sizeof(error_model));

	m->param_free = error_model_param_free_default;

	return m;
}

// Freeing the model and the associated parameters dispatch
/*
	error_model_free
	Destructor for error models, frees the error model and any 
	Associated parameters
	:: error_model* m :: The error model object to be freed
	Returns nothing
*/
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

// Default parameter free
/*
	error_model_param_free_default
	Default destructor for error model parameters
	Use this if none of the parameters have been allocated to heap memory
	Else implement your own method and set error_model->param_free to point to it
	:: error_model* m :: The error model object whose parameters are to be freed
	Returns nothing
*/
void error_model_param_free_default(void* model_params)
{
	free(model_params);
	return;
}

// DISPATCH METHODS ------------------------------------------------------------------------------------------------

// Dispatch method for calling the error model probability
/*
	error_model_prob
	Dispatch method to call the error model's probability function
	:: error_model* m :: The error model object 
	:: const sym* error :: The error
	Returns the probability with which this error occurs under the given error model
*/
double error_model_prob(error_model* m, const sym* error)
{
	return m->model_call(error, m->model_params);
}

// Dispatch method for calling parameter free
/*
	error_model_param_free
	Dispatch method for freeing the parameters of an error model
	:: error_model* m :: The error model object whose parameters are to be freed
	Returns nothing
*/
void error_model_param_free(error_model* m)
{
	m->param_free(m->model_params);
}

#endif