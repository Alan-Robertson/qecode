#ifndef CIRCUIT_RECOVERY
#define CIRCUIT_RECOVERY

#include "../sym.h"
#include "circuit.h"


circuit* recovery_circuit(const sym* code, const decoder* decoders, const gate* cnot, const gate* hadamard, const gate* phase)
{
	// Qubits 0 -> length / 2 are the regular qubits, the others are ancillas
	circuit* recovery = circuit_create(code->length / 2 + code->height);

	syndrome_measurement_circuit(recovery, code, cnot, hadamard);

	return recovery;
}

void syndrome_measurement_circuit(circuit* recovery, const sym* code, const gate* cnot, const gate* hadamard, const gate* phase)
{
	size_t start_ancilla = code->length / 2;

	int I = 0;
	int X = 1;
	int Z = 2;
	int Y = 3;

	// Pauli X checks
	for (size_t i = 0; i < code->length / 2; i++)
	{
		for (size_t j = 0; j < code->height; j++)
		{
			if (sym_is_X(code, j, i))
			{
				circuit_add_gate(recovery, cnot, i, j + start_ancilla);
			}
		}
	}

	// Pauli Z checks
	for (size_t i = 0; i < code->length / 2; i++)
	{
		bool found = false;
		for (size_t j = 0; j < code->height; j++)
		{
			if (sym_is_Z(code, j, i))
			{
				if (false == found)
				{
					circuit_add_gate(recovery, hadamard, i);
				}


				circuit_add_gate(recovery, cnot, i, j + start_ancilla);
			}
		}

		// Qubit has been mapped to the Z basis, map it back
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
			if (sym_is_Z(code, j, i))
			{
				if (false == found)
				{
					circuit_add_gate(recovery, phase, i);
					circuit_add_gate(recovery, hadamard, i);
				}


				circuit_add_gate(recovery, cnot, i, j + start_ancilla);
			}
		}

		// Qubit has been mapped to the Z basis, map it back
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



void recovery_operation_circuit();


#endif