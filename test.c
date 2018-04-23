#include "codes.h"
#include "error_models.h"
#include "destabilisers.h"
#include "sym_iter.h"
#include "tailored.h"
#include "decoders.h"
#include "dmatrix.h"
#include "characterise.h"
#include "random_code_search.h"
#include "channel.h"
#include "gates.h"
#include "circuit.h"
#include "encoding.h"
#include "gate_operations.h"

int main()
{
	sym* code = code_steane();
	sym* logicals = code_steane_logicals();
	
	bit_flip_model_data bf;
	bf.n_qubits = code->length / 2;
	bf.p_error = 0.001;

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

	circuit* encode = encoding_circuit(code, logicals, NULL, cnot, hadamard, phase);

	double* initial_error_rate = (double*)malloc(sizeof(double) * (1 << (code->length)));
	memset(initial_error_rate, 0, (1 << (code->length)) * sizeof(double));
	initial_error_rate[0] = 1; // Set the identity to 1
		
	double* error_rates = circuit_run(encode, initial_error_rate);
	
	lookup_error_model_data md;
	md.lookup_table = error_rates;
	sym** decoder_data = tailor_decoder(code, logicals, error_model_lookup, &md);

	double* probabilities = characterise_code(code, logicals, error_model_lookup, &md, decoder_tailored, (void*)&decoder_data);	
		

	free(probabilities);
	free(initial_error_rate);
	//free(error_rates);
	circuit_free(encode);
	sym_free(logicals);
	sym_free(code);
}
