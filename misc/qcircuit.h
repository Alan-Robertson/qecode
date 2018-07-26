#ifndef QCIRCUIT
#define QCIRCUIT

#define DEFAULT_STRING_SIZE 1
#define CNOT_CONTROL_VALUE 0
#define CNOT_TARGET_VALUE 1
#define CLIFFORD_DEFAULT_VALUE 0
#define MAX_CHARS_NUM 5

#include "../gates/clifford_generators.h"
#include "../circuits/circuit.h"


void qcircuit_print(circuit* c);
char* str_increase(char* original, char const* added);


void qcircuit_print(circuit* c)
{
	char const* header = "\\Qcircuit @C=1em @R=.7em {";
	char const* space = "&";
	char const* gate = "\\gate{";
	char const* tail = "}";
	char const* wire = "\\qw";
	char const* cnot_targ = "\\targ";
	char const* cnot_ctrl = "\\ctrl{";
	char const* end_wire = "\\\\\n";

	uint32_t symbol_table_length = 0;
	char const* unspecified_unitary = "U_{";
	gate_operation_f* symbol_table = (gate_operation_f*)malloc(sizeof(gate_operation_f));

	char** qubits = (char**)malloc(sizeof(char*) * c->n_qubits);

	for (uint32_t i = 0; i < c->n_qubits; i++)
	{
		qubits[i] = (char*)calloc(sizeof(char), DEFAULT_STRING_SIZE);
	}

	// Iterate over the gates in the circuit
	circuit_element* ce = c->start;
	while (NULL != ce)
	{
		// Break from previous round
		for (int i = 0; i < c->n_qubits; i++)
		{
			qubits[i] = str_increase(qubits[i], space);
		}

		// Check the gate being applied
		if (ce->gate_element->operation == gate_cnot)
		{
			char cnot_target_diff[MAX_CHARS_NUM];
			sprintf(cnot_target_diff, "%d\n", ce->target_qubits[CNOT_CONTROL_VALUE] - ce->target_qubits[CNOT_TARGET_VALUE]);
			qubits[ce->target_qubits[CNOT_CONTROL_VALUE]] = str_increase(qubits[ce->target_qubits[CNOT_CONTROL_VALUE]], cnot_ctrl);
			qubits[ce->target_qubits[CNOT_CONTROL_VALUE]] = str_increase(qubits[ce->target_qubits[CNOT_CONTROL_VALUE]], cnot_target_diff);
			qubits[ce->target_qubits[CNOT_CONTROL_VALUE]] = str_increase(qubits[ce->target_qubits[CNOT_CONTROL_VALUE]], tail);
			qubits[ce->target_qubits[CNOT_TARGET_VALUE]] = str_increase(qubits[ce->target_qubits[CNOT_TARGET_VALUE]], cnot_targ);

		} 
		else if (ce->gate_element->operation == gate_hadamard)
		{
			qubits[ce->target_qubits[CLIFFORD_DEFAULT_VALUE]] = str_increase(qubits[ce->target_qubits[CLIFFORD_DEFAULT_VALUE]], gate);
			qubits[ce->target_qubits[CLIFFORD_DEFAULT_VALUE]] = str_increase(qubits[ce->target_qubits[CLIFFORD_DEFAULT_VALUE]], "H");
			qubits[ce->target_qubits[CLIFFORD_DEFAULT_VALUE]] = str_increase(qubits[ce->target_qubits[CLIFFORD_DEFAULT_VALUE]], tail);
		}
		else if (ce->gate_element->operation == gate_cphase)
		{
			qubits[ce->target_qubits[CLIFFORD_DEFAULT_VALUE]] = str_increase(qubits[ce->target_qubits[CLIFFORD_DEFAULT_VALUE]], gate);
			qubits[ce->target_qubits[CLIFFORD_DEFAULT_VALUE]] = str_increase(qubits[ce->target_qubits[CLIFFORD_DEFAULT_VALUE]], "P");
			qubits[ce->target_qubits[CLIFFORD_DEFAULT_VALUE]] = str_increase(qubits[ce->target_qubits[CLIFFORD_DEFAULT_VALUE]], tail);
		}
		else
		{
			// Determine a symbol for this gate
			uint8_t existing_symbol = 0;
			uint32_t symbol_index = symbol_table_length;
			for (uint32_t i = 0; i < symbol_table_length && !existing_symbol; i++)
			{
				if (symbol_table[i] == ce->gate_element->operation)
				{
					existing_symbol = 1;
					symbol_index = i;
				}
			}
			if (!existing_symbol) // Yeah, this isn't efficient, but it's scalable and shouldn't come up too often
			{
				symbol_table[symbol_table_length] = ce->gate_element->operation;
				symbol_table_length++;
				symbol_table = (gate_operation_f*)realloc(symbol_table, symbol_table_length + 1);
			}
			
			// Add the relevant gate
			for (int i = 0; i < ce->gate_element->n_qubits; i++)
			{
				char symbol_number[MAX_CHARS_NUM];
				sprintf(symbol_number, "%d\n",  symbol_index + 1);
				qubits[ce->target_qubits[i]] = str_increase(qubits[ce->target_qubits[i]], gate);
				qubits[ce->target_qubits[i]] = str_increase(qubits[ce->target_qubits[i]], unspecified_unitary);
				qubits[ce->target_qubits[i]] = str_increase(qubits[ce->target_qubits[i]], symbol_number);
				qubits[ce->target_qubits[i]] = str_increase(qubits[ce->target_qubits[i]], tail);
				qubits[ce->target_qubits[i]] = str_increase(qubits[ce->target_qubits[i]], tail);
			}
		}

		// Apply blank operations to all qubits that haven't done anything
		for (uint32_t i = 0; i < c->n_qubits; i++)
		{
			// Check if an operation has already been applied to the qubit
			uint8_t no_operation = 1;
			for (uint32_t j = 0; j < ce->gate_element->n_qubits && no_operation; j++)
			{
				if (i == ce->target_qubits[j])
				{
					no_operation = 0;
				}
			}

			// If no operation, add the quantum wire symbol
			if (no_operation)
			{
				qubits[i] = str_increase(qubits[i], wire);
			}
		}
		
		ce = ce->next;
	}

	// End the wires
	for (uint32_t i = 0; i < c->n_qubits; i++)
	{
		qubits[i] = str_increase(qubits[i], end_wire);
	}

	// Print the wires and free them
	printf("%s", header);
	for (uint32_t i = 0; i < c->n_qubits; i++)
	{
		printf("%s", qubits[i]);
		free(qubits[i]);
	}
	printf("%s", tail);

	// Free the last allocated memory
	free(qubits);
	return;
}

// Increases the length of a malloced string
char* str_increase(char* original, const char const* added)
{
	original = realloc(original, strnlen(original) + strnlen(added) + 2);
	strcat(original, added);
	return original;
}



#endif