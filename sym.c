//#include <iostream>
#include "codes.h"
#include "error_models.h"
#include "destabilisers.h"
#include "sym_iter.h"
#include "tailored.h"
#include "decoders.h"
#include "dmatrix.h"
#include "characterise.h"
#include "channels.h"
#include "random_codes.h"

//using Eigen::MatrixXcd;
//using Eigen::VectorXcd;

/*
 IF YOU SEE A MEMORY LEAK of  72,704 bytes in 1 block, it's caused by bloody iostream
*/

int main()
{	
	/*
		CODE
	*/
	// sym* code = code_11_1_5_gottesman();
	// sym* logicals = code_11_1_5_gottesman_logicals();
	random_code_return r = code_random(8, 1, 3); 
	sym* code = r.code;
	sym* logicals = r.logicals;

	sym_print(code);
	sym_print(logicals);
	
	/*
		Error Model
	*/
	// Setup the error model
	double (*error_model)(const sym*, void*) =  error_model_iid;

	// Pass the data to the model
	iid_model_data model_data;
	model_data.p_error = 0.01;
	model_data.n_qubits = code->length / 2;

	/*
		Decoder
	*/
	// Determine the tailored recovery operators
	sym** decoder_data = tailor_decoder(code, logicals, error_model, (void*)&model_data);
	// Pick the decoder
	sym* (*decoder)(const sym* syndrome, void* decoder_data) = decoder_tailored;

	printf("Logical Error Channel\n");
	MatrixXcd lc = channel_logical(code, logicals, error_model, (void*)&model_data, decoder, (void*)&decoder_data);
	std::cout << lc << std::endl;

	/*
		Characterisation
	*/
	// Get the Krauss operators after performing error correction
	double* probabilities = characterise_code(code, logicals, error_model, (void*)&model_data, decoder, (void*)&decoder_data);

	//characterise_save(probabilities, code->length, "krauss_probs.txt");
	characterise_print(probabilities, logicals->length);

	//Free memory
	destabilisers_free(decoder_data, 1 << (code->height));
	sym_free(code);
	sym_free(logicals);
	free(probabilities);
	return 0;	
}


/*

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
*/

	

