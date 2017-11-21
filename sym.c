#include "sym.h"
#include "codes.h"
#include "error_models.h"
#include "characterise.h"
#include "decoders.h"
#include "destabilisers.h"
//#include "dmatrix.h"
//using Eigen::MatrixXcd;
//using Eigen::VectorXcd;


typedef struct {
	sym* state; // The current state of our sym iterator
	unsigned order_tracker;
	unsigned* shift_trackers; //Initially all -1's 
} sym_iter; 


/*
	The trackers take the form: 


*/

sym_iter* sym_iter_create(unsigned length)
{
	// Allocate memory for the state iterator
	sym_iter* siter = (sym_iter*)malloc(sizeof(sym_iter));
	
	// Create the state currently occupied by the state iterator
	siter->state = sym_create(0, length);

	// Set the order trackers
	siter->order_tracker = 0;

	// Create the trackers to update the state
	siter->shift_trackers = (unsigned*)malloc(sizeof(unsigned) * n);
	// set the initial values of the state parameters
	for (int i = 0; i < siter->state->length; i++)
	{
		if (i == 0)
		{
			siter->shift_trackers[i] = 0 ;
		}
		else
		{
			siter->shift_trackers[i] = -1;
		}
	}
}


// Update the state of the iterator
void sym_iter_next(sym_iter* siter)
{
	BYTE final_bytes[siter->state->mem_size];
	for (int i = 0; i <= siter->order_tracker; i++)
	{
		BYTE tmp_arr[siter->state->mem_size];
		if (siter->shift_trackers[i] != 0)
		{
				sym_iter_array_left_shift(tmp_arr, siter->state->mem_size, siter->shift_trackers[i]);		
		}
	}

}

void sym_iter_array_left_shift(BYTE* arr, unsigned size, unsigned shift)
{
	BYTE cross = 0;
	while(shift--)
	{
		for (int i = size - 1; i >= 0; --i)
		{
			// If the highest bit is set then save it for carrying
			BYTE next = (arr[i] & 0xFF) ? 1 : 0;
			// And OR it in with the next object
			arr[i] = (arr[i] << 1) | cross;
			cross = next; 
		}
	}
}

void sym_iter_array_wise_xor(BYTE* arr_a, BYTE* arr_b, unsigned size)
{
	for (int i = 0; i < size; i++)
	{
		arr_a[i] ^= arr_b[i];
	}
}


int main()
{
	int n = 4;
	unsigned long buff = 1;
	for (unsigned r = 0, k = 0; 
		r <= n; 
		((r + k < n) ? k++ : k = 0), ((k != 0) ? r : r++)) 
	{
		shiftpow(r, k, &buff);
		printf("%d %d %lu\n", r, k, buff);
	}
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
