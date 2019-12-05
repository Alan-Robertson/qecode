#ifndef CIRCUIT_SYNDROME_MEASUREMENT_FLAG_FT
#define CIRCUIT_SYNDROME_MEASUREMENT_FLAG_FT


//#define CIRCUIT_SYNDROME_MEASUREMENT_FLAG_FT_CIRCUIT_PRINT_PROGRESS
// Puts a progress bar over the circuit progress

// ----------------------------------------------------------------------------------------
// DIRECTIVES
// ----------------------------------------------------------------------------------------

#include "../sym.h"
#include "circuit.h"
#include "recovery.h"
#include "error_probabilities.h"
#include "../decoders/lookup.h"

#include "../characterise.h"

#ifdef CIRCUIT_SYNDROME_MEASUREMENT_FLAG_FT_CIRCUIT_PRINT_PROGRESS
    #include "../misc/progress_bar.h"
#endif
// ----------------------------------------------------------------------------------------
// STRUCTS
// ----------------------------------------------------------------------------------------

typedef struct {
    uint32_t n_code_qubits;
    uint32_t n_ancilla_qubits;
    uint32_t n_flag_qubits;
    circuit** flag_recovery_circuits; // These are a collection of individual recovery circuits 
    // that manage the flag ft recovery
    circuit** sub_circuits; // These handle each of the measurement operations
    gate* measure_ancillas;
    gate* measure_flags;
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

/*
 * syndrome_measurement_flag_ft_circuit_free
 * Destructor for the circuit parameters
 * :: circuit* c :: The circuit to free
 * Returns nothing
 */
void syndrome_measurement_flag_ft_circuit_param_free(void* c);


/* 
 *  circuit_flag_recovery_create:
 *  Runs the recovery circuit, this should be referenced by the circuit.circuit_operation struct member of the circuit struct
 *  :: const uint32_t n_code_qubits :: Number of code qubits
 *  :: const uint32_t n_ancilla_qubits :: Number of ancilla qubits
 *  :: decoder* d :: The decoder used with the syndrome information
 *  :: gate* pauli_X :: The pauli X gate to be applied during recovery
 *  :: gate* pauli_Z :: The pauli Z gate to be applied during recovery
 *  :: gate* measure :: A measurement operation
 *  Returns a circuit object that performs the recovery operation dictated by the decoder
 */
circuit* circuit_flag_recovery_create(
    const uint32_t n_code_qubits,
    const uint32_t n_ancilla_qubits,
    decoder* d,
    gate* pauli_X,
    gate* pauli_Z,
    gate* measure);


/* 
 *  circuit_flag_recovery_run:
 *  Runs the recovery circuit, this should be referenced by the circuit.circuit_operation struct member of the circuit struct
 *  :: circuit* recovery :: The circuit object
 *  :: double* initial_error_rates ::  The initial error probabilities associated with each pauli string
 *  :: gate* noise :: Noise operation acting on the wires in the circuit
 *  Returns a heap pointer to the new matrix
 */
double* circuit_flag_recovery_run(
    circuit* recovery, 
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
    syndrome_measurement->circuit_param_free = syndrome_measurement_flag_ft_circuit_param_free;

    // One measurement circuit for each qubit, and one for cleanup
    circuit_data->sub_circuits = (circuit**)malloc(sizeof(circuit*) * (circuit_data->n_ancilla_qubits + 1));

    // And one flag recovery circuit for each measurement circuit
    circuit_data->flag_recovery_circuits = (circuit**)malloc(sizeof(circuit*) * (circuit_data->n_ancilla_qubits));

    // Copy the flag and pauli operations over
    circuit_data->measure_flags = measure_flags;
    circuit_data->measure_ancillas = measure_ancillas;
    circuit_data->pauli_X = pauli_X;
    circuit_data->pauli_Z = pauli_Z;

    // The sub-circuits for each of the ancilla measurements and the cleanup circuit
    // Also let's setup the flag recovery circuits while we're at it
    for (uint32_t i = 0; i < circuit_data->n_ancilla_qubits; i++)
    {
        circuit_data->sub_circuits[i] = circuit_create(
            circuit_data->n_code_qubits + circuit_data->n_ancilla_qubits + circuit_data->n_flag_qubits);
        
        // The flag decoder
        decoder* d = decoder_create_lookup(circuit_data->n_flag_qubits);

        circuit_data->flag_recovery_circuits[i] = circuit_flag_recovery_create(
            circuit_data->n_code_qubits + circuit_data->n_ancilla_qubits, // Our 'code' block also contains the ancillas
            circuit_data->n_flag_qubits, // Our 'syndromes' are the flag qubits
            d,
            pauli_X, 
            pauli_Z,
            measure_flags); // We'll measure the flag qubits in here
    }

    // The cleanup circuit
    circuit_data->sub_circuits[circuit_data->n_ancilla_qubits] = circuit_create(circuit_data->n_code_qubits + circuit_data->n_ancilla_qubits + circuit_data->n_flag_qubits);

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
        measure_flags,
        measure_ancillas);

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
        //circuit_add_gate(syndrome_measurement, prepare_Z, ancilla_qubit); 
        //circuit_add_gate(circuit_data->sub_circuits[j], prepare_Z, ancilla_qubit);

        // Prepare the flags
        for (uint32_t i = 0; i < circuit_data->n_flag_qubits; i++)
        {
            //circuit_add_gate(syndrome_measurement, prepare_X, ancilla_qubit + 1 + i);
            circuit_add_gate(circuit_data->sub_circuits[j], prepare_X, flag_qubit + i);
        }       

        // Iterate over the qubits
        for (size_t i = 0; i < code->n_qubits; i++)
        {
            // Add Flag FT gate
            if (sym_is_not_I(code, j, i))
            {
                // Propagate errors to the flag qubits
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
        // This is just for printing the circuits 
        /*circuit_add_gate(syndrome_measurement, measure_ancillas, ancilla_qubit);
        for (uint32_t i = 0; i < circuit_data->n_flag_qubits; i++)
        {
            circuit_add_gate(syndrome_measurement, measure_flags, ancilla_qubit + 1 + i);
        }*/


        // Probably should just build this from our state tracker, but this will do for now
        sym* stabiliser = sym_create(1, code->length);
        sym_row_copy(stabiliser, code, 0, j);

        // Trawl the sub circuit, finding CNOT operations back to the code block
        gate_operation_f cnot_operation = cnot->operation; // Function pointer to the CNOT operation
        for (circuit_element* ce = circuit_data->sub_circuits[j]->start; ce != NULL; ce = ce->next)
        {
            if (ce->gate_operation->operation == cnot_operation
             && ce->target_qubits[0] < ancilla_qubit) // Found a cnot from the code block!
            {
                // Propagate through the rest of the circuit and see what happens to the flag qubits!
                sym* flag_bits = sym_create(1, circuit_data->n_flag_qubits);
                sym* propagated_error = sym_create(1, (circuit_data->n_code_qubits + circuit_data->n_ancilla_qubits) * 2);

                // Assume errors occur after gates are applied (this changes how we track their spread)
                for (circuit_element* cf = ce->next; cf != NULL; cf = cf->next)
                {
                    // Check if it's a CNOT gate to the flag qubit!
                    if (cf->gate_operation->operation == cnot_operation // It's a CNOT operation
                     && cf->target_qubits[0] > circuit_data->n_code_qubits) // And it's from a flag qubit
                    {
                        // XOR 1 with the flag bit to propagate the flipped error
                        sym_xor(flag_bits, 0, cf->target_qubits[0] - flag_qubit, 1);

                        // Set the Z bit on the ancilla
                        sym_set_Z(propagated_error, 0, ce->target_qubits[1], 1);

                    }

                    // Check if it's a CNOT gate to the code block!
                    if (cf->gate_operation->operation == cnot_operation // It's a CNOT operation
                     && cf->target_qubits[0] < circuit_data->n_code_qubits) // And it's into the code block
                    {
                        // Track the spread of this error
                        uint32_t stabiliser_qubit_index = cf->target_qubits[0];
                        sym_set_Z(propagated_error, 0, stabiliser_qubit_index, 1);
                    }
                }

                for (size_t i = flag_qubit; i < flag_qubit + circuit_data->n_flag_qubits; i++)
                {
                    sym_set_I(propagated_error, 0, i);    
                }
                
                // Add it to our decoder!
                decoder_lookup_insert(
                    ((circuit_recovery_data_t*)(circuit_data->flag_recovery_circuits[j]->circuit_data))->decoder_operation,  
                    flag_bits,
                    propagated_error);

                // Cleanup
                sym_free(flag_bits);
                sym_free(propagated_error);
            }
        }

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
            circuit_add_gate(circuit_data->sub_circuits[circuit_data->n_ancilla_qubits], hadamard, i);
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

    free(x_basis);
    free(y_basis);
    return;
}


void syndrome_measurement_flag_ft_circuit_param_free(void* params_v)
{
    // Unpack the syndrome measurement data
    circuit_syndrome_measurement_flag_ft_data_t* smd = (circuit_syndrome_measurement_flag_ft_data_t*)params_v;

    // Cleanup all the sub-circuits
    for (int i = 0; i <= smd->n_ancilla_qubits; i++)
    {
        circuit_free(smd->sub_circuits[i]);
    }

    // Cleanup all the recovery circuits
    for (int i = 0; i < smd->n_ancilla_qubits; i++)
    {
        // Free the decoders
        decoder_free(((circuit_recovery_data_t*)(smd->flag_recovery_circuits[i]->circuit_data))->decoder_operation);
        circuit_free(smd->flag_recovery_circuits[i]);
    }
    free(smd->sub_circuits);
    free(smd->flag_recovery_circuits);
    free(params_v);

    return;
}


/* 
 *  circuit_flag_recovery_create:
 *  Runs the recovery circuit, this should be referenced by the circuit.circuit_operation struct member of the circuit struct
 *  :: const uint32_t n_code_qubits :: Number of code qubits
 *  :: const uint32_t n_ancilla_qubits :: Number of ancilla qubits
 *  :: decoder* d :: The decoder used with the syndrome information
 *  :: gate* pauli_X :: The pauli X gate to be applied during recovery
 *  :: gate* pauli_Z :: The pauli Z gate to be applied during recovery
 *  :: gate* measure :: A measurement operation
 *  Returns a circuit object that performs the recovery operation dictated by the decoder
 */
circuit* circuit_flag_recovery_create(
    const uint32_t n_code_qubits,
    const uint32_t n_ancilla_qubits,
    decoder* d,
    gate* pauli_X,
    gate* pauli_Z,
    gate* measure)
{
    // Create the initial circuit and overwrite the default run and free operations
    circuit* recovery = circuit_create(n_code_qubits + n_ancilla_qubits);
    recovery->circuit_operation = circuit_flag_recovery_run;
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
 *  circuit_flag_recovery_run:
 *  Runs the recovery circuit, this should be referenced by the circuit.circuit_operation struct member of the circuit struct
 *  :: circuit* recovery :: The circuit object
 *  :: double* initial_error_rates ::  The initial error probabilities associated with each pauli string
 *  :: gate* noise :: Noise operation acting on the wires in the circuit
 *  Returns a heap pointer to the new matrix
 */
double* circuit_flag_recovery_run(
    circuit* recovery, 
    double* initial_error_rates, 
    gate* noise)
{
    circuit_recovery_data_t* rd = (circuit_recovery_data_t*)recovery->circuit_data;

    // Final error rates
    double* recovered_error_rates = error_probabilities_zeros(rd->n_code_qubits + rd->n_ancilla_qubits);

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

            // Update the recovered error rates
            recovered_error_rates[sym_to_ll(recovered_state)] += initial_error_rates[sym_iter_ll_from_state(siter)];

            sym_free(recovery_operator);
            sym_free(recovered_state);
        }
    }
    sym_iter_free(siter);
    return recovered_error_rates;
}



#endif