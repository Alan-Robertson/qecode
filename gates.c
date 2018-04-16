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
	unsigned n_qubits = 3;

	bit_flip_model_data bf;
	bf.n_qubits = 2;
	bf.p_error = 0.1;

	unsigned target_qubits_a[2] = {0, 1};

	double initial_error_rate[1 << (2 * n_qubits)];
	memset(initial_error_rate, 0, (1 << (2 * n_qubits)) * sizeof(double));
	initial_error_rate[0] = 1; // Set the identity to 1
		
	gate* cnot = gate_create(2,  
		gate_cnot,
		error_model_bit_flip,
		&bf,
		NULL);

	sym* s = sym_create(1, n_qubits);

	printf("%p\n", cnot->error_model);

	printf("%d\n", cnot->error_model(s, &bf));
	return 0;
	double* final_error_rate = gate_noise(2, initial_error_rate, cnot, target_qubits_a);
	return 0;
}
/*
	circuit* encode = circuit_create(n_qubits);
	circuit_add_gate(encode, cnot, target_qubits_a);

	double initial_error_rate[1 << (2 * n_qubits)];
	memset(initial_error_rate, 0, (1 << (2 * n_qubits)) * sizeof(double));
	initial_error_rate[0] = 1; // Set the identity to 1
	
	double* encoded_error_rate = circuit_run(initial_error_rate, encode, n_qubits);
	return 0;
	characterise_print(encoded_error_rate, n_qubits);

	circuit_free(encode);
	free(cnot);
}*/