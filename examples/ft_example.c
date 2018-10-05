//#define GATE_MULTITHREADING
//#define N_THREADS 4

#include "../sym.h"
#include <float.h>

#include "../codes/codes.h"
#include "../codes/candidate_codes.h"

#include "../decoders/tailored.h"

#include "../gates/clifford_generators.h"
#include "../gates/pauli_generators.h"
#include "../gates/measurement.h"
#include "../gates/preparation.h"

#include "../circuits/encoding.h"
//#include "../circuits/syndrome_measurement_flag_ft.h"
#include "../circuits/recovery.h"

#include "../error_models/iid.h"
#include "../error_models/lookup.h"
#include "../characterise.h"

#include "../misc/progress_bar.h"
#include "../misc/qcircuit.h"

int main()
{

	double p_gate_error = 0.0001;
	double p_wire_error = 0.0001;

	unsigned n_qubits = 7;
	unsigned n_ancilla_qubits = 6;

	sym* code = code_candidate_seven_a();
	sym* logicals = code_candidate_seven_a_logicals();
	
	// Build our circuit with noise included:
	error_model* em_cnot = error_model_create_iid(2, p_gate_error);
	error_model* em_gate = error_model_create_iid(1, p_gate_error);

	gate* cnot = gate_create(2, gate_cnot, em_cnot,	NULL);
	gate* hadamard = gate_create(1, gate_hadamard, em_gate, NULL);
	gate* phase = gate_create(1, gate_phase, em_gate, NULL);

	gate* pauli_X = gate_create(1, gate_pauli_X, em_gate, NULL);
	gate* pauli_Z = gate_create(1, gate_pauli_Z, em_gate, NULL);
	
	gate* prepare_syndromes = gate_create_prepare_X(1, 1, NULL);
	gate* prepare_flags = gate_create_prepare_Z(1, 1, NULL);

	gate* measure_syndromes = gate_create(1, gate_measure_Z, em_gate, NULL);
	gate* measure_flags = gate_create(1, gate_measure_X, em_gate, NULL);
	
	// Create our encoding circuit
	circuit* encode = encoding_circuit(code, logicals, cnot, hadamard, phase);

	qcircuit_print(encode);
	//double* initial_error_probs = error_probabilities_identity(n_qubits);
	
	// And the syndrome measurement circuit
	/*circuit* syndromes = syndrome_measurement_flag_ft_circuit_create(
		code, 
		cnot, 
		hadamard, 
		phase,
		pauli_X,
		pauli_Z,
		prepare_syndromes,
		prepare_flags,
		measure_syndromes,
		measure_flags);*/

	//qcircuit_print(syndromes);


	/*
	// Setup the error model
	error_model* local_noise_model = error_model_create_iid(1, p_wire_error);
	gate* iid_error_gate = gate_create_iid_noise(local_noise_model);

	// Run the circuit
	double* initial_error_probs = error_probabilities_identity(n_qubits);
	double* encoded_error_probs = circuit_run(encode, initial_error_probs, iid_error_gate);
	double* measured_error_probs = circuit_run(syndromes, encoded_error_probs, iid_error_gate);

	// The error model to feed to our decoder		
	error_model* encoding_error = error_model_create_lookup(n_qubits, encoded_error_probs);

	// Tailor the Decoder
	decoder* tailored_decoder = decoder_create_tailored(code, logicals, encoding_error);
	
	// Create the recovery circuit
	circuit* recovery = circuit_recovery_create(n_qubits, n_ancilla_qubits,	tailored_decoder, pauli_X, pauli_Z, measure_syndromes);

	double* recovered_error_probs = circuit_run(recovery, measured_error_probs, iid_error_gate);

	// Have a look at the code now
	double* probabilities = characterise_code_corrected(code, logicals, recovered_error_probs);
	printf("P Corrected: %e\n", probabilities[0]);
	*/
	// Free allocated objects
	//error_model_free(local_noise_model);
	//error_model_free(encoding_error);

	//gate_free(iid_error_gate);
	
	/*error_probabilities_free(initial_error_probs);
	error_probabilities_free(encoded_error_probs);
	error_probabilities_free(measured_error_probs);
	error_probabilities_free(recovered_error_probs);
	error_probabilities_free(probabilities);
	*/
	sym_free(code);
	sym_free(logicals);
	gate_free(hadamard);
	gate_free(cnot);
	gate_free(phase);
	return 0;	
}
