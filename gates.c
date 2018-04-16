#include <stdarg.h>
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

int main()
{
	unsigned n_qubits = 7;

	iid_model_data bf;
	bf.n_qubits = n_qubits;
	bf.p_error = 0.0001;

	double initial_error_rate[1 << (2 * n_qubits)];
	memset(initial_error_rate, 0, (1 << (2 * n_qubits)) * sizeof(double));
	initial_error_rate[0] = 1; // Set the identity to 1
		
	circuit* encode = circuit_create(n_qubits);

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

	circuit_add_gate(encode, hadamard, 0);
	circuit_add_gate(encode, hadamard, 1);
	circuit_add_gate(encode, hadamard, 2);

	circuit_add_gate(encode, cnot, 3, 4);
	circuit_add_gate(encode, cnot, 3, 5);


	circuit_add_gate(encode, cnot, 2, 3);
	circuit_add_gate(encode, cnot, 2, 4);
	circuit_add_gate(encode, cnot, 2, 5);


	circuit_add_gate(encode, cnot, 1, 3);
	circuit_add_gate(encode, cnot, 1, 5);
	circuit_add_gate(encode, cnot, 1, 6);


	circuit_add_gate(encode, cnot, 0, 6);
	circuit_add_gate(encode, cnot, 0, 5);
	circuit_add_gate(encode, cnot, 0, 4);	


	double* final_error_rate = circuit_run(encode, initial_error_rate);

	characterise_print(final_error_rate, n_qubits * 2);

	free(cnot);
	return 0;
}