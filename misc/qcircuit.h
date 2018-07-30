#ifndef QCIRCUIT
#define QCIRCUIT

#define DEFAULT_STRING_SIZE 1
#define CNOT_CONTROL_VALUE 0
#define CNOT_TARGET_VALUE 1
#define CLIFFORD_DEFAULT_VALUE 0
#define MAX_CHARS_NUM 5

#include "../gates/clifford_generators.h"
#include "../circuits/circuit.h"

/*
 * qcircuit_print
 * Prints the associated qcircuit latex code for a given circuit object
 * :: const circuit* c :: The circuit to print
 * Returns nothing, just prints the latex code to stdout
 */
void qcircuit_print(circuit* c);

/*
 * str_increase
 * Concatenates a string to a malloc'ed string
 * :: char* original :: The malloced string to be concatenated to
 * :: char const* added :: The const string that will be added
 * Reallocs the malloced string and returns the pointer to the new memory
 */
char* str_increase(char* original, char const* added);


/*
 * qcircuit_print
 * Prints the associated qcircuit latex code for a given circuit object
 * :: const circuit* c :: The circuit to print
 * Returns nothing, just prints the latex code to stdout
 */
void qcircuit_print(circuit* c)
{
	// String constants for qcircuit
	char const* header = "\\Qcircuit @C=1em @R=.7em {\n";
	char const* space = " & ";
	char const* gate = "\\gate{";
	char const* tail = "}";
	char const* wire = "\\qw";
	char const* cnot_targ = "\\targ";
	char const* cnot_ctrl = "\\ctrl{";
	char const* end_wire = " \\";
	char const* newline = "\n"

	// For building arbitrary unknown gates
	uint32_t symbol_table_length = 0;
	char const* unspecified_unitary = "U_{";
	gate_operation_f* symbol_table = (gate_operation_f*)malloc(sizeof(gate_operation_f));

	// Our set of qubit strings
	char** qubits = (char**)malloc(sizeof(char*) * c->n_qubits);

	// Initialise our qubits to the null character
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
		{ // Cnot gate found
			char cnot_target_diff[MAX_CHARS_NUM];
			sprintf(cnot_target_diff, "%d", ce->target_qubits[CNOT_TARGET_VALUE] - ce->target_qubits[CNOT_CONTROL_VALUE]);
			qubits[ce->target_qubits[CNOT_CONTROL_VALUE]] = str_increase(qubits[ce->target_qubits[CNOT_CONTROL_VALUE]], cnot_ctrl);
			qubits[ce->target_qubits[CNOT_CONTROL_VALUE]] = str_increase(qubits[ce->target_qubits[CNOT_CONTROL_VALUE]], cnot_target_diff);
			qubits[ce->target_qubits[CNOT_CONTROL_VALUE]] = str_increase(qubits[ce->target_qubits[CNOT_CONTROL_VALUE]], tail);
			qubits[ce->target_qubits[CNOT_TARGET_VALUE]] = str_increase(qubits[ce->target_qubits[CNOT_TARGET_VALUE]], cnot_targ);
		} 
		else if (ce->gate_element->operation == gate_hadamard)
		{ // Hadamard gate found
			qubits[ce->target_qubits[CLIFFORD_DEFAULT_VALUE]] = str_increase(qubits[ce->target_qubits[CLIFFORD_DEFAULT_VALUE]], gate);
			qubits[ce->target_qubits[CLIFFORD_DEFAULT_VALUE]] = str_increase(qubits[ce->target_qubits[CLIFFORD_DEFAULT_VALUE]], "H");
			qubits[ce->target_qubits[CLIFFORD_DEFAULT_VALUE]] = str_increase(qubits[ce->target_qubits[CLIFFORD_DEFAULT_VALUE]], tail);
		}
		else if (ce->gate_element->operation == gate_phase)
		{ // Phase gate found
			qubits[ce->target_qubits[CLIFFORD_DEFAULT_VALUE]] = str_increase(qubits[ce->target_qubits[CLIFFORD_DEFAULT_VALUE]], gate);
			qubits[ce->target_qubits[CLIFFORD_DEFAULT_VALUE]] = str_increase(qubits[ce->target_qubits[CLIFFORD_DEFAULT_VALUE]], "P");
			qubits[ce->target_qubits[CLIFFORD_DEFAULT_VALUE]] = str_increase(qubits[ce->target_qubits[CLIFFORD_DEFAULT_VALUE]], tail);
		}
		else
		{ // Not a clifford gate; we need to consult our symbol table
			// Determine if this gate has been seen before in this circuit
			uint8_t existing_symbol = 0;
			uint32_t symbol_index = symbol_table_length;
			for (uint32_t i = 0; i < symbol_table_length && !existing_symbol; i++)
			{
				// Check if we've seen this function pointer before, if we haven't we're going to add it to the symbol table
				// If we have then we already know what this gate has been called before
				// This breaks if you valgrind or do anything else to change how memory is allocated and called
				if (symbol_table[i] == ce->gate_element->operation) 
				{
					existing_symbol = 1;
					symbol_index = i;
				}
			}
			if (!existing_symbol) // Yeah, this isn't efficient at all, but it's scalable and shouldn't come up too often
			{ // Gate hasn't been seen, increase the size of the symbol table and add it
				symbol_table[symbol_table_length] = ce->gate_element->operation;
				symbol_table_length++;
				symbol_table = (gate_operation_f*)realloc(symbol_table, symbol_table_length + 1);
			}
			
			// Add the relevant gate
			for (int i = 0; i < ce->gate_element->n_qubits; i++)
			{ // Add a 'unitary' with the associated symbol.
				char symbol_number[MAX_CHARS_NUM];
				sprintf(symbol_number, "%d",  symbol_index + 1); // So that we can count the gates from 1 rather than 0
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
			{ // If one of the target qubits is listed here then an operation has been performed
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

		// Get the next gate for the circuit
		ce = ce->next;
	}

	// End the wires, all but the last one
	for (uint32_t i = 0; i < c->n_qubits; i++)
	{
		if (i != c->n_qubits - 1)
		{
			qubits[i] = str_increase(qubits[i], end_wire);
		}
		// And a newline after each for readability
		qubits[i] = str_increase(qubits[i], newline);
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
	free(symbol_table);
	return;
}

/*
 * str_increase
 * Concatenates a string to a malloc'ed string
 * :: char* original :: The malloced string to be concatenated to
 * :: char const* added :: The const string that will be added
 * Reallocs the malloced string and returns the pointer to the new memory
 */
char* str_increase(char* original, char const* added)
{
	original = (char*)realloc(original, strlen(original) + strlen(added) + 2);
	strcat(original, added);
	return original;
}

#endif