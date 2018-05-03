#include "codes.h"
#include "error_models.h"
#include "tailored.h"
#include "decoders.h"
#include "characterise.h"
#include "gates.h"
#include "circuit.h"
#include "error_probabilities.h"

int main()
{
	unsigned n_qubits = 7;

	sym* code = code_steane();
	sym* logicals = code_steane_logicals();
	
	iid_model_data bf;
	bf.n_qubits = n_qubits;
	bf.p_error = 0.001;

	gate* cnot = gate_create(2,  
			gate_cnot,
			error_model_iid,
			&bf,
			&bf);


	circuit* encode = circuit_create(n_qubits);

	circuit_add_gate(encode, cnot, 0, 1);
	circuit_add_gate(encode, cnot, 0, 2);

	double* initial_error_probs = error_probabilities_identity(n_qubits);
	double* final_error_probs = circuit_run(encode, initial_error_probs);

	lookup_error_model_data md;
	md.lookup_table = final_error_probs;

	// Recovery!
	sym** decoder_data = tailor_decoder(code, logicals, error_model_lookup, &md);

	double* probabilities = characterise_code(code, logicals, error_model_lookup, &md, decoder_tailored, (void*)&decoder_data);

	
	
	error_probabilities_free(probabilities);
	decoder_free(decoder_data, code->height);
	error_probabilities_free(initial_error_probs);
	error_probabilities_free(final_error_probs);
	circuit_free(encode);
	gate_free(cnot);
	sym_free(code);
	sym_free(logicals);

	return 0;
}