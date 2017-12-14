#ifndef ERROR_MODELS
#define ERROR_MODELS

#include <string.h>
#include <math.h>
#include <stdio.h>
#include "errors.h"


// Error model template:
/*

unsigned double error_model(sym* error, void* v_m);
{
	// Recast
	model_data* m = (model_data*)v_m;
	
	Evaluate the error string
	If you need to view the error in a string format use the error_sym_to_str() function

	return FLOAT
}

*/

// IID ERROR MODEL ------------------------------------------------------------------------------------------------

typedef struct {
	double p_error;
	unsigned int n_qubits;
} iid_model_data;

double error_model_iid(const sym* error, void* v_model_data)
{
	// Recast
	iid_model_data* model_data = (iid_model_data*)v_model_data;
	
	unsigned int weight = sym_weight(error);

	return pow(model_data->p_error / 3, weight) * pow(1 - model_data->p_error, model_data->n_qubits - weight);
}


// Trivial Bit flip model ------------------------------------------------------------------------------------
// Error only occurs on the first bit

typedef struct {
	double p_error;
} trivial_bit_flip_model_data;

double error_model_trivial_bit_flip(const sym* error, void* v_model_data)
{
	// Recast
	trivial_bit_flip_model_data* model_data = (trivial_bit_flip_model_data*)v_model_data;
	char* error_string = error_sym_to_str(error);
	if (!strcmp(error_string, "II"))
	{
		return (1.0 - model_data->p_error);
	}

	if (!strcmp(error_string, "XI"))
	{
		return model_data->p_error;
	}
	return 0;
}

// Bit flip ------------------------------------------------------------------------------------------------

typedef struct {
	double p_error;
	int n_qubits;
} bit_flip_model_data;

double error_model_bit_flip(const sym* error, void* v_model_data)
{
	// Recast
	bit_flip_model_data* model_data = (bit_flip_model_data*)v_model_data;
	char* error_string = (char*)error;

	unsigned int weight = sym_weight(error);
	unsigned int x_weight = sym_weight_X(error);

	if (weight == x_weight) // No non-x errors allowed
	{
		return pow(model_data->p_error, weight) * pow(1 - model_data->p_error, model_data->n_qubits - weight);
	}
	return 0;
}

// Spatially Asymmetric Noise -------------------------------------------------------------------------------

typedef struct {
	unsigned n_bitflip_qubits;
	double p_bitflip;
	unsigned n_phaseflip_qubits;
	double p_phaseflip;
} spatially_asymmetric_model_data;

double error_model_spatially_asymmetric(const sym* error, void* v_model_data)
{
	spatially_asymmetric_model_data* model_data = (spatially_asymmetric_model_data*) v_model_data;
	if (sym_weight_Y(error) > 0) // No Y errors
	{
		return 0;
	}

	for (int i = 0; i < model_data->n_bitflip_qubits; i++)
	{
		if (sym_get(error, 0, i+(model_data->n_bitflip_qubits + model_data->n_phaseflip_qubits)))
		{
			return 0;
		}
	}


	for (int i = 0; i < model_data->n_phaseflip_qubits; i++)
	{
		if (sym_get(error, 0, i+(model_data->n_bitflip_qubits)))
		{
			return 0;
		}
	}

 	unsigned x_weight = sym_weight_X(error);
 	unsigned z_weight = sym_weight_Z(error);

	return pow(1.0 - (model_data->p_bitflip), model_data->n_bitflip_qubits - x_weight) * pow((model_data->p_bitflip), x_weight) * pow(1.0 - (model_data->p_phaseflip), model_data->n_phaseflip_qubits - z_weight) * pow(model_data->p_phaseflip, z_weight);

}

// Shitty qubits -------------------------------------------------------------------------------

typedef struct {
	double p_error;
	unsigned n_good_qubits;
	unsigned n_bordell_qubits;
} bordell_model_data;

double error_model_bordell(const sym* error, void* model_data)
{
	// Going to need to make sure this is normalised


	return 1;
}

#endif