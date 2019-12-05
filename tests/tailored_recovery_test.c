#include "../codes/codes.h"
#include "../gates/clifford_generators.h"
#include "../gates/pauli_generators.h"
#include "../error_models/iid.h"

#include "../circuits/encoding.h"
#include "../circuits/decoding.h"
#include "../circuits/syndrome_measurement.h"
#include "../circuits/recovery.h"

#include "../characterise.h"
#include "../misc/qcircuit.h"

#include "../decoders/tailored.h"
#include "../decoders/destabiliser.h"

/*
 *	Check that some trivial error is recovered
 */

int main()
{	
	unsigned n_qubits = 5;
	unsigned n_ancilla_qubits = 4;

	double p_gate_error = 0; // Gates themselves are noiseless
	double p_error = 0;
	double p_estimated_error = 0.01;

	sym* code = code_five_qubit();
	sym* logicals = code_five_qubit_logicals();


	// Build our decoder (with an incomplete error model):
	error_model* decoder_error_model = error_model_create_iid(n_qubits, p_estimated_error);
	//decoder* destab_decoder = decoder_create_destabiliser(code, logicals);
	// Let's just use the destabiliser decoder for now

	decoder* iid_decoder = decoder_create_tailored(code, logicals, decoder_error_model);

	 // Setup error models
    error_model* em_cnot = error_model_create_iid(2, p_gate_error);
    error_model* em_hadamard = error_model_create_iid(1, p_gate_error);
    error_model* em_phase = error_model_create_iid(1, p_gate_error);
    error_model* em_wire = error_model_create_iid(1, p_wire_error);

    //--------------------------------
    // Setup our gates
    //--------------------------------
    // Paulis
    gate* pauli_X = gate_create(1, gate_pauli_X, NULL, NULL);
    gate* pauli_Z = gate_create(1, gate_pauli_Z, NULL, NULL);

    // State preparation
    gate* prepare_X = gate_create_prepare_X(1, 0, NULL);
    gate* prepare_Z = gate_create_prepare_Z(1, 0, NULL);

    // Measurement
    gate* measure_flags = gate_create(n_flag_qubits, gate_measure_X, NULL, NULL);
    gate* measure_ancillas = gate_create(n_ancilla_qubits, gate_measure_Z, NULL, NULL);

    // Cliffords 
    gate* cnot = gate_create(2, gate_cnot, em_cnot, NULL);
    gate* hadamard = gate_create(1, gate_hadamard, em_hadamard, NULL);
    gate* phase = gate_create(1, gate_phase, em_phase, NULL);

    // Wire Noise
    gate* wire_noise = gate_create(1, NULL, em_wire, NULL);



	// Create our iid noise
	error_model* em_noise = error_model_create_iid(1, p_error);
	gate* noise = gate_create(1, gate_iid, em_noise, NULL);

	// Create our circuit
	circuit* syndrome_circuit = syndrome_measurement_circuit_create(code, cnot, hadamard, phase);
	circuit* recovery_circuit = circuit_recovery_create(
		n_qubits,
		n_ancilla_qubits,
		destab_decoder,
		pauli_X,
		pauli_Z,
		measure_syndromes);

	//printf("Measurement: \n");
	//qcircuit_print(syndrome_circuit);

	// Run the circuit
	double* initial_error_probs = error_probabilities_identity(n_qubits);
	initial_error_probs[0] = 0.5;
	initial_error_probs[2] = 0.5; // THE ERROR
	
	double* syndrome_error_probs = circuit_run(syndrome_circuit, initial_error_probs, noise);
	double* recovered_error_probs = circuit_run(recovery_circuit, syndrome_error_probs, noise);
	
	//printf("\n\n");
	//characterise_print(syndrome_error_probs, n_qubits + n_ancilla_qubits);

	//printf("\n\n");
	//characterise_print(recovered_error_probs, n_qubits);

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