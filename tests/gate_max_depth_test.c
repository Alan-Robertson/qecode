#define GATE_MAX_DEPTH 4
#define GATE_MULTITHREADING
#define N_THREADS 4

#include "../gates/clifford_generators.h"
#include "../error_models/iid.h"
#include "../characterise.h"
#include "../circuits/error_probabilities.h"

int main()
{
	unsigned n_qubits = 6;
	double p_gate_error = 0.001;
	double p_wire_error = 0.0001;

	// Setup our error models
	error_model* em_cnot = error_model_create_iid(2, p_gate_error);
	error_model* em_gate = error_model_create_iid(1, p_gate_error);
	error_model* em_noise = error_model_create_iid(n_qubits, p_wire_error);

	// Setup our gates
	gate* cnot = gate_create(2, gate_cnot, em_cnot, NULL);
	gate* hadamard = gate_create(2, gate_hadamard, em_gate, NULL);
	gate* phase = gate_create(2, gate_phase, em_gate, NULL);
	gate* noise = gate_create(n_qubits, gate_iid, em_noise, NULL);

	uint32_t* target_qubits = (uint32_t*)malloc(sizeof(uint32_t) * n_qubits);
	for (uint32_t i = 0; i < n_qubits; i++)
	{
		target_qubits[i] = i;
	}

	// Setup the probability distribution
	double* initial_error_probabilities = error_probabilities_identity(n_qubits);
	double* round_one_error_probabilities = gate_apply(n_qubits, initial_error_probabilities, noise, target_qubits);
	double* final_error_probabilities = gate_apply(n_qubits, round_one_error_probabilities, noise, target_qubits);

	//characterise_print(final_error_probabilities, n_qubits);
	printf("Error: %.20e\n", 1 - characterise_test(final_error_probabilities, n_qubits));
	// Cleanup
	free(target_qubits);

	error_probabilities_free(final_error_probabilities);
	error_probabilities_free(round_one_error_probabilities);
	error_probabilities_free(initial_error_probabilities);

	gate_free(noise);
	gate_free(phase);
	gate_free(hadamard);
	gate_free(cnot);

	error_model_free(em_noise);
	error_model_free(em_gate);
	error_model_free(em_cnot);
	
	return 0;
}