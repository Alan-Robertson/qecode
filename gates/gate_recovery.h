#ifndef GATE_RECOVERY
#define GATE_RECOVERY

#include "../sym.h"
#include "../decoders/decoders.h"
#include "gate_result.h"

typedef struct {
	gate* pauli_X;
	gate* pauli_Y;
	gate* pauli_Z;
	sym* recovery_operation;
} gate_data_recovery_t;

// Here the initial state is the output of the measurement data and should only contain the ancilla bits
sym* gate_recovery(const sym* initial_state, void* gate_data, const unsigned* target_qubits)
{
	// Unpack the gate data	
	gate_data_recovery_t* dr = (gate_data_recovery_t*)gate_data;

	// Copy our initial state
	sym* recovered_state = sym_copy(initial_state);
	sym* tmp_state = NULL;

	// Apply the gates that our recovery string dictates that we should
	for (uint32_t i = 0; i < dr->recovery_operation->n_qubits)
	{
		// Check for an anti-commutation relation in the X block
		if (sym_is_X(dr->recovery_operation, 0, i))
		{
			// Apply a Z gate to target qubit i
			tmp_state = gate_operation(dr->pauli_Z, recovered_state, target_qubits + i);
			sym_free(recovered_state);
			recovered_state = tmp_state;
		}

		// Check for an anti-commutation relation in the Z block
		if (sym_is_Z(dr->recovery_operation, 0, i))
		{
			tmp_state = gate_operation(dr->pauli_X, recovered_state, target_qubits + i);
			sym_free(recovered_state);
			recovered_state = tmp_state;

		}
	}
    
    return recovered_state;
}

#endif