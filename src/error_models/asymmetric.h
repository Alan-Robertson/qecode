#ifndef ERROR_MODEL_ASYMMETRIC
#define ERROR_MODEL_ASYMMETRIC

#include "error_models.h"

// Spatially Asymmetric Noise -------------------------------------------------------------------------------

struct model_params_spatially_asymmetric {
	unsigned n_bitflip_qubits;
	double p_bitflip;
	unsigned n_phaseflip_qubits;
	double p_phaseflip;
};

error_model* error_model_create_spatially_asymmetric(
	const unsigned n_bitflip_qubits, 
	const double p_bitflip, 
	const unsigned n_phaseflip_qubits, 
	const double p_phaseflip)
{
	error_model* m = error_model_create(sizeof(model_params_spatially_asymmetric));
	struct model_params_spatially_asymmetric* mp = (struct model_params_spatially_asymmetric*)malloc(sizeof(model_params_spatially_asymmetric));

	mp->n_bitflip_qubits = n_bitflip_qubits;
	mp->p_bitflip = p_bitflip;
	mp->n_phaseflip_qubits = n_phaseflip_qubits;
	mp->p_phaseflip = p_phaseflip;

	m->model_call = error_model_call_bit_flip_trivial;
	m->model_params = mp;
}



double error_model_call_spatially_asymmetric(const sym* error, void* v_model_params)
{
	struct model_params_spatially_asymmetric* model_params = (struct model_params_spatially_asymmetric*) v_model_params;
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

#endif