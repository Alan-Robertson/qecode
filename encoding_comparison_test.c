#include "sym.h"
#include <float.h>

#include "codes/codes.h"
#include "decoders/tailored.h"

#include "gates/clifford_generators.h"
#include "circuits/encoding.h"

#include "error_models/iid.h"
#include "error_models/lookup.h"
#include "characterise.h"

#include "misc/progress_bar.h"

int main()
{	
	double rate_min = 0.0000001, rate_delta = 1.25;
	unsigned n_increments = 40;

	double logical_rate[40];
	double approx_logical_rate[40];

	double approximate_errors[40] = {
		0.99998975007783408, 0.99998718762161376, 0.99998398456503157, 0.9999799807656633,
		0.99997497604987351, 0.99996872020731142, 0.99996090048566533, 0.99995112596102165,
		0.99993890800430163, 0.9999236358694652, 0.99990454618694402, 0.99988068484318171,
		0.99985085934997775, 0.99981357933728554, 0.99976698221782467, 0.99970874034378121,
		0.99963594507130038, 0.99954496202635934, 0.99943125047642922, 0.99928913799682983,
		0.99911153950901066, 0.99888960717939979, 0.99861229451113709, 0.99826581413921478,
		0.99783296425449086, 0.99729229314931556, 0.99661706504906966, 0.99577398320019939,
		0.99472161829800565, 0.99340848218005684, 0.99177067912175432, 0.98972906155994811,
		0.98718581620538093, 0.98402041447169997, 0.98008488454702103, 0.97519841135555541,
		0.96914136003532081, 0.96164896966971647, 0.95240520568786979, 0.94103762788716738};

	unsigned n_qubits = 7, n_logicals = 1, distance = 3;

	sym* code = code_steane();
	sym* logicals = code_steane_logicals();
	
	// Build our circuit with noise included:
	gate* cnot = gate_create_noiseless(2, gate_cnot);
	gate* hadamard = gate_create_noiseless(1, gate_hadamard);
	gate* phase = gate_create_noiseless(1, gate_phase);
	
	// Create our circuit
	circuit* test_circuit = encoding_circuit(code, logicals, cnot, hadamard, phase);

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
		double* final_error_probs = circuit_run(test_circuit, initial_error_probs, iid_error_gate);
		

		error_model* approximate_error = error_model_create_iid(n_qubits, 1 - approximate_errors[i]);

		// Tailor the Decoder 
		double* probabilities = characterise_code_corrected(code, logicals, final_error_probs);
		logical_rate[i] = probabilities[0];

		// Tailor the Decoder
		decoder* tailored_decoder_approx = decoder_create_tailored(code, logicals, approximate_error);

		double* approximate_probabilities = characterise_code(code, logicals, approximate_error, tailored_decoder_approx);
		approx_logical_rate[i] = approximate_probabilities[0];

		// Free allocated objects
		error_model_free(local_noise_model);
		
		error_probabilities_free(initial_error_probs);
		error_probabilities_free(final_error_probs);
		error_probabilities_free(probabilities);
		
		decoder_free(tailored_decoder_approx);
	}
	progress_bar_free(b);

	printf("Physical Rate \t Logical Rate \t Approx Logical Rate\n");
	for (unsigned i = 0; i < n_increments; i++)
	{
		printf("%.15f \t %.15f \t %.15f", rate_min * pow(rate_delta, i), logical_rate[i], approx_logical_rate[i]);
		printf("\n");
	}

	sym_free(code);
	sym_free(logicals);
	gate_free(hadamard);
	gate_free(cnot);
	gate_free(phase);
	return 0;	
}