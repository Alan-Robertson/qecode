#ifndef CIRCUIT_SYNDROME_MEASUREMENT_FLAG_FT
#define CIRCUIT_SYNDROME_MEASUREMENT_FLAG_FT

// ----------------------------------------------------------------------------------------
// DIRECTIVES
// ----------------------------------------------------------------------------------------

#include "../sym.h"
#include "circuit.h"
#include "error_probabilities.h"
#include "../decoders/lookup.h"

// ----------------------------------------------------------------------------------------
// STRUCTS
// ----------------------------------------------------------------------------------------

typedef struct {
    uint32_t n_code_qubits;
    uint32_t n_ancilla_qubits;
    uint32_t n_flag_qubits;
    circuit** flag_recovery_circuits;
    circuit** sub_circuits;
    gate* measure_ancilla;
    gate* measure_flag;
    gate* pauli_X;
    gate* pauli_Z;
} circuit_syndrome_measurement_flag_ft_data_t;

// ----------------------------------------------------------------------------------------
// FUNCTION DECLARATIONS 
// ----------------------------------------------------------------------------------------

circuit* syndrome_measurement_flag_ft_circuit_create(
    const sym* code,
    gate* cnot,
    gate* hadamard,
    gate* phase,
    gate* pauli_X,
    gate* pauli_Z,
    gate* prepare_X,
    gate* prepare_Z,
    gate* measure_flags, // This should be a measurement gate over the flag qubits
    gate* measure_ancillas); // This should be a measurement gate over the ancilla qubits);

void syndrome_measurement_flag_ft_circuit_construct(
    circuit* syndrome_measurement,
    const sym* code,
    gate* cnot,
    gate* hadamard,
    gate* phase,
    gate* pauli_X,
    gate* pauli_Z,
    gate* prepare_X,
    gate* prepare_Z,
    gate* measure_flags, // This should be a measurement gate over the flag qubits
    gate* measure_ancillas); // This should be a measurement gate over the ancilla qubits);

/*
 * circuit_syndrome_measurement_run
 * Runs the syndrome measurement circuit, this should overload the circuit.circuit_operation member
 * :: circuit* syndrome_measurement ::
 * :: double* initial_error_rates ::
 * :: gate* noise :: 
 * Returns the probabilities associated with each output state
 */
double* circuit_syndrome_measurement_flag_ft_run(
    circuit* syndrome_measurement, 
    double* initial_error_rates, 
    gate* noise);

// ----------------------------------------------------------------------------------------
// FUNCTION DEFINITIONS
// ----------------------------------------------------------------------------------------

circuit* syndrome_measurement_flag_ft_circuit_create(
    const sym* code,
    gate* cnot,
    gate* hadamard,
    gate* phase,
    gate* pauli_X,
    gate* pauli_Z,
    gate* prepare_X,
    gate* prepare_Z,
    gate* measure_flags,
    gate* measure_ancillas)
{
    // Setup the circuit data
    circuit_syndrome_measurement_flag_ft_data_t* circuit_data = (circuit_syndrome_measurement_flag_ft_data_t*)malloc(sizeof(circuit_syndrome_measurement_flag_ft_data_t));
    circuit_data->n_code_qubits = code->n_qubits;
    circuit_data->n_ancilla_qubits = code->height;

    // How many flag qubits do we need?
    uint32_t weight = 0;
    circuit_data->n_flag_qubits = 0;
    for (uint32_t i = 0; i < code->height; i++)
    {
        // We don't care about any errors on the last gate, it can't propagate back to the code block
        uint32_t row_weight = -1;
        for (uint32_t j = 0; j < code->n_qubits; j++)
        {
            row_weight += sym_is_not_I(code, i, j);
        }

        if (weight < row_weight)
        {
            weight = row_weight;
        }
    }

    // See if we need to add more flag qubits
    while (weight > 1 << circuit_data->n_flag_qubits)
    {
        circuit_data->n_flag_qubits++;
    }

    // Create the circuit itself
    // Qubits 0 -> n_qubits are the regular qubits, the others are ancillas and then flags
    circuit* syndrome_measurement = circuit_create(code->n_qubits + code->height + circuit_data->n_flag_qubits);

    // Override the operation that runs the circuit
    syndrome_measurement->circuit_operation = circuit_syndrome_measurement_flag_ft_run;

    // One measurement circuit for each qubit, and one for cleanup
    circuit_data->sub_circuits = (circuit**)malloc(sizeof(circuit*) * (circuit_data->n_ancilla_qubits + 1));

    // And one flag recovery circuit for each measurement circuit
    circuit_data->flag_recovery_circuits = (circuit**)malloc(sizeof(circuit*) * (circuit_data->n_ancilla_qubits));

    // Copy the flag and pauli operations over
    circuit_data->measure_flag = measure_flag;
    circuit_data->measure_ancilla = measure_ancilla;
    circuit_data->pauli_X = pauli_X;
    circuit_data->pauli_Z = pauli_Z;

    // The sub-circuits for each of the ancilla measurements and the cleanup circuit
    for (uint32_t i = 0; i <= circuit_data->n_ancilla_qubits; i++)
    {
        circuit_data->sub_circuits[i] = circuit_create(circuit_data->n_code_qubits + circuit_data->n_ancilla_qubits + circuit_data->n_flag_qubits);
    }

    syndrome_measurement->circuit_data = circuit_data;

    // Construct the gates for the circuit
    syndrome_measurement_flag_ft_circuit_construct(
        syndrome_measurement, 
        code, 
        cnot, 
        hadamard, 
        phase, 
        pauli_X,
        pauli_Z,
        prepare_X,
        prepare_Z,
        measure_X,
        measure_Z);

    return syndrome_measurement;
}

void syndrome_measurement_flag_ft_circuit_construct(
    circuit* syndrome_measurement,
    const sym* code,
    gate* cnot,
    gate* hadamard,
    gate* phase,
    gate* pauli_X,
    gate* pauli_Z,
    gate* prepare_X,
    gate* prepare_Z,
    gate* measure_flags,
    gate* measure_ancillas)
{
    circuit_syndrome_measurement_flag_ft_data_t* circuit_data = (circuit_syndrome_measurement_flag_ft_data_t*) syndrome_measurement->circuit_data;

    size_t ancilla_qubit = code->n_qubits;
    size_t flag_qubit = circuit_data->n_code_qubits + circuit_data->n_ancilla_qubits;

    // Keep track of the basis transforms on the code qubits, starts in the Z basis
    uint8_t* x_basis = (uint8_t*)calloc(code->height, sizeof(uint8_t));
    uint8_t* y_basis = (uint8_t*)calloc(code->height, sizeof(uint8_t));

    // This is a strictly poor way of setting up the flagged FT, but it works for weight 4 and below measurements
    // Future work can improve on this particular scheme
    uint32_t curr_flag = 0;

    // Iterate through ancillas
    for (size_t j = 0; j < code->height; j++)
    {
        // Determine the weight of this particular stabiliser
        // This is equivalent to the number of CNOTs performed between the code block and the ancilla
        // We will be using this to determine what the flag operations should be
        uint32_t weight = 0;
        for (size_t i = 0; i < code->n_qubits; i++)
        {
            weight += sym_is_not_I(code, j, i);
        }

        // Prepare the ancillas
        circuit_add_gate(syndrome_measurement, prepare_Z, ancilla_qubit); 
        circuit_add_gate(circuit_data->sub_circuits[j], prepare_Z, ancilla_qubit);

        // Prepare the flags
        for (uint32_t i = 0; i < circuit_data->n_flag_qubits; i++)
        {
            circuit_add_gate(syndrome_measurement, prepare_X, ancilla_qubit + 1 + i);
            circuit_add_gate(circuit_data->sub_circuits[j], prepare_X, flag_qubit + i);
        }       

        // Iterate over the qubits
        for (size_t i = 0; i < code->n_qubits; i++)
        {
            // Add Flag FT gate
            if (sym_is_not_I(code, j, i))
            {
                // Propagate errors to the flag qubit
                circuit_add_gate(syndrome_measurement, cnot, ancilla_qubit + curr_flag + 1, ancilla_qubit);
                circuit_add_gate(circuit_data->sub_circuits[j], cnot, flag_qubit + curr_flag, ancilla_qubit + j);

                // This needs replacing with a better method, but it will do for now
                curr_flag++;
                curr_flag %= circuit_data->n_flag_qubits;

                // Pauli Z checks
                if (sym_is_Z(code, j, i))
                {
                    // Move out of the X basis
                    if (x_basis[i] == true)
                    {
                        circuit_add_gate(syndrome_measurement, hadamard, i);
                        circuit_add_gate(circuit_data->sub_circuits[j], hadamard, i);
                        x_basis[i] = false;
                    }

                    // Move out of the Y basis
                    if (y_basis[i] == true)
                    {
                        circuit_add_gate(syndrome_measurement, hadamard, i);
                        circuit_add_gate(syndrome_measurement, phase, i);
                        circuit_add_gate(circuit_data->sub_circuits[j], hadamard, i);
                        circuit_add_gate(circuit_data->sub_circuits[j], phase, i);
                        y_basis[i] = false;
                    }

                    // Cnot in the Z basis
                    circuit_add_gate(syndrome_measurement, cnot, i, ancilla_qubit);
                    circuit_add_gate(circuit_data->sub_circuits[j], cnot, i, ancilla_qubit + j);
                }
                else if (sym_is_X(code, j, i))
                {
                    if (y_basis[i] == true)
                    {
                        circuit_add_gate(syndrome_measurement, hadamard, i);
                        circuit_add_gate(syndrome_measurement, phase, i);
                        circuit_add_gate(circuit_data->sub_circuits[j], hadamard, i);
                        circuit_add_gate(circuit_data->sub_circuits[j], phase, i);
                    }
                    if (x_basis[i] == false)
                    {
                        circuit_add_gate(syndrome_measurement, hadamard, i);
                        circuit_add_gate(circuit_data->sub_circuits[j], hadamard, i);
                        x_basis[i] = true;
                    }
                    circuit_add_gate(syndrome_measurement, cnot, i, ancilla_qubit);
                    circuit_add_gate(circuit_data->sub_circuits[j], cnot, i, ancilla_qubit + j);
                }
                else if (sym_is_Y(code, j, i))
                {
                    if (x_basis[i] == true)
                    {
                        circuit_add_gate(syndrome_measurement, hadamard, i);
                        circuit_add_gate(circuit_data->sub_circuits[j], hadamard, i);
                        x_basis[i] = false;
                    }

                    if (y_basis[i] == false)
                    { // Map Y to Z then cnot to pass the error along
                        circuit_add_gate(syndrome_measurement, phase, i);
                        circuit_add_gate(syndrome_measurement, phase, i);
                        circuit_add_gate(syndrome_measurement, phase, i);
                        circuit_add_gate(syndrome_measurement, hadamard, i);
                        circuit_add_gate(circuit_data->sub_circuits[j], phase, i);
                        circuit_add_gate(circuit_data->sub_circuits[j], phase, i);
                        circuit_add_gate(circuit_data->sub_circuits[j], phase, i);
                        circuit_add_gate(circuit_data->sub_circuits[j], hadamard, i);
                        y_basis[i] = true;
                    }
                    // Cnot in the Y basis
                    circuit_add_gate(syndrome_measurement, cnot, i, ancilla_qubit);
                    circuit_add_gate(circuit_data->sub_circuits[j], cnot, i, ancilla_qubit + j);
                }
            }
        }

        // Measure
        circuit_add_gate(syndrome_measurement, measure_Z, ancilla_qubit);
        for (uint32_t i = 0; i < circuit_data->n_flag_qubits; i++)
        {
            circuit_add_gate(syndrome_measurement, measure_X, ancilla_qubit + 1 + i);
        }

        decoder* flag_decoder = decoder_create_lookup(circuit_data->n_flag_qubits);

        sym* stabiliser = sym_row_copy(code, j);

        // Trawl the sub circuit, finding CNOT operations back to the code block
        gate_operation_f cnot_operation = cnot->operation; // Function pointer to the CNOT operation
        for (circuit_element* ce = circuit_data->sub_circuits[j]->start; ce != NULL; ce = ce->next)
        {
            if (ce->gate_operation->operation == cnot_operation
             && ce->target_qubits[0] < ancilla_qubit) // Found a cnot from the code block!
            {
                // Propagate through the rest of the circuit and see what happens to the flag qubits!
                sym* flag_bits = sym_create(1, circuit_data->n_flag_qubits);
                sym* propagated_error = sym_create(1, circuit_data->n_code_qubits + circuit_data->n_ancilla_qubits);

                for (circuit_element* cf = ce->next; cf != NULL; cf = cf->next)
                {
                    // Check if it's a CNOT gate to the flag qubit!
                    if (ce->gate_operation->operation == cnot_operation // It's a CNOT operation
                     && ce->target_qubits[0] > circuit_data->n_code_qubits) // And it's from a flag qubit
                    {
                        // XOR 1 with the flag bit to propagate the flipped error
                        sym_xor(flag_bits, 0, ce->target_qubits[0] - flag_qubit, 1);
                    }

                    // Check if it's a CNOT gate to the flag qubit!
                    if (ce->gate_operation->operation == cnot_operation // It's a CNOT operation
                     && ce->target_qubits[0] < circuit_data->n_code_qubits) // And it's into the code block
                    {
                        uint32_t stabiliser_qubit_index = ce->target_qubits[0];
                        // Copy the element from the stabiliser
                        sym_set_X(propagated_error, 0, stabiliser_qubit_index, sym_get_X(stabiliser, 0, stabiliser_qubit_index));
                        sym_set_Z(propagated_error, 0, stabiliser_qubit_index, sym_get_Z(stabiliser, 0, stabiliser_qubit_index));
                    }
                }

                // Add it to our decoder!
                decoder_lookup_insert(circuit_data->flag_decoders[j], flag_bits, propagated_error);
                // Cleanup
                sym_free(flag_bits);
                sym_free(propagated_error);
            }
        }

        // Create the flag recovery circuit for this syndrome measurement circuit
        circuit_data->flag_recovery_circuits[j] = circuit_recovery_create(
            circuit_data->n_code_qubits + circuit_data->n_ancilla_qubits,
            circuit_data->n_flag_qubits,
            flag_decoder,
            pauli_X,
            pauli_Z,

            );

        // Cleanup
        sym_free(stabiliser);
    }

    
    // Cleanup any residual transformations
    // This occurs on the last sub-circuit
    for (int i = 0; i < code->n_qubits; i++)
    {
        if (x_basis[i] == true)
        {
            circuit_add_gate(syndrome_measurement, hadamard, i);
            circuit_add_gate(circuit_data->sub_circuits[circuit_data->n_ancilla_qubits - 1], hadamard, i);
            x_basis[i] = false;
        }

        if (y_basis[i] == true)
        {
            circuit_add_gate(syndrome_measurement, hadamard, i);
            circuit_add_gate(syndrome_measurement, phase, i);
            circuit_add_gate(circuit_data->sub_circuits[circuit_data->n_ancilla_qubits], hadamard, i);
            circuit_add_gate(circuit_data->sub_circuits[circuit_data->n_ancilla_qubits], phase, i);
        }
    }
    return;
}

/*
 * circuit_syndrome_measurement_run
 * 
 * :: circuit* syndrome_measurement ::
 * :: double* initial_error_rates ::
 * :: gate* noise :: 
 * 
 */
double* circuit_syndrome_measurement_flag_ft_run(
    circuit* syndrome_measurement, 
    double* initial_error_rates, 
    gate* noise)
{
    // Unpack the syndrome measurement data
    circuit_syndrome_measurement_flag_ft_data_t* smd = (circuit_syndrome_measurement_flag_ft_data_t*)syndrome_measurement->circuit_data;

    // Setup the larger state space
    double* expanded_error_probs = error_probabilities_zeros(smd->n_code_qubits + smd->n_ancilla_qubits);

    // Copy the errors from the initial buffer to our larger buffer
    sym_iter* cpy_iter = sym_iter_create_n_qubits(smd->n_code_qubits);
    sym_iter* target_buffer = sym_iter_create_n_qubits(smd->n_code_qubits + 1);
    while (sym_iter_next(cpy_iter))
    {
        if (initial_error_rates[sym_iter_ll_from_state(cpy_iter)] > 0)
        {
            // Copy the state to the target buffer
            for (uint32_t i = 0; i < smd->n_code_qubits; i++)
            { // All other values in the target buffer should be zero
                sym_set(target_buffer->state, 0, i, sym_get(cpy_iter->state, 0, i)); // X elements
                sym_set(target_buffer->state, 0, i + target_buffer->state->n_qubits, sym_get(cpy_iter->state, 0, i + cpy_iter->state->n_qubits)); // Z elements
            } 

            // Set the value in the new buffer
            expanded_error_probs[sym_iter_ll_from_state(target_buffer)] += initial_error_rates[sym_iter_ll_from_state(cpy_iter)];
        }
    }
    sym_iter_free(target_buffer);
    sym_iter_free(cpy_iter);

    unsigned long n_bytes_code = error_probabilities_bytes_in_table(smd->n_code_qubits);
    unsigned long n_bytes_ancilla = error_probabilities_bytes_in_table(smd->n_code_qubits + smd->n_ancilla_qubits);
    unsigned long n_bytes_ft = error_probabilities_bytes_in_table(smd->n_code_qubits + smd->n_ancilla_qubits + smd->n_flag_qubits);
    
    // Run the sub-circuits
    // These let us track the "active" ancilla qubit while leaving the others untouched
    for (uint32_t i = 0; i <= smd->n_ancilla_qubits; i++)
    {
        circuit* c = smd->sub_circuits[i];
        circuit_element* ce = c->start;
        uint32_t active_ancilla = i + smd->n_code_qubits;

        while (NULL != ce)
        {
            // Gate operation
            double* tmp_error_rate = gate_apply(c->n_qubits, expanded_error_probs, ce->gate_operation, ce->target_qubits);
            memcpy(expanded_error_probs, tmp_error_rate, n_bytes);
            free(tmp_error_rate);
        
            // Environmental Noise operations on code block qubits
            if (noise != NULL)
            {
                for (unsigned j = 0; j < smd->n_code_qubits; j++)
                {
                    for (uint32_t k = 0; k < ce->gate_operation->n_qubits; k++)
                    {   
                        // No Noise on inactive ancillas!
                        if (j != ce->target_qubits[k])
                        {
                            double* tmp_error_rate = gate_apply(c->n_qubits, expanded_error_probs, noise, &j);
                            memcpy(expanded_error_probs, tmp_error_rate, n_bytes);
                            free(tmp_error_rate);
                        }
                    }
                }

                // We don't need to worry about this section once all the ancillas have been measured
                if (active_ancilla < smd->n_ancilla_qubits)
                {
                    // Environmental Noise operations on ancilla qubits
                    // No noise on the inactive ancillas!
                    uint8_t ancilla_used = false;
                    for (uint32_t k = 0; k < ce->gate_operation->n_qubits; k++)
                    {   
                        if (active_ancilla != ce->target_qubits[k])
                        {
                            ancilla_used = true;
                        }
                    }   

                    if (false == ancilla_used)
                    {
                        double* tmp_error_rate = gate_apply(c->n_qubits, expanded_error_probs, noise, &active_ancilla);
                        memcpy(expanded_error_probs, tmp_error_rate, n_bytes);
                        free(tmp_error_rate);  
                    }           
                }
            }
            // Next circuit element
            ce = ce->next;
        }

        // Apply FT correction circuit
        if (i != smd->n_ancilla_qubits) // We don't do any flag FT correction on the cleanup sub-circuit
        {
            double* tmp_error_rate = circuit_run(smd->flag_recovery_circuits[i]);    
        }
        
    }

    // Cleanup anything that needs to be de-allocated
    //error_probabilities_free(expanded_error_probs);

    return expanded_error_probs;
}


#endif