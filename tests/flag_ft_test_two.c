//#define GATE_MULTITHREADING
//#define N_THREADS 2

//#define CIRCUIT_SYNDROME_MEASUREMENT_FLAG_FT_CIRCUIT_PRINT_PROGRESS

#include "../codes/codes.h"
#include "../codes/candidate_codes.h"

#include "../decoders/destabiliser.h"
#include "../decoders/tailored.h"

#include "../gates/measurement.h"
#include "../gates/preparation.h"
#include "../gates/pauli_generators.h"
#include "../gates/clifford_generators.h"

#include "../error_models/lookup.h"
#include "../error_models/iid.h"

#include "../circuits/syndrome_measurement_flag_ft.h"

#include "../sym_iter.h"
#include "../characterise.h"

#include "../misc/qcircuit.h"


double* circuit_syndrome_measurement_flag_ft_run_debug(
    circuit* syndrome_measurement, 
    double* initial_error_rates, 
    gate* noise);


uint8_t glob_location = 7;
uint8_t block_location = 9;
gate* glob_pauli_Z = gate_create(1, gate_pauli_Z, NULL, NULL);
unsigned glob_target_qubits[] = {5};


/*
 *  Check the recovery circuit
 */

int main(int argv, char** argc)
{   

    // if (argv == 3)
    // {
    //     glob_location = atoi(argc[1]);
    //     block_location = atoi(argc[2]);   
    // }


    double p_gate_error = 0.000001;

    if (argv == 2)
    {
        p_gate_error = atof(argc[1]);
    }    


    printf("Error Block: %d\n", block_location);
    printf("Error Location: %d\n", glob_location);

    //--------------------------------
    // Setup the code and the space of qubits
    //--------------------------------
    sym* code = code_steane();
    sym* logicals = code_steane_logicals();

    //decoder* destab_decoder = decoder_create_destabiliser(code, logicals);

    sym_print_pauli(code);

    unsigned n_qubits = code->n_qubits;
    unsigned n_ancilla_qubits = code->height;
    unsigned n_flag_qubits = 2;

    ///////
    glob_target_qubits[0] =  n_qubits + block_location;

    // Setup error models

    double p_wire_error = 0;

    error_model* em_cnot = error_model_create_iid(2, p_gate_error);
    error_model* em_hadamard = error_model_create_iid(1, p_gate_error);
    error_model* em_phase = error_model_create_iid(1, p_gate_error);
    error_model* em_wire = error_model_create_iid(1, p_wire_error);

    //--------------------------------
    // Setup our gates
    //--------------------------------
    // Paulis
    gate* pauli_X = gate_create(1, gate_pauli_X, NULL, NULL);
    gate* pauli_Z = gate_create(1, gate_pauli_Z, NULL, NULL);

    // State preparation
    gate* prepare_X = gate_create_prepare_X(1, 0, NULL);
    gate* prepare_Z = gate_create_prepare_Z(1, 0, NULL);

    // Measurement
    gate* measure_flags = gate_create(n_flag_qubits, gate_measure_X, NULL, NULL);
    gate* measure_ancillas = gate_create(n_ancilla_qubits, gate_measure_Z, NULL, NULL);

    // Cliffords 
    gate* cnot = gate_create(2, gate_cnot, em_cnot, NULL);
    gate* hadamard = gate_create(1, gate_hadamard, em_hadamard, NULL);
    gate* phase = gate_create(1, gate_phase, em_phase, NULL);

    // Wire Noise
    gate* wire_noise = gate_create(1, NULL, em_wire, NULL);

    //--------------------------------
    // Setup our initial error probabilities
    //--------------------------------

    // double* qwop = error_probabilities_identity(16);
    // qwop[1] = 1.0;
    // qwop[0] = 0;

    // double* test = error_probabilities_step_down(qwop, 32, 10); 

    ////////
    //double* initial_error_probs = error_probabilities_zeros(n_qubits);
    double* initial_error_probs = error_probabilities_identity(n_qubits);
    // // Generate some single qubit X and Z errors
    // double prob = (1.0) / (code->n_qubits * 3 + 1);
    // initial_error_probs[0] = prob;
    // for (int i = 0; i < code->length; i++)
    // {
    //     initial_error_probs[1 << i] = prob;
    // }

    // // Generate some single qubit Y errors
    // for (int i = 0; i < code->n_qubits; i++)
    // {
    //     initial_error_probs[((1 << code->n_qubits) + 1) << i] = prob;
    // }

    //initial_error_probs[1 << 5] = 1.0;
    //initial_error_probs[0] = 0.0;
    ////////


    sym_iter_print_pauli(n_qubits, initial_error_probs);


    //--------------------------------------------
    // Use the above to build what we need to
    //-------------------------------------------

    // Create an iid error model
    //error_model* em =  error_model_create_iid(n_qubits, p_gate_error);

    // Note that we're going to pass this into our circuit, you might want to run the circuit using 
    // The distribution as the input, then use the output of the measurement circuit 
    // as the error model to create the decoder



    // Build a flag fault tolerant measurement circuit
    circuit* flag_ft_measurement = syndrome_measurement_flag_ft_circuit_create(
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

;

    // Build a real decoder
    //decoder* tailored = decoder_create_tailored(code, logicals, uniform);

    
    printf("Measuring the state\n");   
    
    double* measured_state = circuit_syndrome_measurement_flag_ft_run(flag_ft_measurement, initial_error_probs, wire_noise);  

    double* register_state = error_probabilities_step_down(measured_state, n_qubits + n_ancilla_qubits, n_qubits);


    error_model* em = error_model_create_lookup(n_qubits, register_state);

    //error_model* em = error_model_create_iid(n_qubits, 1 - register_state[0]);


     // Tailor a decoder
    decoder* tailored_decoder = decoder_create_tailored(code, logicals, em);

    // // Tailor a decoder
    decoder* iid_decoder = decoder_create_tailored(code, logicals, em);


    // Build a recovery circuit
    circuit* recovery_tailored = circuit_recovery_create(
        n_qubits,
        n_ancilla_qubits,
        tailored_decoder,
        pauli_X,
        pauli_Z,
        measure_ancillas);


    printf("Recovering the state\n");
    //double* recovered_state_destab = circuit_run(recovery_destab, measured_state, NULL);
    //sym_iter_print_pauli(5, recovered_state_destab);
    double* recovered_state_tailored = circuit_run(recovery_tailored, measured_state, NULL);



    printf("Results\n");
    double* logical_rates_tailored = characterise_code_corrected(code, logicals, recovered_state_tailored);
    //double* logical_rates_tailored = characterise_code_corrected(code, logicals, recovered_state_tailored);

    sym_iter_print_pauli(1, logical_rates_tailored);


    //printf("Tailored:\n");
    //sym_iter_print_pauli(1, recovered_state_tailored);


    // printf("\nIID:\n");
    // sym_iter_print_pauli(n_qubits, recovered_state_iid);

    printf("Physical: %e, Dressed: %e\n", p_gate_error, 1 - measured_state[0]);
    //printf("Tailored: (%e)\n", logical_rates_destab[0]);
    // printf("IID: (%e)\n", logical_rates_iid[0]);
    sym_iter_print_pauli(1, logical_rates_tailored);


    // // Cleanup
    // sym_multi_free(2, code, logicals);  
    // gate_multi_free(9, pauli_X, pauli_Z, prepare_X, prepare_Z, measure_flags, measure_ancillas, cnot, hadamard, phase);

    // error_model_free(em);
    // error_model_free(em_cnot);

    // //decoder_free(tailored);

    // free(initial_error_probs);
    // circuit_free(flag_ft_measurement);

    //circuit_free(recovery);

    //free(measured_state);
    //free(recovered_state);
    return 0;
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

    // Setup our register for repeat measurements
    //double* repeat_measurement_probs = error_probabilities_zeros(smd->n_code_qubits);

    // Setup the larger state space
    double* syndrome_error_probs = error_probabilities_step_up(initial_error_rates, 
        smd->n_code_qubits, 
        smd->n_code_qubits + smd->n_ancilla_qubits + smd->n_flag_qubits);

    double* remeasurement_probs = error_probabilities_zeros(smd->n_code_qubits + smd->n_ancilla_qubits + smd->n_flag_qubits);

    unsigned long n_bytes_code = error_probabilities_bytes_in_table(smd->n_code_qubits);
    unsigned long n_bytes_ancilla = error_probabilities_bytes_in_table(smd->n_code_qubits + smd->n_ancilla_qubits);
    unsigned long n_bytes_ft = error_probabilities_bytes_in_table(smd->n_code_qubits + smd->n_ancilla_qubits + smd->n_flag_qubits);

    // Run the sub-circuits
    // These let us track the "active" ancilla qubit while leaving the others untouched
    for (uint32_t i = 0; i < smd->n_ancilla_qubits; i++)
    {
        circuit* c = smd->sub_circuits[i];
        circuit_element* ce = c->start;
        uint32_t active_ancilla = i + smd->n_code_qubits;

        double* expanded_error_probs = syndrome_error_probs;


        // Create our progress bar if applicable
        #ifdef CIRCUIT_SYNDROME_MEASUREMENT_FLAG_FT_CIRCUIT_PRINT_PROGRESS 
            char progress_bar_name[32]; // Holding our string
            sprintf(progress_bar_name, "Sub-Circuit %d", i);
            progress_bar* ce_progress_bar = progress_bar_create(c->n_gates, progress_bar_name);
        #endif

        double* flagged_error_probs = error_probabilities_zeros(smd->n_code_qubits);

        uint8_t loc_error = 0;
        // Apply the operations on the measurement sub circuit
        int stopgap = 0;

        while (NULL != ce)
        {

            // if (stopgap <= 3)
            // {
            //     ce = ce->next;
            //     stopgap++;
            //     break;
            // } 

            // Gate operation
            double* tmp_error_rate = gate_apply(
                c->n_qubits,
                expanded_error_probs,
                ce->gate_operation,
                ce->target_qubits);

            error_probabilities_free(expanded_error_probs);
            expanded_error_probs = tmp_error_rate;

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
                            error_probabilities_free(expanded_error_probs);
                            expanded_error_probs = tmp_error_rate;
                        }
                    }
                }


            // Artificial Error
            // if (loc_error == glob_location && block_location == i)
            // {
            //     printf("Applying Error!\n");
            //     // Gate operation
            //     double* tmp_error_rate = gate_apply(
            //     c->n_qubits,
            //     expanded_error_probs,
            //     glob_pauli_Z,
            //     glob_target_qubits);

            //     error_probabilities_free(expanded_error_probs);
            //     expanded_error_probs = tmp_error_rate;
            
            //     glob_target_qubits[0] += 1;
            // } 
            //////////

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
                        error_probabilities_free(expanded_error_probs);
                        expanded_error_probs = tmp_error_rate;
                    }           
                }
            }

            // Next circuit element
            ce = ce->next;
            #ifdef CIRCUIT_SYNDROME_MEASUREMENT_FLAG_FT_CIRCUIT_PRINT_PROGRESS
                progress_bar_update(ce_progress_bar);
            #endif

            loc_error += 1;
        }

        #ifdef CIRCUIT_SYNDROME_MEASUREMENT_FLAG_FT_CIRCUIT_PRINT_PROGRESS
            progress_bar_free(ce_progress_bar);
        #endif

         // Apply FT correction circuit
        if (i != smd->n_ancilla_qubits) // We don't do any flag FT correction on the cleanup sub-circuit
        {
            syndrome_error_probs = circuit_run(smd->flag_recovery_circuits[i], expanded_error_probs, noise);
            free(expanded_error_probs); 
        

    

            
            // Split step-down, determines when a re-measurement occurs
            sym_iter* siter = sym_iter_create_n_qubits(smd->n_code_qubits + smd->n_ancilla_qubits + smd->n_flag_qubits);
            while (sym_iter_next(siter))
            {
                if (syndrome_error_probs[sym_iter_ll_from_state(siter)] > 0)
                {
                
                    // Check if a flag has been raised
                    uint8_t unflagged = 1;
                    for (size_t j = smd->n_code_qubits + smd->n_ancilla_qubits;
                            1 == unflagged && j < smd->n_code_qubits + smd->n_ancilla_qubits + smd->n_flag_qubits;
                            j++)
                    {
                        if (sym_is_not_I(siter->state, 0, j))
                        {
                            unflagged = 0;
                        }
                    }

                    // Flag qubits raised, step down
                    if (0 == unflagged) 
                    {
                            
                            sym* state = sym_copy(siter->state);

                            for (size_t k = smd->n_code_qubits;
                                    k < smd->n_code_qubits + smd->n_ancilla_qubits + smd->n_flag_qubits;
                                    k++)
                            {
                                sym_set_X(state, 0, k, 0);
                                sym_set_Z(state, 0, k, 0);
                            }

                            // Copy for remeasurement
                            remeasurement_probs[sym_to_ll(state)] += syndrome_error_probs[sym_iter_ll_from_state(siter)]; 

                            // Given we've copied this state for remeasurement, we zero the probability here
                            syndrome_error_probs[sym_iter_ll_from_state(siter)] = 0;

                            // Free
                            sym_free(state);
                    }

                }

            }
            sym_iter_free(siter);

        }

        
    }

    // Apply the final cleanup circuit
    double* tmp = circuit_run(smd->sub_circuits[smd->n_ancilla_qubits], syndrome_error_probs, noise);
    free(syndrome_error_probs);
    syndrome_error_probs = tmp;


    ///////////////////////////////////////////////////////////

    // Run the sub-circuits
    // These let us track the "active" ancilla qubit while leaving the others untouched
    for (uint32_t i = 0; i < smd->n_ancilla_qubits; i++)
    {
        //////////


        circuit* c = smd->sub_circuits[i];
        circuit_element* ce = c->start;
        uint32_t active_ancilla = i + smd->n_code_qubits;


        double* expanded_error_probs = remeasurement_probs;


        // Create our progress bar if applicable
        #ifdef CIRCUIT_SYNDROME_MEASUREMENT_FLAG_FT_CIRCUIT_PRINT_PROGRESS 
            char progress_bar_name[32]; // Holding our string
            sprintf(progress_bar_name, "Sub-Circuit - Remeasure %d", i);
            progress_bar* ce_progress_bar = progress_bar_create(c->n_gates, progress_bar_name);
        #endif

        double* flagged_error_probs = error_probabilities_zeros(smd->n_code_qubits);

        uint8_t loc_error = 0;
        // Apply the operations on the measurement sub circuit
        while (NULL != ce)
        {

            // Gate operation
            double* tmp_error_rate = gate_apply(
                c->n_qubits,
                expanded_error_probs,
                ce->gate_operation,
                ce->target_qubits);

            error_probabilities_free(expanded_error_probs);
            expanded_error_probs = tmp_error_rate;

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
                            error_probabilities_free(expanded_error_probs);
                            expanded_error_probs = tmp_error_rate;
                        }
                    }
                }
            }


            // Next circuit element
            ce = ce->next;
            #ifdef CIRCUIT_SYNDROME_MEASUREMENT_FLAG_FT_CIRCUIT_PRINT_PROGRESS
                progress_bar_update(ce_progress_bar);
            #endif

            loc_error += 1;
        }

        #ifdef CIRCUIT_SYNDROME_MEASUREMENT_FLAG_FT_CIRCUIT_PRINT_PROGRESS
            progress_bar_free(ce_progress_bar);
        #endif

        // Apply FT correction circuit
        if (i != smd->n_ancilla_qubits) // We don't do any flag FT correction on the cleanup sub-circuit
        {
            remeasurement_probs = circuit_run(smd->flag_recovery_circuits[i], expanded_error_probs, noise);
            free(expanded_error_probs); 

        }
    
    }

    // Apply the final cleanup circuit
    tmp = circuit_run(smd->sub_circuits[smd->n_ancilla_qubits], remeasurement_probs, noise);
    free(remeasurement_probs);
    remeasurement_probs = tmp;


    // Join up our syndrome error probs and our remeasurement probs
    for (uint32_t i = 0; i < error_probabilities_entries_in_table(smd->n_code_qubits + smd->n_ancilla_qubits + smd->n_flag_qubits); i++)
    {
        syndrome_error_probs[i] += remeasurement_probs[i];
    }

    free(remeasurement_probs);

    tmp = error_probabilities_step_down(syndrome_error_probs, 
        smd->n_code_qubits + smd->n_ancilla_qubits + smd->n_flag_qubits ,
        smd->n_code_qubits + smd->n_ancilla_qubits);
    free(syndrome_error_probs);
    syndrome_error_probs = tmp;
    // Cleanup anything that needs to be de-allocated
   
    return syndrome_error_probs;
}