#include "../sym_iter.h"
#include "../codes/codes.h"
#include "../gates/clifford_generators.h"
#include "../error_models/iid.h"

#include "../circuits/encoding.h"
#include "../circuits/decoding.h"
#include "../circuits/syndrome_measurement.h"

#include "../characterise.h"
#include "../misc/qcircuit.h"

/*
	Check that when you place an error on the circuit (line 62)
	That the appropriate ancilla qubit reports an X error and will
	Provide the correct syndrome
*/

int main()
{	
	unsigned n_qubits = 5;
	unsigned n_ancilla_qubits = 4;

	double p_gate_error = 0; // Gates themselves are noiseless
	double p_error = 0.1;

	sym* code = code_five_qubit();
	sym* logicals = code_five_qubit_logicals();

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
	gate* noise = NULL;//gate_create(1, gate_iid, em_noise, NULL);

	// Create our circuit
	circuit* syndrome_circuit = syndrome_measurement_circuit_create(code, cnot, hadamard, phase);
	
	printf("Measurement: \n");
	qcircuit_print(syndrome_circuit);

	
	// Run the circuit
	double* initial_error_probs = error_probabilities_identity(n_qubits);
	initial_error_probs[0] = 0.5;
	initial_error_probs[2] = 0.5; // THE ERROR
	
	double* syndrome_error_probs = circuit_run(syndrome_circuit, initial_error_probs, noise);
	
	sym_iter_print_pauli(n_qubits + n_ancilla_qubits, syndrome_error_probs);

	return 0;
	printf("\n\n");
	characterise_print(syndrome_error_probs, n_qubits + n_ancilla_qubits);

	// Cleanup
	error_model_free(em_cnot);
	error_model_free(em_gate);
	error_model_free(em_noise);

	error_probabilities_free(initial_error_probs);
	error_probabilities_free(syndrome_error_probs);
	
	circuit_free(syndrome_circuit);	
	
	gate_free(cnot);
	gate_free(hadamard);
	gate_free(phase);
	gate_free(noise);

	sym_free(code);
	sym_free(logicals);
	return 0;
}