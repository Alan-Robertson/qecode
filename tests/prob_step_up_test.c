#include "../error_models/iid.h"
#include "../gates/error.h"
#include "../characterise.h"
#include "../circuits/error_probabilities.h"

int main()
{
	uint32_t initial_qubits = 3;
	uint32_t final_qubits = 2;

	double p_error = 0.01;

	error_model* em_noise = error_model_create_iid(1, p_error);
	gate* noise = gate_create_error(1, em_noise);

	uint32_t* target_qubits = (uint32_t*)malloc(sizeof(uint32_t) * initial_qubits);
	for (uint32_t i = 0; i < initial_qubits - 1; i++)
	{
		target_qubits[i] = i;
	}

	double* initial_probs = error_probabilities_identity(initial_qubits);
	double* noisy_probs = gate_apply(initial_qubits, initial_probs, noise, target_qubits);

	characterise_print(noisy_probs, initial_qubits);
	printf("Gate applied: %e\n\n\n", 1.0 - characterise_test(noisy_probs, initial_qubits));

	double* step_up_probs = error_probabilities_step(noisy_probs, initial_qubits, final_qubits);
	characterise_print(step_up_probs, final_qubits);
	printf("Gate applied: %e\n\n\n", 1.0 - characterise_test(step_up_probs, final_qubits));

	free(target_qubits);
	return 0;
}