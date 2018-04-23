#include "codes.h"
#include "error_models.h"
#include "tailored.h"
#include "decoders.h"
#include "characterise.h"
#include "gates.h"
#include "circuit.h"

int main()
{
	unsigned n_qubits = 7;

	sym* code = code_steane();
	sym* logicals = code_steane_logicals();

	for (int i = 1; i < 100; i++)
	{
		iid_model_data bf;
		bf.n_qubits = n_qubits;
		bf.p_error = i * 0.0001;

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

		double* final_error_rate;

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

		final_error_rate = circuit_run(encode, initial_error_rate);

		lookup_error_model_data md;
		md.lookup_table = final_error_rate;

		// Recovery!
		sym** decoder_data = tailor_decoder(code, logicals, error_model_lookup, &md);

		double* probabilities = characterise_code(code, logicals, error_model_lookup, &md, decoder_tailored, (void*)&decoder_data);	
		//printf("[%e, %e],\n", bf.p_error, probabilities[0]);
		//MatrixXcd lc = channel_logical(code, logicals, error_model_lookup, &md, decoder_tailored, (void*)&decoder_data);
		//std::cout << lc << std::endl;
		free(probabilities);
		circuit_free(encode);
		free(cnot);
		free(hadamard);
	}

	sym_free(code);
	sym_free(logicals);

	return 0;
}