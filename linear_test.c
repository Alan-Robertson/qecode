#include "error_models/iid.h"
#include "gates/clifford_generators.h"
#include "gates/measurement.h"
#include "misc/qcircuit.h"
#include "circuits/syndrome_measurement_sequential.h"
#include "characterise.h"
#include "codes/codes.h"

int main()
{	
	unsigned n_qubits = 5;
	unsigned n_ancilla_qubits = 4;

	double p_gate_error = 0; // Gates themselves are noiseless
	double p_error = 0.0001;

	sym* code = code_five_qubit();
	sym* logicals = code_five_qubit_logicals();

	gate* cnot = gate_create_noiseless(2, gate_cnot);
	gate* hadamard = gate_create_noiseless(1, gate_hadamard);
	gate* phase = gate_create_noiseless(1, gate_phase);
	gate* measure_Z = gate_create_noiseless(1, gate_measure_Z);

	// Create our iid noise
	error_model* em_noise = error_model_create_iid(1, p_error);
	gate* passive_noise = gate_create_iid_noise(em_noise);

	// Create our circuit
	circuit* test_circuit = syndrome_measurement_sequential_circuit(code, cnot, hadamard, phase, measure_Z);

	// Print the circuit
	qcircuit_print(test_circuit);

	// Run the circuit
	double* initial_error_probs = error_probabilities_identity(n_qubits);
	double* final_error_probs = circuit_run(test_circuit, initial_error_probs, passive_noise);

	//printf("\n\n");
	//characterise_print(final_error_probs, n_qubits + n_ancilla_qubits);
	printf("Sum of probabilities: %e\n", characterise_test(final_error_probs, n_qubits + n_ancilla_qubits));
	// Cleanup
	error_model_free(em_noise);

	//error_probabilities_free(initial_error_probs);
	//error_probabilities_free(final_error_probs);
	
	circuit_free(test_circuit);

	gate_free(cnot);
	gate_free(hadamard);
	gate_free(phase);
	gate_free(measure_Z);

	return 0;
}