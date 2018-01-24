//#include <iostream>
#include "codes.h"
#include "error_models.h"
#include "destabilisers.h"
#include "sym_iter.h"
#include "tailored.h"
#include "decoders.h"
#include "dmatrix.h"
#include "characterise.h"
#include "random_code_search.h"
#include "channel.h"

//using Eigen::MatrixXcd;
//using Eigen::VectorXcd;

/*
 IF YOU SEE A RECOVERABLE MEMORY LEAK of 72,704 bytes in 1 block, it's caused by iostream
*/

int main()
{	
	/* 
		Codes
	*/
	sym* code = code_five_qubit();
	sym* logicals = code_five_qubit_logicals();

	unsigned n_qubits = 5, n_logicals = 1, distance = 3;
	unsigned n_codes_searched = 10000;

	/*
		Error Model
	*/
	// Setup the error model
	const unsigned n_models = 2;
	error_model_f error_model = error_model_multi_composition;

	iid_model_data bf_a;
	bf_a.n_qubits = 3;
	bf_a.p_error = 0.1;
	
	bit_flip_model_data bf_b;
	bf_b.n_qubits = 2;
	bf_b.p_error = 0.02;
	

	//multi_composition_error_model_data* model_data = error_model_multi_builder(
	//	n_models, iid_a.n_qubits, iid_b.n_qubits, error_model_iid, error_model_iid, (void*)&iid_a, (void*)&iid_b);
	multi_composition_error_model_data model_data = error_model_multi_builder(
		n_models, bf_a.n_qubits, bf_b.n_qubits,
		error_model_iid, error_model_bit_flip,
		(void*)&bf_a, (void*)&bf_b);

	/* 
		Tailor the Decoder
	*/
	decoder_f decoder = decoder_tailored;
	sym** decoder_data = tailor_decoder(code, logicals, error_model, (void*)&model_data);
	
	// Get the Probabilities of each logical error occurring
	double* probabilities = characterise_code(code, logicals, error_model, (void*)&model_data, decoder, (void*)&decoder_data);	

	//MatrixXcd lc = channel_logical(code, logicals, error_model, (void*)&model_data, decoder, (void*)&decoder_data);
	
	/*
		Random Code
	*/

	struct random_search_results r = random_code_search_best_of_n_codes_with_stats(
		n_qubits, 
		n_logicals,
		distance,
		error_model,
		(void*)&model_data,
		n_codes_searched); 

	sym* random_code = r.code;
	sym* random_logicals = r.logicals;

	sym** random_decoder_data = tailor_decoder(random_code, random_logicals, error_model, (void*)&model_data);

	double* probabilities_r = characterise_code(random_code, random_logicals, error_model, (void*)&model_data, decoder, (void*)&random_decoder_data);	

	/*
		Display and Cleanup
	*/

	sym_print(random_code);
	sym_print(random_logicals);
	printf("5 Qubit Perfect Code\n");
	characterise_print(probabilities, logicals->length);

	printf("5 Qubit Random Code\n");
	characterise_print(probabilities_r, logicals->length);	

	//printf("Logical Error Channel\n");	
	//std::cout << lc << std::endl;

	// Free allocated objects
	error_model_multi_free(&model_data);
	free(probabilities);
	destabilisers_free(decoder_data, 1 << (code->height));
	sym_free(code);
	sym_free(logicals);
	return 0;	
}


/*
	In progress
	// Decoder
	
	// Determine the tailored recovery operators
	sym** decoder_data = tailor_decoder(code, logicals, error_model, (void*)&model_data);
	// Pick the decoder
	sym* (*decoder)(const sym* syndrome, void* decoder_data) = decoder_tailored;

	printf("Logical Error Channel\n");
	MatrixXcd lc = channel_logical(code, logicals, error_model, (void*)&model_data, decoder, (void*)&decoder_data);
	std::cout << lc << std::endl;
	
		// Characterisation
	
	// Get the Krauss operators after performing error correction
	double* probabilities = characterise_code(code, logicals, error_model, (void*)&model_data, decoder, (void*)&decoder_data);

	//characterise_save(probabilities, code->length, "krauss_probs.txt");
	characterise_print(probabilities, logicals->length);

	//Free memory
	destabilisers_free(decoder_data, 1 << (code->height));
	sym_free(code);
	sym_free(logicals);
	free(probabilities);

	// Determine the logical error channel
	//printf("Logical Error Channel\n");
	//MatrixXcd lc = logical_error_channel(code, logicals, error_model, (void*)&model_data, decoder, (void*)&decoder_data);
	//std::cout << lc << std::endl;
	
	//printf("Physical Error Channel\n");
	//MatrixXcd pc = physical_error_channel(code, logicals, error_model, (void*)&model_data, decoder, (void*)&decoder_data);
	//std::cout << pc << std::endl;
	
	//printf("Logical Closure\n");
	//MatrixXcd cl = logical_closure(code, logicals, error_model, (void*)&model_data, decoder, (void*)&decoder_data);
	//std::cout << cl << std::endl;

	// Pass the data to the model
	/*spatially_asymmetric_model_data model_data;
	model_data.n_bitflip_qubits = 4;
	model_data.n_phaseflip_qubits = 4;
	model_data.p_bitflip = 0.01;
	model_data.p_phaseflip = 0.05;
*/
