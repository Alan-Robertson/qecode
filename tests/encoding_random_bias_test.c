#include "sym.h"
#include <float.h>

#include "codes/random_code_search.h"
#include "decoders/tailored.h"

#include "gates/clifford_generators.h"
#include "circuits/encoding.h"
#include "circuits/circuit_search.h"

#include "error_models/iid_biased.h"

#include "error_models/lookup.h"
#include "characterise.h"

#include "misc/progress_bar.h"

int main()
{	
	unsigned n_increments = 15;

	double error_rate = 0.0001;
	double gate_error = 0.0003;

	double logical_rate[30];
	uint32_t n_codes_searched = 10;

	unsigned n_qubits = 7, n_logicals = 1, distance = 3;

	sym* code = code_steane();
	sym* logicals = code_steane_logicals();

	double bias = 0.5;	
	progress_bar* b = progress_bar_create(n_increments, "Iterations");
	for (unsigned i = 0; i < n_increments; i++)
	{
		bias *= 4;
		progress_bar_update(b);

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

		// Setup the error model
		error_model* local_noise_model = error_model_create_iid_biased_Z(1, error_rate, bias);
		gate* iid_error_gate = gate_create_iid_noise(local_noise_model);

		// Set the error rate
		random_code_return r = circuit_search_stabiliser(
			n_qubits, 
			n_logicals, 
			distance, 
			n_codes_searched,
			encoding_circuit,
			cnot,
			hadamard,
			phase,
			iid_error_gate);

		logical_rate[i] = r.p_correction;

		// Free allocated objects
		error_model_free(local_noise_model);
		sym_free(r.code);
		sym_free(r.logicals);
			
		gate_free(hadamard);
		gate_free(cnot);
		gate_free(phase);
	}
	progress_bar_free(b);

	printf("Physical Rate \t Logical Rate");
	bias = 0.5;
	for (unsigned i = 0; i < n_increments; i++)
	{
		bias *= 2;
		printf("%.15f \t %.15f", bias, logical_rate[i]);
		printf("\n");
	}

	return 0;	
}