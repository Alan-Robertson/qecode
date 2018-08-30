#include "sym.h"
#include <float.h>

#include "codes/random_code_search.h"
#include "decoders/tailored.h"

#include "gates/clifford_generators.h"
#include "circuits/encoding.h"
#include "circuits/circuit_search.h"


#include "error_models/iid.h"
#include "error_models/iid_biased.h"

#include "error_models/lookup.h"
#include "error_models/error_model_debug.h"
#include "characterise.h"

#include "misc/progress_bar.h"

int main()
{	
	unsigned n_increments = 15;

	double error_rate = 0.0001;
	double gate_error = 0.0003;

	double logical_rate[30];

	unsigned n_qubits = 7, n_logicals = 1, distance = 3;

	sym* code = code_cyclic_seven();
	sym* logicals = code_cyclic_seven_logicals();

	double bias = 0.5;	
	for (unsigned i = 0; i < n_increments; i++)
	{
		bias *= 4;

		// Build our circuit with noise included:
		error_model* gate_noise = error_model_create_iid_biased_Z(1, gate_error, bias);
		error_model* cnot_noise = error_model_create_iid_biased_Z(2, gate_error, bias);

		// Setup the error model
		error_model* local_noise_model = error_model_create_iid_biased_Z(1, error_rate, bias);

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
		printf("Encoding Error: %e\n", error_model_debug(encoding_error, n_qubits));

		// Tailoring the decoder
		decoder* tailored = decoder_create_tailored(code, logicals, encoding_error);

		// Find the logical correction rates
		double* probabilities = characterise_code(code, logicals, encoding_error, tailored);

		logical_rate[i] = probabilities[0];

		// Free allocated objects
		error_model_free(local_noise_model);			
		gate_free(hadamard);
		gate_free(cnot);
		gate_free(phase);
	}

	printf("Physical Rate \t Logical Rate\n");
	bias = 0.5;
	for (unsigned i = 0; i < n_increments; i++)
	{
		bias *= 2;
		printf("%f \t %.15f", bias, logical_rate[i]);
		printf("\n");
	}

	return 0;	
}