#ifndef ERROR_MODELS
#define ERROR_MODELS

#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdarg.h>
#include "errors.h"


// Error model template:
/*

unsigned double error_model(sym* error, void* v_m);
{
	// Recast
	model_data* m = (model_data*)v_m;
	
	Evaluate the error string
	If you need to view the error in a string format use the error_sym_to_str() function

	return double
}

*/

typedef double (*error_model_f)(const sym*, void*);

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
		printf("%d %d %d\n", model_data->p_error, model_data->n_qubits, weight);
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

// Multi Model Composition -------------------------------------------------------------------------------
 
typedef struct {
	unsigned n_models;
	unsigned* model_split;
	error_model_f* error_models;
	void** error_models_data;
} multi_composition_error_model_data;

double error_model_multi_composition(const sym* error, void* v_model_data)
{
	double prob = 1;
	unsigned current_qubit = 0;
	multi_composition_error_model_data* model_data = (multi_composition_error_model_data*)v_model_data;

	for (size_t i = 0; i < model_data->n_models; i++)
	{
		sym* partial_error = sym_create(1, model_data->model_split[i] * 2);

		for (size_t j = 0; j < model_data->model_split[i]; j++)
		{
			sym_set(partial_error, 0, j, sym_get(error, 0, current_qubit + j));
			sym_set(partial_error, 0, j + model_data->model_split[i], sym_get(error, 0, (error->length / 2) + current_qubit + j));
		}

		prob *= model_data->error_models[i](partial_error, model_data->error_models_data[i]);
		current_qubit += model_data->model_split[i];
		
		sym_free(partial_error);
	}	
	return prob;
}


multi_composition_error_model_data error_model_multi_builder(unsigned n_models, ...)
{
	multi_composition_error_model_data model_data;
	model_data.n_models = n_models;
	model_data.model_split = (unsigned*)malloc(sizeof(unsigned) * model_data.n_models);
	model_data.error_models = (error_model_f*)malloc(sizeof(error_model_f) * model_data.n_models);
	model_data.error_models_data = (void**)malloc(sizeof(void*) * model_data.n_models);

	unsigned counter = 0;

	va_list argv;
	va_start(argv, n_models);

	enum mode {MODEL_SPLIT_e, ERROR_MODELS_e, MODEL_DATA_e, TERM_e};
	unsigned current_mode = MODEL_SPLIT_e;

	while (current_mode != TERM_e)
	{
		switch (current_mode)
		{
			case MODEL_SPLIT_e:
				model_data.model_split[counter] = va_arg(argv, unsigned);
			break;

			case ERROR_MODELS_e:
				model_data.error_models[counter] = va_arg(argv, error_model_f);
			break;

			case MODEL_DATA_e:
				model_data.error_models_data[counter] = va_arg(argv, void*);
			break;
		}

		counter++;

		if (counter == model_data.n_models)
		{
			current_mode++;
			counter = 0;
		}
	}
	va_end(argv);
	return model_data;
}

void error_model_multi_free(multi_composition_error_model_data* model_data)
{
	free(model_data->model_split);
	free(model_data->error_models);
	free(model_data->error_models_data);
	return;
}

// Lookup Model Composition -------------------------------------------------------------------------------
 
typedef struct {
	double* lookup_table;
} lookup_error_model_data;

double error_model_lookup(const sym* error, void* v_model_data)
{
	lookup_error_model_data* model_data = (lookup_error_model_data*)v_model_data;
	return model_data->lookup_table[sym_to_ll(error)];
}


#endif
