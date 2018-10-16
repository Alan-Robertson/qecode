#include "../codes/codes.h"
#include "../gates/pauli_generators.h"
#include "../gates/gate_recovery.h"
#include "../decoders/destabiliser.h"
#include "../sym_iter.h"
#include "../characterise.h"

/*
 *	Check that some trivial error is recovered
 */

int main()
{	
	unsigned n_qubits = 5;

	double p_error = 0;

	sym* code = code_five_qubit();
	sym* logicals = code_five_qubit_logicals();

	// Build our decoder (with an incomplete error model):
	decoder* destab_decoder = decoder_create_destabiliser(code, logicals);

	// Setup our gates
	gate* pauli_X = gate_create(1, gate_pauli_X, NULL, NULL);
	gate* pauli_Z = gate_create(1, gate_pauli_Z, NULL, NULL);
	gate* recovery = gate_create_recovery(n_qubits, pauli_X, pauli_Z);

	// Setup our initial error probabilities
	double* initial_error_probs = error_probabilities_zeros(n_qubits);
	double* final_error_probs = error_probabilities_zeros(n_qubits);

	// Generate some single qubit errors
	double prob = (1.0) / (code->length);
	for (int i = 0; i < code->length; i++)
	{
		initial_error_probs[1 << i] = prob;
	}

	// Setup the target qubits
	uint32_t* target_qubits = (uint32_t*)malloc(sizeof(uint32_t) * n_qubits);
	for (uint32_t i = 0; i < n_qubits; i++)
	{
		target_qubits[i] = i;
	}

	// Find and recover each of these errors
	sym_iter* siter = sym_iter_create_n_qubits(n_qubits);
	while (sym_iter_next(siter))
	{
		if (initial_error_probs[sym_iter_ll_from_state(siter)] > 0)
		{
			printf("#######\n");
			sym* error_state = sym_copy(siter->state);
			sym* syndrome = sym_syndrome(code, error_state);
			sym* recovery_operation = decoder_call(destab_decoder, syndrome);

			// Cracking this open to setup the recovery operation
			((gate_data_recovery_t*)(recovery->operation_data))->recovery_operation = sym_copy(recovery_operation);

			// Calling the method here directly, because normally we need a specialised circuit 
			// to determine the other values to pass to this
			gate_result* recovered_state = gate_recovery(error_state, recovery, target_qubits);
			printf("Error Rate %e\n", initial_error_probs[sym_iter_ll_from_state(siter)]);
			printf("Initial Error: \t\t");
			sym_print(error_state);

			printf("Recovery Operation: \t");
			sym_print(recovery_operation);

			printf("Recovered State: \t");
			sym_print(recovered_state->state_results[0]);

			// Setup the recovered state
			final_error_probs[sym_to_ll(recovered_state->state_results[0])] += initial_error_probs[sym_iter_ll_from_state(siter)];

			sym_free(syndrome);
			sym_free(recovery_operation);
			sym_free(error_state);
			gate_result_free(recovered_state);
		}
	}
	sym_iter_free(siter);
	double* logical_rates = characterise_code_corrected(code, logicals, final_error_probs);
	siter = sym_iter_create_n_qubits(logicals->n_qubits);
	
	while(sym_iter_next(siter))
	{
		printf("%e \t", logical_rates[sym_iter_ll_from_state(siter)]);
		sym_print(siter->state);
	}
	

	return 0;
}