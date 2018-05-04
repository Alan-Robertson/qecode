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

double error_model_spatially_asymmetric(const sym* error, void* v_model_params)
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