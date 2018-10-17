#include "../codes/codes.h"
#include "../codes/candidate_codes.h"

#include "../decoders/destabiliser.h"
#include "../decoders/tailored.h"

#include "../gates/pauli_generators.h"
#include "../gates/measurement.h"

#include "../error_models/lookup.h"

#include "../circuits/recovery.h"

#include "../sym_iter.h"
#include "../characterise.h"

/*
 *	Check the recovery circuit
 *
 */

int main()
{	
	sym* code = code_candidate_seven_a();
	sym* logicals = code_candidate_seven_a_logicals();

	unsigned n_qubits = code->n_qubits;
	unsigned n_ancilla_qubits = code->height;
	
	// Build our decoder (with an incomplete error model):
	decoder* destab_decoder = decoder_create_destabiliser(code, logicals);

	// Setup our gates
	gate* pauli_X = gate_create(1, gate_pauli_X, NULL, NULL);
	gate* pauli_Z = gate_create(1, gate_pauli_Z, NULL, NULL);
	gate* measure_ancillas = gate_create(n_ancilla_qubits, gate_measure_Z, NULL, NULL);

	// Setup our initial error probabilities
	double* initial_error_probs = error_probabilities_zeros(n_qubits);

	// Generate some single qubit X and Z errors
	double prob = (1.0) / (code->n_qubits * 3);
	for (int i = 0; i < code->length; i++)
	{
		initial_error_probs[1 << i] = prob;
	}

	// Generate some single qubit Y errors
	for (int i = 0; i < code->n_qubits; i++)
	{
		initial_error_probs[((1 << code->n_qubits) + 1) << i] = prob;
	}

	// Treat the above as a lookup based error model
	error_model* em =  error_model_create_lookup(n_qubits, initial_error_probs);

	// Build a real decoder
	decoder* tailored = decoder_create_tailored(code, logicals, em);

	// Get our recovery circuit for the destabliser decoder
	circuit* recovery_destab = circuit_recovery_create(
		n_qubits,
		n_ancilla_qubits,
		destab_decoder,
		pauli_X,
		pauli_Z,
		measure_ancillas);


	// Get our recovery circuit for the tailored
	circuit* recovery_tailored = circuit_recovery_create(
		n_qubits,
		n_ancilla_qubits,
		tailored,
		pauli_X,
		pauli_Z,
		measure_ancillas);

	// Fake the measurement circuit
	double* syndrome_error_probs = error_probabilities_zeros(n_qubits + n_ancilla_qubits);

	// Setup the targets
	uint32_t* code_qubits = target_qubits_create_range(0, n_qubits);
	uint32_t* ancilla_qubits = target_qubits_create_range(n_qubits, n_ancilla_qubits + n_qubits);

	sym_iter* siter = sym_iter_create_n_qubits(n_qubits);
	while (sym_iter_next(siter))
	{
		if (initial_error_probs[sym_iter_ll_from_state(siter)] > 0)
		{
			sym* error_state = sym_copy(siter->state);
			sym* syndrome = sym_syndrome(code, error_state);
			sym* syndrome_state = sym_create(1, n_ancilla_qubits * 2);

			// And return from the syndrome to the proper syndrome state
			for (int i = 0; i < n_ancilla_qubits; i++)
			{
				sym_set_X(syndrome_state, 0, i, sym_get(syndrome, i, 0));	
			}

			// Combine the syndrome with the error that caused it
			sym* bare_combined_state = sym_create(1, 2 * (n_qubits + n_ancilla_qubits));
			sym* code_combined_state = sym_partial_add(bare_combined_state, error_state, code_qubits);
			sym* pre_measurement_state = sym_partial_add(code_combined_state, syndrome_state, ancilla_qubits);
			
			printf("Error Rate %e\n", initial_error_probs[sym_iter_ll_from_state(siter)]);
			printf("Initial Error: \t\t");
			sym_print(error_state);

			printf("Syndrome: \t\t");
			sym_print(syndrome_state);

			printf("Measured State: \t");
			sym_print(pre_measurement_state);
			printf("#######\n");

			// Setup the pre-recovery state in the distribution
			syndrome_error_probs[sym_to_ll(pre_measurement_state)] += initial_error_probs[sym_iter_ll_from_state(siter)];

			// Cleanup
			sym_free(error_state);
			sym_free(syndrome);
			sym_free(syndrome_state);
			sym_free(bare_combined_state);
			sym_free(code_combined_state);
			sym_free(pre_measurement_state);
		}
	}
	sym_iter_free(siter);
	
	double* recovered_state_destab = circuit_run(recovery_destab, syndrome_error_probs, NULL);	
	double* recovered_state_tailored = circuit_run(recovery_tailored, syndrome_error_probs, NULL);

	printf("Destabiliser Results\n");
	double* logical_rates = characterise_code_corrected(code, logicals, recovered_state_destab);
	siter = sym_iter_create_n_qubits(logicals->n_qubits);
	while(sym_iter_next(siter))
	{
		printf("%e \t", logical_rates[sym_iter_ll_from_state(siter)]);
		sym_print(siter->state);
	}
	
	printf("Tailored Results\n");
	logical_rates = characterise_code_corrected(code, logicals, recovered_state_tailored);
	siter = sym_iter_create_n_qubits(logicals->n_qubits);
	while(sym_iter_next(siter))
	{
		printf("%e \t", logical_rates[sym_iter_ll_from_state(siter)]);
		sym_print(siter->state);
	}

	return 0;
}