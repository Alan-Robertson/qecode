//#define GATE_MULTITHREADING
//#define N_THREADS 4

#define CIRCUIT_PRINT_PROGRESS

#include "../sym.h"
#include <float.h>

#include "../codes/random_code_search.h"
#include "../codes/candidate_codes.h"
#include "../decoders/tailored.h"

#include "../gates/clifford_generators.h"
#include "../gates/pauli_generators.h"

#include "../circuits/syndrome_measurement.h"
#include "../circuits/circuit_search.h"

#include "../error_models/iid.h"
#include "../error_models/iid_biased.h"

#include "../error_models/lookup.h"
#include "../error_models/error_model_debug.h"
#include "../characterise.h"

#include "../misc/progress_bar.h"

#define N_INCREMENTS 1

int main()
{	
	double logical_rate[N_INCREMENTS];

	sym* code = code_candidate_seven_c();
	sym* logicals = code_candidate_seven_c_logicals();


	unsigned n_qubits = code->n_qubits;
	unsigned n_logicals = 1;
	unsigned distance = 3;

	uint32_t* preparation_targets = (uint32_t*)malloc(sizeof(uint32_t) * n_qubits);
	for (uint32_t j = 0; j < n_qubits; j++)
	{
		preparation_targets[j] = j;
	}


	double error_rate = 0.000000001;
	double gate_error = 0;//.000000001;
	double bias = 1;	


		// Initial noise
		error_model* initial_noise = error_model_create_iid_biased_Z(n_qubits, error_rate, bias);

		// Build our circuit with noise included:
		error_model* gate_noise = error_model_create_iid_biased_X(1, gate_error, bias);
		error_model* cnot_noise = error_model_create_iid_biased_X(2, gate_error, bias);
		
		// Setup the error model
		error_model* wire_noise = error_model_create_iid_biased_X(1, error_rate, bias);
		
		// The state preparation gate
		gate* preparation = gate_create(n_qubits, NULL, gate_noise, NULL);

		// Construct the gates, including the error gate
		gate* wire = gate_create(1, NULL, wire_noise, NULL);
		gate* cnot = gate_create(2, gate_cnot, cnot_noise, NULL);
		gate* hadamard = gate_create(1, gate_hadamard, gate_noise, NULL);
		gate* phase = gate_create(1, gate_phase, gate_noise, NULL);

		gate* pauli_X = gate_create(1, gate_pauli_X, 0, NULL);
		gate* pauli_Z = gate_create(1, gate_pauli_Z, 0, NULL);

		// Our syndrome measurement circuit
		circuit* measure = syndrome_measurement_circuit_create(code, cnot, hadamard, phase);

		// Build a recovery circuit
		// circuit* recovery = circuit_recovery_create(
		// 	n_qubits,
		// 	n_ancilla_qubits,
		// 	tailored,
		// 	pauli_X,
		// 	pauli_Z,
		// 	measure_ancillas);

		double* initial_error_probs = error_probabilities_identity(n_qubits);


		//double* prepared_error_probs = gate_apply(n_qubits, initial_error_probs, preparation, preparation_targets);
		double* measured_error_probs = circuit_run(measure, initial_error_probs, wire);

		// The error model to feed to our decoder		
		error_model* measured_error = error_model_create_lookup(n_qubits, measured_error_probs);
		
		// Tailoring the decoder
		decoder* tailored = decoder_create_tailored(code, logicals, measured_error);

		// Find the logical correction rates
		double* probabilities = characterise_code(code, logicals, measured_error, tailored);

		logical_rate[0] = probabilities[0];

	printf("Physical Rate \t Effective Rate \t Logical Rate\n");
	printf("((");
	for (unsigned i = 0; i < N_INCREMENTS; i++)
	{
		printf("(%e, %e, %e)", error_rate, 1.0 - measured_error_probs[0], logical_rate[i]);
		if (i < N_INCREMENTS - 1)
		{
			printf(",");
		}
		printf("\n");
	}
	printf("))\n");

	sym_free(code);
	sym_free(logicals);
	free(preparation_targets);

	return 0;	
}
