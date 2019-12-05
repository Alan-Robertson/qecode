#ifndef ERROR_MODEL_WEIGHT_ONE
#define ERROR_MODEL_WEIGHT_ONE

#include "error_models.h"

//----------------------------------------------------------------------------------------
// Inheriting Error Models
//----------------------------------------------------------------------------------------

// All errors in this model are at most weight one
// Errors of weight greater than one are ignored

// IID ERROR MODEL ------------------------------------------------------------------------------------------------

// Model params
typedef struct {
    double p_error;
    unsigned int n_qubits;
} model_params_weight_one ;

// DECLARATIONS ------------------------------------------------------------------------------------------------

/*
    error_model_create_iid
    Base model constructor for a weight one error model
    :: const double p_error :: Probability of a physical error
    :: const unsigned n_qubits :: Number of physical qubits
    Returns a pointer to a new error model object on the heap
*/
error_model* error_model_create_weight_one(const unsigned int n_qubits, const double p_error);

// Model Call
double error_model_call_weight_one(const sym* error, void* v_model_params);


// DEFINITIONS ------------------------------------------------------------------------------------------------

/*
    error_model_create_iid
    Base model constructor for iid error models
    :: const double p_error :: Probability of a physical error
    :: const unsigned n_qubits :: Number of physical qubits
    Returns a pointer to a new error model object on the heap
*/
error_model* error_model_create_weight_one(const unsigned int n_qubits, const double p_error)
{   
    error_model* m = error_model_create(sizeof(model_params_weight_one));
    model_params_weight_one* mp = (model_params_weight_one*)malloc(sizeof(model_params_weight_one));

    mp->p_error = p_error;
    mp->n_qubits = n_qubits;
    m->call = error_model_call_weight_one;
    m->params = mp;
    return m;
}

// Model Call
double error_model_call_weight_one(const sym* error, void* v_model_params)
{
    // Recast
    model_params_weight_one* model_params = (model_params_weight_one*)v_model_params;
    uint32_t weight = sym_weight(error);

    double prob = 0;
    if (weight == 0)
    {
        prob = 1 - model_params->p_error;
    }
    else if (weight == 1)
    {
        prob = model_params->p_error / (model_params->n_qubits * 3);
    }
    else
    {
        prob = 0.0;
    }

    return prob;
}

#endif