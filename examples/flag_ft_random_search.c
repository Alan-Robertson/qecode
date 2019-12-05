//#define GATE_MULTITHREADING
//#define N_THREADS 4

#define CIRCUIT_PRINT_PROGRESS

#include "../sym.h"
#include <float.h>

#include "../codes/random_codes.h"
#include "../decoders/tailored.h"

#include "../gates/clifford_generators.h"
#include "../gates/pauli_generators.h"
#include "../gates/measurement.h"
#include "../gates/preparation.h"

#include "../circuits/syndrome_measurement_flag_ft.h"
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

	unsigned n_qubits = 7;
	unsigned n_logicals = 1;
	unsigned distance = 3;

	unsigned n_flag_qubits = 2;
	unsigned n_ancilla_qubits = 6;

	unsigned n_iterations = 1000;

	double current_best = 0;

	uint32_t* preparation_targets = (uint32_t*)malloc(sizeof(uint32_t) * n_qubits);
	for (uint32_t j = 0; j < n_qubits; j++)
	{
		preparation_targets[j] = j;
	}

	double error_rate = 0.00001;
	double gate_error = 0.00001;
	double bias = 1;	


		// Initial noise
		error_model* initial_noise = error_model_create_iid_biased_Z(n_qubits, error_rate, bias);

		// Build our circuit with noise included:
		error_model* gate_noise = error_model_create_iid_biased_Z(1, gate_error, bias);
		error_model* cnot_noise = error_model_create_iid_biased_Z(2, gate_error, bias);
		
		// Setup the error model
		error_model* wire_noise = error_model_create_iid_biased_Z(1, error_rate, bias);
		
		// The state preparation gate
		gate* preparation = gate_create(n_qubits, NULL, gate_noise, NULL);

		// Construct the gates, including the error gate
		gate* wire = gate_create(1, NULL, wire_noise, NULL);
		gate* cnot = gate_create(2, gate_cnot, cnot_noise, NULL);
		gate* hadamard = gate_create(1, gate_hadamard, gate_noise, NULL);
		gate* phase = gate_create(1, gate_phase, gate_noise, NULL);

		gate* pauli_X = gate_create(1, gate_pauli_X, gate_noise, NULL);
		gate* pauli_Z = gate_create(1, gate_pauli_Z, gate_noise, NULL);

		gate* prepare_X = gate_create_prepare_X(1, 0, NULL);
	        gate* prepare_Z = gate_create_prepare_Z(1, 0, NULL);

	// Measurement
	         gate* measure_flags = gate_create(n_flag_qubits, gate_measure_X, NULL, NULL);
	         gate* measure_ancillas = gate_create(n_ancilla_qubits, gate_measure_Z, NULL, NULL);
	

	
	for (int i = 0; i < n_iterations; i++)
	{		
		random_code_return rcr = code_random(n_qubits, n_logicals, distance);
		
	sym* code = rcr.code;
	sym* logicals = rcr.logicals;

		// Our syndrome measurement circuit
		circuit* measure = syndrome_measurement_flag_ft_circuit_create(code, cnot, hadamard, phase, pauli_X, pauli_Z, prepare_X, prepare_Z, measure_flags, measure_ancillas);

		// Build a recovery circuit
		// circuit* recovery = circuit_recovery_create(
		// 	n_qubits,
		// 	n_ancilla_qubits,
		// 	tailored,
		// 	pauli_X,
		// 	pauli_Z,
		// 	measure_ancillas);

		double* initial_error_probs = error_probabilities_identity(n_qubits);

		double* measured_error_probs = circuit_run(measure, initial_error_probs, wire);

		// The error model to feed to our decoder		
		error_model* measured_error = error_model_create_lookup(n_qubits, measured_error_probs);
		
		// Tailoring the decoder
		decoder* tailored = decoder_create_tailored(code, logicals, measured_error);

		// Find the logical correction rates
		double* probabilities = characterise_code(code, logicals, measured_error, tailored);

		if (probabilities[0] > current_best)
		{		
			printf("############\n");
			printf("%.15e\n", probabilities[0]);
			current_best = probabilities[0];
			sym_print(code);
			sym_print(logicals);
			printf("############\n");

		}

		sym_free(code);
		sym_free(logicals);
		free(measured_error_probs);
		free(probabilities);
}

	
		free(preparation_targets);

	return 0;	
}
