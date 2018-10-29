//#define GATE_MULTITHREADING
//#define N_THREADS 2

#define CIRCUIT_SYNDROME_MEASUREMENT_FLAG_FT_CIRCUIT_PRINT_PROGRESS

#include "../codes/codes.h"
#include "../codes/candidate_codes.h"

#include "../decoders/destabiliser.h"
#include "../decoders/tailored.h"

#include "../gates/measurement.h"
#include "../gates/preparation.h"
#include "../gates/pauli_generators.h"
#include "../gates/clifford_generators.h"

#include "../error_models/lookup.h"
#include "../error_models/iid.h"

#include "../circuits/syndrome_measurement_flag_ft.h"

#include "../sym_iter.h"
#include "../characterise.h"

#include "../misc/qcircuit.h"




/*
 *	Check the recovery circuit
 */

int main()
{	
	//--------------------------------
	// Setup the code and the space of qubits
	//--------------------------------
	sym* code = code_five_qubit();
	sym* logicals = code_five_qubit_logicals();

	unsigned n_qubits = code->n_qubits;
	unsigned n_ancilla_qubits = code->height;
	unsigned n_flag_qubits = 2;

	// Setup error models

	double p_gate_error = 0;
	double p_wire_error = p_gate_error / 100;

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

	gate* wire_noise = gate_create(1, NULL, em_wire, NULL);

	//--------------------------------
	// Setup our initial error probabilities
	//--------------------------------
	double* initial_error_probs = error_probabilities_identity(n_qubits);
	initial_error_probs[0] = 1;

	// X error on a qubit, let's try to propagate it

	// Generate some single qubit X and Z errors
	/*double prob = (1.0) / (code->n_qubits * 3);
	for (int i = 0; i < code->length; i++)
	{
		initial_error_probs[1 << i] = prob;
	}*/

	// Generate some single qubit Y errors
	/*for (int i = 0; i < code->n_qubits; i++)
	{
		initial_error_probs[((1 << code->n_qubits) + 1) << i] = prob;
	}*/

	//--------------------------------------------
	// Use the above to build what we need to
	//-------------------------------------------

	// Create an iid error model
	error_model* em =  error_model_create_iid(n_qubits, p_gate_error);
	// Note that we're going to pass this into our circuit, you might want to run the circuit using 
	// The distribution as the input, then use the output of the measurement circuit 
	// as the error model to create the decoder

	

	// Build a flag fault tolerant measurement circuit
	circuit* flag_ft_measurement = syndrome_measurement_flag_ft_circuit_create(
    code,
    cnot,
    hadamard,
    phase,
    pauli_X,
    pauli_Z,
    prepare_X,
    prepare_Z,
    measure_flags, 
    measure_ancillas);

    // Tailor a decoder
	decoder* tailored_decoder = decoder_create_tailored(code, logicals, em);

	// Tailor a decoder
	decoder* iid_decoder = decoder_create_tailored(code, logicals, em);

	// Build a recovery circuit
	circuit* recovery_iid = circuit_recovery_create(
		n_qubits,
		n_ancilla_qubits,
		iid_decoder,
		pauli_X,
		pauli_Z,
		measure_ancillas);

	// Build a recovery circuit
	circuit* recovery_tailored = circuit_recovery_create(
		n_qubits,
		n_ancilla_qubits,
		tailored_decoder,
		pauli_X,
		pauli_Z,
		measure_ancillas);

	printf("Measuring the state\n");
	double* measured_state = circuit_run(flag_ft_measurement, initial_error_probs, wire_noise);	
	
	printf("Recovering the state\n");
	double* recovered_state_iid = circuit_run(recovery_iid, measured_state, NULL);
	double* recovered_state_tailored = circuit_run(recovery_tailored, measured_state, NULL);

	printf("Results\n");
	double* logical_rates_iid = characterise_code_corrected(code, logicals, recovered_state_iid);
	double* logical_rates_tailored = characterise_code_corrected(code, logicals, recovered_state_tailored);
	printf("Tailored: (%e, %e)\n", p_gate_error, logical_rates_tailored[0]);
	printf("IID: (%e, %e)\n", p_gate_error, logical_rates_iid[0]);

	// Cleanup
	sym_multi_free(2, code, logicals);	
	gate_multi_free(9, pauli_X, pauli_Z, prepare_X, prepare_Z, measure_flags, measure_ancillas, cnot, hadamard, phase);

	error_model_free(em);
	error_model_free(em_cnot);

	//decoder_free(tailored);

	free(initial_error_probs);
	circuit_free(flag_ft_measurement);
	//circuit_free(recovery);

	//free(measured_state);
	//free(recovered_state);
	return 0;
}