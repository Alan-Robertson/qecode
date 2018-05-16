#include "codes.h"
//#include "tailored.h"
#include "decoders/tailored.h"
//#include "characterise.h"
//#include "gates.h"
//#include "circuit.h"
#include "error_models/iid.h"
#include "error_models/lookup.h"
#include "error_probabilities.h"

int main()
{
	unsigned n_qubits = 7;
	unsigned n_logical_qubits = 1;
	double p_error = 0.01;

	sym* code = code_steane();
	sym* logicals = code_steane_logicals();
	
	error_model* em_iid = error_model_create_iid(n_qubits, p_error);

	/*gate* cnot = gate_create(2,  
			gate_cnot,
			em_iid,
			NULL);
	
	circuit* encode = circuit_create(n_qubits);

	circuit_add_gate(encode, cnot, 0, 1);
	circuit_add_gate(encode, cnot, 0, 2);
	
	double* initial_error_probs = error_probabilities_identity(n_qubits);
	double* final_error_probs = circuit_run(encode, initial_error_probs);*/
	
	//error_model* em_lookup = error_model_create_lookup(n_qubits, final_error_probs);
	//decoder* destabilisers = decoder_create_destabiliser(code, logicals);

	// Build the tailored decoder
	decoder* tailored_decoder = decoder_create_tailored(code, logicals, em_iid);

	//double* probabilities = characterise_code(code, logicals, em_lookup, decoder_tailored, (void*)&decoder_data);
	
	//decoder_free(destabilisers);

	error_model_free(em_iid);
	//error_model_free(em_lookup);

	//error_probabilities_free(probabilities);
	//error_probabilities_free(initial_error_probs);
	//error_probabilities_free(final_error_probs);

	decoder_free(tailored_decoder);

	//circuit_free(encode);
	//gate_free(cnot);

	sym_free(code);
	sym_free(logicals);

	return 0;
}