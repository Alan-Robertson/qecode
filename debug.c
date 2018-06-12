#include "codes/codes.h"
#include "characterise.h"

#include "tailored.h"

#include "logical.h"

#include "decoders/tailored.h"
#include "circuits/gates.h"
#include "circuits/circuit.h"
#include "error_models/iid.h"
#include "error_models/lookup.h"
#include "circuits/error_probabilities.h"

#include "codes/low_weight.h"

int main()
{


	unsigned n_qubits = 7;
	unsigned n_logical_qubits = 1;
	double p_gate_error = 0.01;
	double p_environmental_error = 0.001;

	// Pick a code
	sym* code = code_steane();
	
	sym* low_weight = lowest_weight_rep(code);
	

	sym_free(low_weight);
	sym_free(code);
	return 0;
}

/*	int i = 0;
	sym_iter* siter = sym_iter_create(26);
	while (sym_iter_next(siter))
	{
		//sym_print(siter->state);
		i++;
	}

	printf("%d\n", i);
	return 0;*/

/*
	sym* logicals = code_five_qubit_logicals();

	// Build our circuit with noise included:
	error_model* em_cnot = error_model_create_iid(2, p_gate_error);

	gate* cnot = gate_create(2,  
			gate_cnot,
			em_cnot,
			NULL);

	error_model* em_noise = error_model_create_iid(1, p_environmental_error);
	gate* noise = gate_create_iid_noise(em_noise);

	circuit* encode = circuit_create(n_qubits);

	circuit_add_gate(encode, cnot, 0, 1);
	circuit_add_gate(encode, cnot, 0, 2);
	circuit_add_gate(encode, cnot, 1, 2);
	circuit_add_gate(encode, cnot, 2, 0);

	// Run the circuit and find the overall error rates
	double* initial_error_probs = error_probabilities_identity(n_qubits);
	double* final_error_probs = circuit_run(encode, initial_error_probs, noise);
	
	// Build an error model using the output probabilities of the circuit
	error_model* circuit_error = error_model_create_lookup(n_qubits, final_error_probs);
	
	// Build the tailored decoder
	decoder* tailored_decoder = decoder_create_tailored(code, logicals, circuit_error);

	// Characterise the code using the tailored decoder and the circuit error model
	double* probabilities = characterise_code(code, logicals, circuit_error, tailored_decoder);
	printf("Probability of Correction: %e\n", probabilities[0]);

	// Cleanup
	error_model_free(em_cnot);
	error_model_free(em_noise);
	error_model_free(circuit_error);

	error_probabilities_free(probabilities);
	error_probabilities_free(initial_error_probs);
	error_probabilities_free(final_error_probs);
	
	decoder_free(tailored_decoder);
	circuit_free(encode);
	gate_free(cnot);
	gate_free(noise);

	sym_free(code);
	sym_free(logicals);

	return 0;
}*/