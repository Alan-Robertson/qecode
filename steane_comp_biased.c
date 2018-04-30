#include "codes.h"
#include "error_models.h"
#include "destabilisers.h"
#include "sym_iter.h"
#include "tailored.h"
#include "decoders.h"
#include "dmatrix.h"
#include "characterise.h"
#include "circuit_search.h"
#include "channel.h"
#include "gates.h"
#include "circuit.h"
#include "encoding.h"

int main()
{	
	double rate_min = 0.0001, rate_delta = 2;
	unsigned n_increments = 10;

	double logical_rate[10];
	double error_rate = 0.0001;

	unsigned n_qubits = 7, n_logicals = 1, distance = 3;
	unsigned n_codes_searched = 1000;

	//sym* code = code_steane();
	//sym* logicals = code_steane_logicals();

	for (unsigned i = 0; i < n_increments; i++)
	{
		double bias = rate_min * pow(rate_delta, i); 
	
		biased_iid_model_data bf;
		bf.n_qubits = n_qubits;
		bf.bias = bias;
		bf.p_error = error_rate;

		gate* cnot = gate_create(2,  
			gate_cnot,
			error_model_iid,
			&bf,
			&bf);

		gate* hadamard = gate_create(1,
			gate_hadamard,
			error_model_iid,
			&bf,
			&bf);

		gate* phase = gate_create(1,
			gate_phase,
			error_model_iid,
			&bf,
			&bf);

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

		circuit* encode = encoding_circuit(code, logicals, NULL, cnot, hadamard, phase);

		double* initial_error_rate = (double*)malloc(sizeof(double) * (1 << (code->length)));
		memset(initial_error_rate, 0, (1 << (code->length)) * sizeof(double));
		initial_error_rate[0] = 1; // Set the identity to 1
			
		double* error_rates = circuit_run(encode, initial_error_rate);
		
		lookup_error_model_data md;
		md.lookup_table = error_rates;
		sym** decoder_data = tailor_decoder(code, logicals, error_model_lookup, &md);

		double* probabilities = characterise_code(code, logicals, error_model_lookup, &md, decoder_tailored, (void*)&decoder_data);

		logical_rate[i] = probabilities[0];

		// Free allocated objects 
		free(initial_error_rate);
		free(probabilities);
		free(code);
		free(logicals);		
	}

	printf("--------------------------------------\n");
	printf("Best Random of %d \n", n_codes_searched);
	printf("--------------------------------------\n");

	for (unsigned i = 0; i < n_increments; i++)
	{
		printf("%e ", logical_rate[i]);
		printf("\n");
	}

	return 0;	
}