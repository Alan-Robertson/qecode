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
		Code
	*/
	sym* code = code_five_qubit();
	sym* logicals = code_five_qubit_logicals();

	/*
		Error Model
	*/
	// Setup the error model
	error_model_f error_model = error_model_multi_composition;

	multi_composition_error_model_data model_data;
	model_data.n_models = 2;
	model_data.model_split = (unsigned*)malloc(sizeof(unsigned) * model_data.n_models);
	

	// Setup the model data
	
	model_data.n_qubits = code->length/2;
	model_data.p_error = 0.1;


	/* 
		Tailor the Decoder
	*/
	decoder_f decoder = decoder_tailored;
	sym** decoder_data = tailor_decoder(code, logicals, error_model, (void*)&model_data);

	// Get the Probabilities of each logical error occurring
	double* probabilities = characterise_code(code, logicals, error_model, (void*)&model_data, decoder, (void*)&decoder_data);	

	/*
		Display and Cleanup
	*/
	characterise_print(probabilities, logicals->length);	

	// Free allocated objects
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
