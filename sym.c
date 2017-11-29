#include "codes.h"
#include "error_models.h"
#include "characterise.h"
#include "decoders.h"
#include "destabilisers.h"
#include "sym_iter.h"
//using Eigen::MatrixXcd;
//using Eigen::VectorXcd;

int main()
{
	sym_iter* iter = sym_iter_create(5);
	while(sym_iter_next(iter))
	{
		sym_print(iter->state);
	}
	sym_iter_free(iter);
	return 0;
}

	/*sym* code = code_steane();
	sym* logicals = code_steane_logicals();
	
	
	sym* destabilisers = destabilisers_low_weight_generate(code, logicals);

	//sym_print(destabilisers);
	//printf("################\n");
	//sym_print(code);

	printf("################\n");
	sym_free(destabilisers);	

	destabilisers = destabilisers_generate(code, logicals);
	sym_print(destabilisers);

	printf("################\n");
	sym_print(code);

	printf("################\n");
	sym_free(destabilisers);	

	//sym_print(logicals);

	sym_free(code);
	sym_free(logicals);*/

/*{
	MatrixXcd pi = dmatrix_pauli_i();
	MatrixXcd px = dmatrix_pauli_x();
	MatrixXcd pxi = kroneckerProduct(px, pi);
	MatrixXcd pix = kroneckerProduct(pi, px);

	MatrixXcd p = dmatrix_pauli_string("XI");

	std::cout <<  << std::endl;

	return 0; 
} */


/*

// Setup the code and logicals
	sym* code = code_asymmetric_five();
	sym* logicals = code_asymmetric_five_logicals();
	
	// Setup the error model
	double (*error_model)(const sym*, void*) =  error_model_iid;

	// Pass the data to the model
	iid_model_data model_data;
	model_data.p_error = 0.01;
	model_data.n_qubits = 5;

	// Pick a decoder
	sym* (*decoder)(const sym*, void*) = decoder_null;
	null_decoder_data decoder_data;
	decoder_data.n_qubits = 5;

	// Characterise the logical Krauss operators
	double* krauss_probs = characterise_code(
		code, logicals, 
		error_model, (void*)&model_data, 
		decoder, (void*)&decoder_data);
	

	//characterise_save(krauss_probs, 2, "Krauss.txt");
	//characterise_save(krauss_probs[1], 1, "logical_krauss.txt");

	sym_free(code);
	sym_free(logicals);
	free(krauss_probs);
	*/