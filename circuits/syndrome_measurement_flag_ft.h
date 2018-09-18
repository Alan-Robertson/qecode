#ifndef CIRCUIT_SYNDROME_MEASUREMENT_FLAG_FT
#define CIRCUIT_SYNDROME_MEASUREMENT_FLAG_FT

// ----------------------------------------------------------------------------------------
// DIRECTIVES
// ----------------------------------------------------------------------------------------

#include "../sym.h"
#include "circuit.h"
#include "error_probabilities.h"
#include "decoders/lookup.h"

// ----------------------------------------------------------------------------------------
// STRUCTS
// ----------------------------------------------------------------------------------------

typedef struct {
    uint32_t n_code_qubits;
    uint32_t n_ancilla_qubits;
    uint32_t n_flag_qubits;
    decoder** flag_decoders;
    circuit** sub_circuits;
} circuit_syndrome_measurement_sequential_data_t;

// ----------------------------------------------------------------------------------------
// FUNCTION DECLARATIONS 
// ----------------------------------------------------------------------------------------

circuit* syndrome_measurement_sequential_circuit(
    const sym* code,
    gate* cnot,
    gate* hadamard,
    gate* phase,
    gate* prepare_z,
    gate* measure_z,
    gate* prepare_x,
    gate* measure_x);

void syndrome_measurement_sequential_circuit_construct(
    circuit* syndrome_measurement,
    const sym* code,
    gate* cnot,
    gate* hadamard,
    gate* phase,
    gate* prepare_z,
    gate* measure_z,
    gate* prepare_x,
    gate* measure_x);

/*
 * circuit_syndrome_measurement_run
 * 
 * :: circuit* syndrome_measurement ::
 * :: double* initial_error_rates ::
 * :: gate* noise :: 
 * 
 */
double* circuit_syndrome_measurement_sequential_run(
    circuit* syndrome_measurement, 
    double* initial_error_rates, 
    gate* noise);

// ----------------------------------------------------------------------------------------
// FUNCTION DEFINITIONS
// ----------------------------------------------------------------------------------------

circuit* syndrome_measurement_flag_ft(
    const sym* code,
    gate* cnot,
    gate* hadamard,
    gate* phase,
    gate* prepare_z,
    gate* measure_z,
    gate* prepare_x,
    gate* measure_x)
{
    // Qubits 0 -> n_qubits are the regular qubits, the others are ancillas
    circuit* syndrome_measurement = circuit_create(code->n_qubits + code->height);

    // Override the operation that runs the circuit
    syndrome_measurement->circuit_operation = circuit_syndrome_measurement_flag_ft_run;

    // Setup the circuit data
    circuit_syndrome_measurement_sequential_data_t* circuit_data = (circuit_syndrome_measurement_sequential_data_t*)malloc(sizeof(circuit_syndrome_measurement_sequential_data_t));
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

    // Add the flag qubits to be tracked
    syndrome_measurement->n_qubits += circuit_data->n_flag_qubits;

    // One measurement circuit for each qubit, and one for cleanup
    circuit_data->sub_circuits = (circuit**)malloc(sizeof(circuit*) * (circuit_data->n_ancilla_qubits + 1));

    // One final sub-circuit for any cleanup

    // The sub-circuits for each of the ancilla measurements
    for (uint32_t i = 0; i <= circuit_data->n_ancilla_qubits; i++)
    {
        circuit_data->sub_circuits[i] = circuit_create(circuit_data->n_code_qubits + circuit_data->n_ancilla_qubits + circuit_data->n_flag_qubits);
    }

    syndrome_measurement->circuit_data = circuit_data;

    // Construct the gates for the circuit
    syndrome_measurement_sequential_circuit_construct(syndrome_measurement, code, cnot, hadamard, phase, measure_z);

    return syndrome_measurement;
}

void syndrome_measurement_flag_ft_circuit_construct(
    circuit* syndrome_measurement,
    const sym* code,
    gate* cnot,
    gate* hadamard,
    gate* phase,
    gate* prepare_z,
    gate* measure_z,
    gate* prepare_x,
    gate* measure_x);
{
    circuit_syndrome_measurement_sequential_data_t* circuit_data = (circuit_syndrome_measurement_sequential_data_t*) syndrome_measurement->circuit_data;

    size_t ancilla_qubit = code->n_qubits;
    size_t flag_qubit = circuit_data->n_code_qubits + circuit_data->n_ancilla_qubits;

    // Keep track of the basis transforms on the code qubits, starts in the Z basis
    uint8_t* x_basis = (uint8_t*)calloc(code->height, sizeof(uint8_t));
    uint8_t* y_basis = (uint8_t*)calloc(code->height, sizeof(uint8_t));


    // This is a stricly poor way of setting up the flagged FT, but it works for weight 4 and below measurements
    uint32_t curr_flag;

    // Iterate through ancillas
    for (size_t j = 0; j < code->height; j++)
    {
        circuit_add_gate(syndrome_measurement, prepare_z, i);
        circuit_add_gate(circuit_data->sub_circuits[j], prepare_z, i);

        // Prepare the flags
        for (uint32_t i = 0; i < code->n_flag_qubits; i++)
        {
            
        }

        // Determine the weight of this particular stabiliser
        // This is equivalent to the number of CNOTs performed between the code block and the ancilla
        uint32_t weight = 0;
        for (size_t i = 0; i < code->n_qubits; i++)
        {
            weight += sym_is_not_I(code, j, i);
        }

        // Iterate over the qubits
        for (size_t i = 0; i < code->n_qubits; i++)
        {
            // Add Flag FT gate
            if (sym_is_not_I(code, j, i))
            {
                // Propagate errors to the flag qubit
                circuit_add_gate(syndrome_measurement, cnot, ancilla + curr_flag + 1, ancilla_qubit);
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

        // Measure!!
        circuit_add_gate(syndrome_measurement, measure_z, ancilla_qubit + j);
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
    circuit_syndrome_measurement_sequential_data_t* smd = (circuit_syndrome_measurement_sequential_data_t*)syndrome_measurement->circuit_data;

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

    unsigned long n_bytes = (1ull << ((smd->n_code_qubits + smd->n_ancilla_qubits) * 2)) * sizeof(double);

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
    }

    // Cleanup anything that needs to be de-allocated
    //error_probabilities_free(expanded_error_probs);

    return expanded_error_probs;
}

decoder* circuit_syndrome_measurement_flag_ft_build_decoder(circuit* subcircuit)
{

};


#endif