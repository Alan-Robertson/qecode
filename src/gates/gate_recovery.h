#ifndef GATE_RECOVERY
#define GATE_RECOVERY

#include "../sym.h"
#include "../decoders/decoders.h"
#include "gate_result.h"

typedef struct {
	gate* pauli_X;
	gate* pauli_Z;
	sym* recovery_operation;
} gate_data_recovery_t;


gate_result* gate_recovery(const sym* initial_state, const void* gate_data, const unsigned* target_qubits);

gate* gate_create_recovery(const uint32_t n_qubits, gate* pauli_X, gate* pauli_Z)
{
	// Create the recovery gate
	gate* recovery = gate_create(n_qubits, gate_recovery, NULL, NULL);

	// Setup the parameters to be passed
	gate_data_recovery_t* dr = (gate_data_recovery_t*)malloc(sizeof(gate_data_recovery_t));
	dr->pauli_X = pauli_X;
	dr->pauli_Z = pauli_Z;

	// And set the operation data 
	recovery->operation_data = (void*)dr;
	return recovery;
};


// Here the initial state is the output of the measurement data and should only contain the ancilla bits
gate_result* gate_recovery(const sym* initial_state, const void* gate_data, const unsigned* target_qubits)
{
	// Unpack the gate data	
	gate_data_recovery_t* dr = (gate_data_recovery_t*)(((gate*)gate_data)->operation_data);

	// Copy our initial state
	sym* recovered_state = sym_copy(initial_state);

	// Let's make the correction noiseless for now, mostly to avoid having to expand the error probs during the correction
	if (dr->pauli_X->gate_error_model == NULL && dr->pauli_Z->gate_error_model == NULL)
	{
		// Apply the gates that our recovery string dictates that we should
		for (uint32_t i = 0; i < dr->recovery_operation->n_qubits; i++)
		{

			// Check for an anti-commutation relation in the X block
			if (sym_get_X(dr->recovery_operation, 0, i))
			{
				// Apply a Z gate to target qubit i
				gate_result* gr = dr->pauli_Z->operation(recovered_state, dr->pauli_Z, target_qubits + i);
				sym_free(recovered_state);
				recovered_state = sym_copy(gr->state_results[0]);
				gate_result_free(gr);
			}

			// Check for an anti-commutation relation in the Z block
			if (sym_get_Z(dr->recovery_operation, 0, i))
			{
				gate_result* gr = dr->pauli_X->operation(recovered_state, dr->pauli_X, target_qubits + i);
				sym_free(recovered_state);
				recovered_state = sym_copy(gr->state_results[0]);
				gate_result_free(gr);
			}
		}

    	return gate_result_create_single(1, recovered_state);
	}
	else
	{
		// TODO
		printf("Currently does not support errors during recovery, WIP\n");
		return NULL;
	}
}



#endif