#include "../error_models/iid.h"
#include "../gates/clifford_generators.h"
#include "../circuits/circuit.h"

#include "../codes/candidate_codes.h"

#include "../characterise.h"

int main()
{	
	unsigned n_qubits = 2;

	double p_gate_error = 0; // Gates themselves are noiseless
	double p_error = 0.05;

	// Build our circuit with noise included:
	error_model* em_cnot = error_model_create_iid(2, p_gate_error);
	error_model* em_gate = error_model_create_iid(1, p_gate_error);

	gate* cnot = gate_create(2,  
		gate_cnot,
		em_cnot,
		NULL);

	gate* hadamard = gate_create(1,  
		gate_hadamard,
		em_gate,
		NULL);

	gate* phase = gate_create(1,  
		gate_phase,
		em_gate,
		NULL);

	// Create our iid noise
	error_model* em_noise = error_model_create_iid(1, p_error);
	gate* noise = gate_create(1, gate_iid, em_noise, NULL);

	// Create our circuit
	circuit* test_circuit = circuit_create(n_qubits);

	circuit_add_gate(test_circuit, noise, 0);
	circuit_add_gate(test_circuit, cnot, 0, 1);
	circuit_add_gate(test_circuit, hadamard, 0);
	circuit_add_gate(test_circuit, phase, 0);

	// Run the circuit
	double* initial_error_probs = error_probabilities_identity(n_qubits);
	double* final_error_probs = circuit_run_noiseless(test_circuit, initial_error_probs);

	characterise_print(final_error_probs, n_qubits);

	// Cleanup
	error_model_free(em_cnot);
	error_model_free(em_gate);
	error_model_free(em_noise);

	error_probabilities_free(initial_error_probs);
	error_probabilities_free(final_error_probs);
	
	circuit_free(test_circuit);

	gate_free(cnot);
	gate_free(hadamard);
	gate_free(phase);
	gate_free(noise);

	return 0;

}