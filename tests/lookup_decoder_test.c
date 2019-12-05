
#include "../sym.h"
#include "../codes/codes.h"
#include "../decoders/tailored.h"
#include "../error_models/iid.h"
#include "../error_models/iid_biased.h"
#include "../error_models/weight_one.h"
#include "../error_models/lookup.h"
#include "../characterise.h"
#include "../misc/progress_bar.h"
#include "../gates/gates.h"

int main()
{   
    double physical_error_rate = 0.0001;

    sym* code = code_five_qubit();
    sym* logicals = code_five_qubit_logicals();

    unsigned n_qubits = code->n_qubits, n_logicals = logicals->length / 2, distance = 3;

    // Initial error probabilities
    double* initial_error_probs = error_probabilities_identity(n_qubits);



    // Setup the error model
    //error_model* noise_model = error_model_create_iid_biased_X(n_qubits, physical_error_rate, bias);
    error_model* noise_model = error_model_create_weight_one(n_qubits, physical_error_rate);

    // Single error gate
    gate* wire_noise = gate_create(n_qubits, NULL, noise_model, NULL);
    uint32_t target_qubits[] = {0,1,2,3,4};

    double* final_error_probs = gate_apply(n_qubits, initial_error_probs, wire_noise, target_qubits);
    sym_iter_print_pauli(n_qubits, final_error_probs);

    error_model* measured_error_model = error_model_create_lookup(n_qubits, final_error_probs);
      



    // Tailor the Decoder
    decoder* tailored_decoder = decoder_create_tailored(code, logicals, measured_error_model);  
    double* probabilities = characterise_code(code, logicals, noise_model, tailored_decoder);


    
    sym_iter_print_pauli(1, probabilities);
    
    // Free allocated objects
    error_model_free(noise_model);
    decoder_free(tailored_decoder);
    free(probabilities);    



    return 0;   
}