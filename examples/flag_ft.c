//#define GATE_MULTITHREADING
//#define N_THREADS 20
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
 *
 */

int main()
{	

	//--------------------------------
	// Setup the code and the space of qubits
	//--------------------------------
	sym* code = code_steane();
	sym* logicals = code_steane_logicals();

	unsigned n_qubits = code->n_qubits;
	unsigned n_ancilla_qubits = code->height;
	unsigned n_flag_qubits = 2;

	double p_gate_error = 0.001;
	double p_wire_error = p_gate_error / 100;

	error_model* em_cnot = error_model_create_iid(2, p_gate_error);
	error_model* em_gate = error_model_create_iid(1, p_gate_error);
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
	gate* hadamard = gate_create(1, gate_hadamard, em_gate, NULL);
	gate* phase = gate_create(1, gate_phase, em_gate, NULL);

	gate* wire = gate_create(1, NULL, em_wire, NULL);

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

	// Tailor a decoder
	decoder* tailored = decoder_create_tailored(code, logicals, em);

	// Build a recovery circuit
	circuit* recovery = circuit_recovery_create(
		n_qubits,
		n_ancilla_qubits,
		tailored,
		pauli_X,
		pauli_Z,
		measure_ancillas);

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

	/*for (int i = 0; i < code->height + 1; i++)
	{
		qcircuit_print(((circuit_syndrome_measurement_flag_ft_data_t*)flag_ft_measurement->circuit_data)->sub_circuits[i]);
	}*/

	printf("Measuring the state\n");
	double* measured_state = circuit_run(flag_ft_measurement, initial_error_probs, wire);	
	
	printf("Recovering the state\n");
	double* recovered_state = circuit_run(recovery, measured_state, wire);

	//characterise_print(recovered_state, n_qubits);
	printf("Measured: %e\n", characterise_test(measured_state, n_qubits + n_ancilla_qubits));
	printf("Recovered: %e\n", characterise_test(recovered_state, n_qubits));

	printf("Tailored Results\n");
	double* logical_rates = characterise_code_corrected(code, logicals, recovered_state);
	sym_iter* siter = sym_iter_create_n_qubits(logicals->n_qubits);
	/*while(sym_iter_next(siter))
	{
		printf("%e \t", logical_rates[sym_iter_ll_from_state(siter)]);
		sym_print(siter->state);
	}*/
	printf("(%e %e ),\n",  p_gate_error, logical_rates[0]);

	return 0;
}
