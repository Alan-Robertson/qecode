

typedef struct {
	uint32_t n_syndrome_bits;
	sym** syndrome_states;
	sym* syndrome;
} circuit_recovery_data;


circuit* circuit_syndrome_measurement_create(const unsigned int n_qubits, const sym* code, const gate* cnot, const gate* hadamard, const gate* phase)
{
	circuit* recovery = circuit_create(n_qubits);
	recovery->circuit_operation = circuit_syndrome_measurement_run;
	syndrome_measurement_circuit(circuit* recovery, code, cnot, hadamard, phase);

	circuit_recovery_data* rd = (circuit_recovery_data*)malloc(sizeof(circuit_recovery_data));
	rd->n_syndrome_bits = code->height;
	recovery->circuit_data = rd;

	return recovery;
}

double* circuit_syndrome_measurement_run(circuit* recovery, double* initial_error_rates, gate* noise)
{
	uint32_t n_errors = (1 << (recovery->n_qubits * 2));
	circuit_recovery_data* rd = (circuit_recovery_data*)recovery->circuit_data;

	rd->syndrome_states = (circuit_recovery_data**)malloc(sizeof(circuit_recovery_data*) * n_errors);

	sym* base_state = sym_create(1, rd->n_syndrome_bits);

	for (uint32_t i = 0; i < n_errors; i++)
	{

	}





	for (uint32_t i = 0; i < n_errors; i++)
	{
		sym_free(rd->syndrome_states[i]);	
	}

}

void syndrome_measurement_circuit(circuit* recovery, const sym* code, const gate* cnot, const gate* hadamard, const gate* phase)
{
	size_t start_ancilla = code->length / 2;

	// Pauli Z checks
	for (size_t i = 0; i < code->length / 2; i++)
	{
		for (size_t j = 0; j < code->height; j++)
		{
			if (sym_is_Z(code, j, i))
			{
				circuit_add_gate(recovery, cnot, i, j + start_ancilla);
			}
		}
	}

	// Pauli X checks
	for (size_t i = 0; i < code->length / 2; i++)
	{
		bool found = false;
		for (size_t j = 0; j < code->height; j++)
		{
			if (sym_is_X(code, j, i))
			{
				if (false == found)
				{
					circuit_add_gate(recovery, hadamard, i);
				}

				circuit_add_gate(recovery, cnot, i, j + start_ancilla);
			}
		}

		// Qubit has been mapped to the X basis, map it back
		if (found)
		{
			circuit_add_gate(recovery, hadamard, i);
		}
	}

	// Pauli Y checks
	for (size_t i = 0; i < code->length / 2; i++)
	{
		bool found = false;
		for (size_t j = 0; j < code->height; j++)
		{
			if (sym_is_Y(code, j, i))
			{
				if (false == found)
				{
					circuit_add_gate(recovery, phase, i);
					circuit_add_gate(recovery, hadamard, i);
				}


				circuit_add_gate(recovery, cnot, i, j + start_ancilla);
			}
		}

		// Qubit has been mapped to the Y basis, map it back
		if (found)
		{
			circuit_add_gate(recovery, hadamard, i);
			circuit_add_gate(recovery, phase, i);
			circuit_add_gate(recovery, phase, i);
			circuit_add_gate(recovery, phase, i);
		}
	}

	return;
}