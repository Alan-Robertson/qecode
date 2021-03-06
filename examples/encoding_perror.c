#define GATE_MULTITHREADING
#define N_THREADS 4

#include "../sym.h"
#include <float.h>

#include "../codes/random_code_search.h"
#include "../codes/candidate_codes.h"
#include "../decoders/tailored.h"

#include "../gates/clifford_generators.h"
#include "../circuits/encoding.h"
#include "../circuits/circuit_search.h"

#include "../error_models/iid.h"
#include "../error_models/iid_biased.h"

#include "../error_models/lookup.h"
#include "../error_models/error_model_debug.h"
#include "../characterise.h"

#include "../misc/progress_bar.h"

int main()
{	
	unsigned n_increments = 13;

	double init_p_error = 0.02;
	double delta = 0.5;

	double logical_rate[24];

	unsigned n_qubits = 7, n_logicals = 1, distance = 3;

	sym* code = code_cyclic_seven();
	sym* logicals = code_cyclic_seven_logicals();

	double error_rate = init_p_error;
	double gate_error = init_p_error;	
	progress_bar* p = progress_bar_create(n_increments, "Steane Code Bias: ");
	for (unsigned i = 0; i < n_increments; i++)
	{
		error_rate *= delta;
		gate_error *= delta;

		// Build our circuit with noise included:
		error_model* gate_noise = error_model_create_iid(1, gate_error);
		error_model* cnot_noise = error_model_create_iid(2, gate_error);
		
		// Setup the error model
		error_model* local_noise_model = error_model_create_iid(1, error_rate);
		
		// Construct the gates, including the error gate
		gate* iid_error_gate = gate_create_iid_noise(local_noise_model);
		gate* cnot = gate_create(2, gate_cnot, cnot_noise, NULL);
		gate* hadamard = gate_create(1, gate_hadamard, gate_noise, NULL);
		gate* phase = gate_create(1, gate_phase, gate_noise, NULL);

		circuit* encode = encoding_circuit(code, logicals, cnot, hadamard, phase);

		double* initial_error_probs = error_probabilities_identity(n_qubits);
		double* encoded_error_probs = circuit_run(encode, initial_error_probs, iid_error_gate);

		// The error model to feed to our decoder		
		error_model* encoding_error = error_model_create_lookup(n_qubits, encoded_error_probs);
		
		// Tailoring the decoder
		decoder* tailored = decoder_create_tailored(code, logicals, encoding_error);

		// Find the logical correction rates
		double* probabilities = characterise_code(code, logicals, encoding_error, tailored);

		logical_rate[i] = probabilities[0];

		// Free allocated objects
		error_probabilities_free(initial_error_probs);
		error_probabilities_free(encoded_error_probs);
		error_probabilities_free(probabilities);

		error_model_free(local_noise_model);
		error_model_free(gate_noise);
		error_model_free(cnot_noise);
		error_model_free(encoding_error);

		gate_free(hadamard);
		gate_free(cnot);
		gate_free(phase);
		gate_free(iid_error_gate);

		decoder_free(tailored);

		circuit_free(encode);

		progress_bar_update(p);
	}
	progress_bar_free(p);

	printf("Physical Rate \t Logical Rate\n");
	error_rate = init_p_error;
	printf("((");
	for (unsigned i = 0; i < n_increments; i++)
	{
		error_rate *= delta;
		printf("(%e, %e)", error_rate, logical_rate[i]);
		if (i < n_increments - 1)
		{
			printf(",");
		}
		printf("\n");
	}
	printf("))\n");

	sym_free(code);
	sym_free(logicals);

	return 0;	
}
