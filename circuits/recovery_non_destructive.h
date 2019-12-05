#ifndef CIRCUIT_RECOVERY
#define CIRCUIT_RECOVERY

// ----------------------------------------------------------------------------------------
// DIRECTIVES
// ----------------------------------------------------------------------------------------

#include "../sym.h"
#include "circuit.h"
#include "recovery.h"
#include "error_probabilities.h"
#include "../decoders/decoders.h"
#include "../gates/gates.h"



// ----------------------------------------------------------------------------------------
// FUNCTION DECLARATIONS 
// ----------------------------------------------------------------------------------------

/* 
 *  circuit_recovery_nd_run:
 *  Runs the recovery circuit, this should be referenced by the circuit.circuit_operation struct member of the circuit struct
 *  :: const uint32_t n_code_qubits :: Number of code qubits
 *  :: const uint32_t n_ancilla_qubits :: Number of ancilla qubits
 *  :: decoder* d :: The decoder used with the syndrome information
 *  :: gate* pauli_X :: The pauli X gate to be applied during recovery
 *  :: gate* pauli_Z :: The pauli Z gate to be applied during recovery
 *  :: gate* measure :: A measurement operation
 *  Returns a circuit object that performs the recovery operation dictated by the decoder
 */
circuit* circuit_recovery_nd_create(
    const uint32_t n_code_qubits,
    const uint32_t n_ancilla_qubits,
    decoder* d,
    gate* pauli_X,
    gate* pauli_Z,
    gate* measure);

/* 
 *  circuit_recovery_run:
 *  Runs the recovery circuit, this should be referenced by the circuit.circuit_operation struct member of the circuit struct
 *  :: circuit* recovery :: The circuit object
 *  :: double* initial_error_rates ::  The initial error probabilities associated with each pauli string
 *  :: gate* noise :: Noise operation acting on the wires in the circuit
 *  Returns a heap pointer to the new matrix
 */
double* circuit_recovery_nd_run(
    circuit* recovery, 
    double* initial_error_rates, 
    gate* noise);


// ----------------------------------------------------------------------------------------
// FUNCTION DEFINITIONS
// ----------------------------------------------------------------------------------------

/* 
 *  circuit_recovery_run:
 *  Runs the recovery circuit, this should be referenced by the circuit.circuit_operation struct member of the circuit struct
 *  :: const uint32_t n_code_qubits :: Number of code qubits
 *  :: const uint32_t n_ancilla_qubits :: Number of ancilla qubits
 *  :: decoder* d :: The decoder used with the syndrome information
 *  :: gate* pauli_X :: The pauli X gate to be applied during recovery
 *  :: gate* pauli_Z :: The pauli Z gate to be applied during recovery
 *  :: gate* measure :: A measurement operation
 *  Returns a circuit object that performs the recovery operation dictated by the decoder
 */
circuit* circuit_recovery_nd_create(
    const uint32_t n_code_qubits,
    const uint32_t n_ancilla_qubits,
    decoder* d,
    gate* pauli_X,
    gate* pauli_Z,
    gate* measure)
{
    // Create the initial circuit and overwrite the default run and free operations
    circuit* recovery = circuit_create(n_code_qubits + n_ancilla_qubits);
    recovery->circuit_operation = circuit_recovery_nd_run;
    recovery->circuit_param_free = circuit_recovery_param_free;
    
    // Setup our recovery data
    // Note that most of this is just copying the pointer; we shouldn't free it ourselves
    circuit_recovery_data_t* rd = (circuit_recovery_data_t*)malloc(sizeof(circuit_recovery_data_t));

    // Copy the number of code block and ancilla block qubits
    rd->n_code_qubits = n_code_qubits;
    rd->n_ancilla_qubits = n_ancilla_qubits;

    // Copy the gate operations
    rd->pauli_X = pauli_X;
    rd->pauli_Z = pauli_Z;
    rd->measure = measure;
    rd->decoder_operation = d;

    // Measure the ancilla qubits to get the syndromes
    uint32_t* measurement_targets = target_qubits_create_range(n_code_qubits, n_code_qubits + n_ancilla_qubits);
    rd->measurement_targets = measurement_targets;

    // Link up the recovery data
    recovery->circuit_data = rd;

    // And return our recovery 'circuit'
    return recovery;
}

/* 
 *  circuit_recovery_run:
 *  Runs the recovery circuit, this should be referenced by the circuit.circuit_operation struct member of the circuit struct
 *  :: circuit* recovery :: The circuit object
 *  :: double* initial_error_rates ::  The initial error probabilities associated with each pauli string
 *  :: gate* noise :: Noise operation acting on the wires in the circuit
 *  Returns a heap pointer to the new matrix
 */
double* circuit_recovery_nd_run(
    circuit* recovery, 
    double* initial_error_rates, 
    gate* noise)
{
    circuit_recovery_data_t* rd = (circuit_recovery_data_t*)recovery->circuit_data;

    // Final error rates
    double* recovered_error_rates = error_probabilities_zeros(rd->n_code_qubits);

    // For stripping the ancilla qubits
    sym_iter* target_buffer = sym_iter_create_n_qubits(rd->n_code_qubits);

    // Iterate over the set of states
    sym_iter* siter = sym_iter_create_n_qubits(rd->n_code_qubits + rd->n_ancilla_qubits);
    while (sym_iter_next(siter))
    {   
        if (initial_error_rates[sym_iter_ll_from_state(siter)] > 0)
        {

            // Measure the syndrome bits
            gate_result* syndrome_results = gate_operation(rd->measure, siter->state, rd->measurement_targets);
            sym* syndrome = sym_copy(syndrome_results->state_results[0]);
            gate_result_free(syndrome_results);


            // Transpose the syndrome for easier reading
            sym* syndrome_trans = sym_transpose(syndrome);
            sym_free(syndrome);
        
            // Decode to determine the recovery operation required
            sym* recovery_operator = decoder_call(rd->decoder_operation, syndrome_trans);



            if (NULL == recovery_operator) // Decoder table has no entry for this syndrome
            { // Give a blank recovery operation
                recovery_operator = sym_create(1, rd->n_code_qubits * 2);
            }

            sym_free(syndrome_trans);

            // Recover the state
            sym* recovered_state = sym_copy(siter->state);
            for (uint32_t i = 0; i < recovery_operator->n_qubits; i++)
            {
                // Apply X operations where required
                if (sym_get_X(recovery_operator, 0, i))
                {
                    gate_result* applied_result = gate_operation(rd->pauli_X, recovered_state, &i);
                    sym_free(recovered_state);
                    recovered_state = sym_copy(applied_result->state_results[0]);
                    gate_result_free(applied_result);
                }

                // And apply Z operations where required
                if (sym_get_Z(recovery_operator, 0, i))
                {
                    gate_result* applied_result = gate_operation(rd->pauli_Z, recovered_state, &i);
                    sym_free(recovered_state);
                    recovered_state = sym_copy(applied_result->state_results[0]);
                    gate_result_free(applied_result);
                }
            }



            sym_free(recovery_operator);


            // Copy the state to the target buffer
            // TODO: use the pauli gates to do this
            for (uint32_t i = 0; i < rd->n_code_qubits; i++)
            { // All other values in the target buffer should be zero
                sym_set_X(target_buffer->state, 0, i, sym_get_X(recovered_state, 0, i)); // X elements
                sym_set_Z(target_buffer->state, 0, i, sym_get_Z(recovered_state, 0, i)); // Z elements 
            } 
            sym_free(recovered_state);
            sym_iter_update(target_buffer);
            
            // Set the value in the new buffer
            recovered_error_rates[sym_iter_ll_from_state(target_buffer)] += initial_error_rates[sym_iter_ll_from_state(siter)];
        }
    }
    sym_iter_free(siter);
    sym_iter_free(target_buffer);
    return recovered_error_rates;
}

#endif