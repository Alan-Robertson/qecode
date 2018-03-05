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

int main()
{
	const unsigned n_qubits = 5;

	sym* code = code_five_qubit();
	sym* logicals = code_five_qubit_logicals();

	error_model_f error_model = error_model_iid;
	decoder_f decoder = decoder_tailored;

	bit_flip_model_data cnot_model_data;
	cnot_model_data.n_qubits = 2;
	cnot_model_data.p_error = 0.001;

	gate* cnot = gate_build(cnot_model_data.n_qubits, error_model, &cnot_model_data);
	unsigned cnot_qubits[2] = {3,4};

	double initial_error_rate[1 << (2*n_qubits)];
	initial_error_rate[0] = 1;
	for (unsigned i = 1; i < (1<< (2*n_qubits)); i++)
	{
		initial_error_rate[i] = 0;
	}
	
	double* gate_error_rate = gate_apply_noisy(n_qubits, initial_error_rate, cnot, cnot_qubits);
	double* gate_error_rate_two = gate_apply_noisy(n_qubits, gate_error_rate, cnot, cnot_qubits);
	characterise_print(gate_error_rate_two, code->length);



	free(cnot);
	sym_free(code);
	sym_free(logicals);
	free(gate_error_rate);
}