#include "codes/codes.h"
#include "gates/clifford_generators.h"
#include "error_models/iid.h"

#include "circuits/encoding.h"
#include "circuits/decoding.h"

#include "characterise.h"
#include "misc/qcircuit.h"

int main()
{	
	unsigned n_qubits = 7;

	double p_gate_error = 0.01; // Gates themselves are noiseless
	double p_error = 0.01;

	sym* code = code_steane();
	sym* logicals = code_steane_logicals();

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
	circuit* enc_circuit = encoding_circuit(code, logicals, cnot, hadamard, phase);
	circuit* dec_circuit = decoding_circuit_from_encoding(enc_circuit);

	// Print the circuit
	printf("Encoding: \n");
	qcircuit_print(enc_circuit);

	printf("Decoding: \n");
	qcircuit_print(dec_circuit);

	// Run the circuit
	double* initial_error_probs = error_probabilities_identity(n_qubits);
	double* encoded_error_probs = circuit_run_noiseless(enc_circuit, initial_error_probs);
	double* decoded_error_probs = circuit_run_noiseless(dec_circuit, encoded_error_probs);

	printf("\n\n");
	//characterise_print(decoded_error_probs, n_qubits);

	// Cleanup
	error_model_free(em_cnot);
	error_model_free(em_gate);
	error_model_free(em_noise);

	error_probabilities_free(initial_error_probs);
	error_probabilities_free(encoded_error_probs);
	error_probabilities_free(decoded_error_probs);
	
	circuit_free(enc_circuit);	
	circuit_free(dec_circuit);

	gate_free(cnot);
	gate_free(hadamard);
	gate_free(phase);
	gate_free(noise);

	return 0;
}