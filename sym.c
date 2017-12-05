#include "codes.h"
#include "error_models.h"
#include "destabilisers.h"
#include "sym_iter.h"
#include "tailored.h"
//using Eigen::MatrixXcd;
//using Eigen::VectorXcd;

int main()
{
	sym* code = code_8_3_3_gottesman();
	sym* logicals = code_8_3_3_gottesman_logicals();

	// Setup the error model
	double (*error_model)(const sym*, void*) =  error_model_iid;

	// Pass the data to the model
	iid_model_data model_data;
	model_data.p_error = 0.01;
	model_data.n_qubits = 5;
	
	sym** tailored_decoder = decoder_tailor(code, logicals, error_model, (void*)&model_data);
	
	destabilisers_print(tailored_decoder, 1 << (code->height));

	destabilisers_free(tailored_decoder, 1 << (code->height));
	sym_print(logicals);
	sym_free(code);
	sym_free(logicals);
	
	return 0;
}

	
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
for (int i = 0; i < code->height; i++)
	{
		sym_print(destabilisers[i]);
	}

	printf("################\n");
	sym_print(code);

	printf("################\n");
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