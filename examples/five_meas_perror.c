#define GATE_MULTITHREADING
#define N_THREADS 20
#define GATE_MAX_DEPTH 7

#include "../sym.h"
#include <float.h>

#include "../codes/random_code_search.h"
#include "../codes/candidate_codes.h"
#include "../decoders/tailored.h"

#include "../gates/clifford_generators.h"
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
	double init_p_error = 0.02;
	double delta = 0.5;

	double logical_rate[N_INCREMENTS];

	unsigned n_qubits = 7, n_logicals = 1, distance = 3;

	uint32_t* preparation_targets = (uint32_t*)malloc(sizeof(uint32_t) * n_qubits);
	for (uint32_t j = 0; j < n_qubits; j++)
	{
		preparation_targets[j] = j;
	}

	sym* code = code_steane();
	sym* logicals = code_steane_logicals();

	double error_rate = init_p_error;
	double gate_error = init_p_error;	
	progress_bar* p = progress_bar_create(N_INCREMENTS, "Progress: ");
	for (unsigned i = 0; i < N_INCREMENTS; i++)
	{
		error_rate *= delta;
		gate_error *= delta;

		// Initial noise
		error_model* initial_noise = error_model_create_iid(n_qubits, error_rate);

		// Build our circuit with noise included:
		error_model* gate_noise = error_model_create_iid(1, gate_error);
		error_model* cnot_noise = error_model_create_iid(2, gate_error);
		
		// Setup the error model
		error_model* wire_noise = error_model_create_iid(1, error_rate);
		
		// The state preparation gate
		gate* preparation = gate_create(n_qubits, gate_identity, initial_noise, NULL);

		// Construct the gates, including the error gate
		gate* iid_error_gate = gate_create_iid_noise(wire_noise);
		gate* cnot = gate_create(2, gate_cnot, cnot_noise, NULL);
		gate* hadamard = gate_create(1, gate_hadamard, gate_noise, NULL);
		gate* phase = gate_create(1, gate_phase, gate_noise, NULL);

		// Our syndrome measurement circuit
		circuit* measure = syndrome_measurement_circuit_create(code, cnot, hadamard, phase);

		double* initial_error_probs = error_probabilities_identity(n_qubits);
		double* prepared_error_probs = gate_apply(n_qubits, initial_error_probs, preparation, preparation_targets);
		double* measured_error_probs = circuit_run(measure, initial_error_probs, iid_error_gate);

		// The error model to feed to our decoder		
		error_model* measured_error = error_model_create_lookup(n_qubits, measured_error_probs);
		
		// Tailoring the decoder
		decoder* tailored = decoder_create_tailored(code, logicals, measured_error);

		// Find the logical correction rates
		double* probabilities = characterise_code(code, logicals, measured_error, tailored);

		logical_rate[i] = probabilities[0];

		// Free allocated objects
		error_probabilities_free(initial_error_probs);
		error_probabilities_free(prepared_error_probs);
		error_probabilities_free(measured_error_probs);
		error_probabilities_free(probabilities);

		error_model_free(initial_noise);
		error_model_free(wire_noise);
		error_model_free(gate_noise);
		error_model_free(cnot_noise);
		error_model_free(measured_error);

		gate_free(hadamard);
		gate_free(cnot);
		gate_free(phase);
		gate_free(iid_error_gate);

		decoder_free(tailored);

		circuit_free(measure);

		progress_bar_update(p);
	}
	progress_bar_free(p);

	printf("Physical Rate \t Logical Rate\n");
	error_rate = init_p_error;
	printf("((");
	for (unsigned i = 0; i < N_INCREMENTS; i++)
	{
		error_rate *= delta;
		printf("(%e, %e)", error_rate, logical_rate[i]);
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
