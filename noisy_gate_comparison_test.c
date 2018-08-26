#include "sym.h"
#include <float.h>

#include "codes/codes.h"
#include "decoders/tailored.h"

#include "gates/clifford_generators.h"
#include "gates/pauli_generators.h"
#include "gates/measurement.h"

#include "circuits/encoding.h"
#include "circuits/syndrome_measurement.h"
#include "circuits/recovery.h"

#include "error_models/iid.h"
#include "error_models/lookup.h"
#include "characterise.h"

#include "misc/progress_bar.h"

int main()
{	
	double rate_min = 0.000001, rate_delta = 1.25;
	unsigned n_increments = 20;

	double p_gate_error = 0.00001;

	double logical_rate[20];
	double approx_logical_rate[20];

	unsigned n_qubits = 7;
	unsigned n_ancilla_qubits = 6;

	sym* code = code_steane();
	sym* logicals = code_steane_logicals();
	
	// Build our circuit with noise included:
	// Build our circuit with noise included:
	error_model* em_cnot = error_model_create_iid(2, p_gate_error);
	error_model* em_gate = error_model_create_iid(1, p_gate_error);

	gate* cnot = gate_create(2, gate_cnot, em_cnot,	NULL);
	gate* hadamard = gate_create(1, gate_hadamard, em_gate, NULL);
	gate* phase = gate_create(1, gate_phase, em_gate, NULL);
	gate* pauli_X = gate_create(1, gate_pauli_X, em_gate, NULL);
	gate* pauli_Z = gate_create(1, gate_pauli_Z, em_gate, NULL);
	gate* measure_syndromes = gate_create(n_ancilla_qubits, gate_measure_Z, em_gate, NULL);
	
	// Create our encoding circuit
	circuit* encode = encoding_circuit(code, logicals, cnot, hadamard, phase);

	// And the syndrome measurement circuit
	circuit* syndromes = syndrome_measurement_circuit_create(code, cnot, hadamard, phase);

	progress_bar* b = progress_bar_create(n_increments, "Iterations");
	for (unsigned i = 0; i < n_increments; i++)
	{
		progress_bar_update(b);

		// Set the error rate
		double error_rate = rate_min * pow(rate_delta, i); 

		// Setup the error model
		error_model* local_noise_model = error_model_create_iid(1, error_rate);
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

		double* probabilities = characterise_code_corrected(code, logicals, recovered_error_probs);
		logical_rate[i] = probabilities[0];

		// Free allocated objects
		error_model_free(local_noise_model);
		error_model_free(encoding_error);

		gate_free(iid_error_gate);
		
		error_probabilities_free(initial_error_probs);
		error_probabilities_free(encoded_error_probs);
		error_probabilities_free(measured_error_probs);
		error_probabilities_free(recovered_error_probs);
		error_probabilities_free(probabilities);
	}
	progress_bar_free(b);

	printf("Physical Rate \t Logical Rate\n");
	for (unsigned i = 0; i < n_increments; i++)
	{
		printf("%.15f \t %.15f", rate_min * pow(rate_delta, i), logical_rate[i]);
		printf("\n");
	}

	sym_free(code);
	sym_free(logicals);
	gate_free(hadamard);
	gate_free(cnot);
	gate_free(phase);
	return 0;	
}