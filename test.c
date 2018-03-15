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
#include "codewords.h"

int main()
{
	
	sym* code = code_steane();
	sym* logicals = code_steane_logicals();

	// Our initial codeword |000....00>
	sym* codeword = sym_create(1, code->length);
	for (int i = 0; i < codeword->length/2; i++)
	{
		sym_set(codeword, 0, i, 1);
	}

	sym* codewords = codewords_find(codeword, code, logicals);

	sym_print(codewords);

	sym_free(codewords);
	sym_free(codeword);
	sym_free(logicals);
	sym_free(code);
}



/*

const unsigned n_qubits = 5;

	sym* code = code_five_qubit();
	sym* logicals = code_five_qubit_logicals();

	error_model_f error_model = error_model_bit_flip;
	decoder_f decoder = decoder_tailored;

	bit_flip_model_data cnot_model_data;
	cnot_model_data.n_qubits = 2;
	cnot_model_data.p_error = 0.001;

	gate* cnot = gate_create(cnot_model_data.n_qubits, error_model, &cnot_model_data);
	unsigned cnot_qubits[2] = {0,1};

	circuit* encode = circuit_create();
	circuit_add_gate(encode, cnot, cnot_qubits);
	circuit_add_gate(encode, cnot, cnot_qubits);
	circuit_add_gate(encode, cnot, cnot_qubits);
	circuit_add_gate(encode, cnot, cnot_qubits);

	double initial_error_rate[1 << (2 * n_qubits)];
	memset(initial_error_rate, 0, (1 << (2 * n_qubits)) * sizeof(double));
	initial_error_rate[0] = 1;
	
	double* encoded_error_rate = circuit_run(initial_error_rate, encode, n_qubits);

	characterise_print(encoded_error_rate, code->length);

	circuit_delete(encode);
	free(cnot);
	sym_free(code);
	sym_free(logicals);
	free(encoded_error_rate); */