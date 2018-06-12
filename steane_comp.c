#include "codes/codes.h"
#include "characterise.h"

#include "error_models/iid.h"
#include "error_models/lookup.h"

#include "decoders/tailored.h"

#include "circuits/gates.h"
#include "circuits/circuit.h"
#include "circuits/circuit_search.h"
#include "circuits/encoding.h"


int main()
{	
	double rate_min = 0.0001, rate_delta = 2;

	unsigned n_qubits = 7, n_logicals = 1, distance = 3;
	unsigned n_codes_searched = 100;

	//sym* code = code_steane();
	//sym* logicals = code_steane_logicals();

	double p_gate_error = 0; // Gates themselves are noiseless
	//double  = 0.001;

	for (double p_environmental_error = 0.00001; p_environmental_error < 0.001; p_environmental_error += 0.00001)
	{

		// Build our circuit with noise included:
		error_model* em_cnot = error_model_create_iid(2, p_gate_error);
		error_model* em_gate = error_model_create_iid(1, p_gate_error);

		gate* cnot = gate_create(2,  
				gate_cnot,
				em_cnot,
				NULL);

		gate* hadamard = gate_create(1,  
				gate_hadamard,
				em_gate,
				NULL);

		gate* phase = gate_create(1,  
				gate_phase,
				em_gate,
				NULL);

		// Create our iid noise
		error_model* em_noise = error_model_create_iid(1, p_environmental_error);
		gate* noise = gate_create_iid_noise(em_noise);

			random_code_return r = circuit_search_stabiliser(
			n_qubits, 
			n_logicals, 
			distance, 
			n_codes_searched,
			encoding_circuit,
			cnot,
			hadamard,
			phase,
			noise);

		sym* code = r.code; 
		sym* logicals = r.logicals;


		// Create our circuit
		circuit* encode = encoding_circuit(code, logicals, cnot, hadamard, phase);

		// Run the circuit and find the overall error rates
		double* initial_error_probs = error_probabilities_identity(n_qubits);
		double* final_error_probs = circuit_run(encode, initial_error_probs, noise);
		
		// Build an error model using the output probabilities of the circuit
		error_model* circuit_error = error_model_create_lookup(n_qubits, final_error_probs);
		
		// Build the tailored decoder
		decoder* tailored_decoder = decoder_create_tailored(code, logicals, circuit_error);

		// Characterise the code using the tailored decoder and the circuit error model
		double* probabilities = characterise_code(code, logicals, circuit_error, tailored_decoder);
		printf("%e %e\n", p_environmental_error, probabilities[0]);

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

	}

	return 0;

}

/*
	for (unsigned i = 0; i < n_increments; i++)
	{
		double error_rate = rate_min * pow(rate_delta, i); 
	
		error_model* em_iid = error_model_create_iid(n_qubits, p_error);

		gate* cnot = gate_create(2,  
			gate_cnot,
			em_iid,
			NULL);


		random_code_return r = circuit_search_stabiliser(
			n_qubits, 
			n_logicals, 
			distance, 
			n_codes_searched,
			encoding_circuit,
			cnot,
			hadamard,
			phase);

		sym* code = r.code; 
		sym* logicals = r.logicals;

		circuit* encode = encoding_circuit(code, logicals, cnot, hadamard, phase);

		double* initial_error_probs = error_probabilities_identity(n_qubits);
		double* final_error_probs = circuit_run(encode, initial_error_probs);	
		
		// Build an error model using the output probabilities of the circuit
		error_model* circuit_error = error_model_create_lookup(n_qubits, final_error_probs);

		// Build the tailored decoder
		decoder* tailored_decoder = decoder_create_tailored(code, logicals, circuit_error);

		// Characterise the code using the tailored decoder and the circuit error model
		double* probabilities = characterise_code(code, logicals, circuit_error, tailored_decoder);
		logical_rate[i] = probabilities[0];

		// Free allocated objects 
		free(initial_error_rate);
		free(probabilities);		
	}

	printf("--------------------------------------\n");
	printf("Best Random of %d \n", n_codes_searched);
	printf("--------------------------------------\n");

	for (unsigned i = 0; i < n_increments; i++)
	{
		printf("%e ", logical_rate[i]);
		printf("\n");
	}

	return 0;	*/