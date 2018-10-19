#include "../codes/codes.h"
#include "../codes/candidate_codes.h"

#include "../decoders/destabiliser.h"
#include "../decoders/tailored.h"

#include "../gates/measurement.h"
#include "../gates/preparation.h"
#include "../gates/pauli_generators.h"
#include "../gates/clifford_generators.h"

#include "../error_models/lookup.h"

#include "../circuits/syndrome_measurement_flag_ft.h"

#include "../sym_iter.h"
#include "../characterise.h"

/*
 *	Check the recovery circuit
 *
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
	gate* cnot = gate_create(2, gate_cnot, NULL, NULL);
	gate* hadamard = gate_create(1, gate_hadamard, NULL, NULL);
	gate* phase = gate_create(1, gate_phase, NULL, NULL);


	//--------------------------------
	// Setup our initial error probabilities
	//--------------------------------
	double* initial_error_probs = error_probabilities_zeros(n_qubits);

	// Generate some single qubit X and Z errors
	double prob = (1.0) / (code->n_qubits * 3);
	for (int i = 0; i < code->length; i++)
	{
		initial_error_probs[1 << i] = prob;
	}

	// Generate some single qubit Y errors
	for (int i = 0; i < code->n_qubits; i++)
	{
		initial_error_probs[((1 << code->n_qubits) + 1) << i] = prob;
	}

	printf("qwop\n");
	//--------------------------------------------
	// Use the above to build what we need to
	//-------------------------------------------

	// Treat the above as a lookup based error model
	error_model* em =  error_model_create_lookup(n_qubits, initial_error_probs);
	// Note that we're going to pass this into our circuit, you might want to run the circuit using 
	// The distribution as the input, then use the output of the measurement circuit 
	// as the error model to create the decoder

	// Tailor a decoder
	decoder* tailored = decoder_create_tailored(code, logicals, em);


	printf("qwop\n");

	// Get our recovery circuit for the tailored
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


	printf("qwop\n");
	double* recovered_state = circuit_run(flag_ft_measurement, initial_error_probs, NULL);	
	
	printf("Tailored Results\n");
	//logical_rates = characterise_code_corrected(code, logicals, recovered_state);
	/*siter = sym_iter_create_n_qubits(logicals->n_qubits ++);
	while(sym_iter_next(siter))
	{
		printf("%e \t", logical_rates[sym_iter_ll_from_state(siter)]);
		sym_print(siter->state);
	}*/

	return 0;
}