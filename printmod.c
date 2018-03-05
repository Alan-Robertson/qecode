#include <Python.h>
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


static PyObject* sym_system(PyObject *self, PyObject *args)
{
	const char* command;
	int sts;

	if (!PyArg_ParseTuple(args, "s", &command))
		return NULL;
	sts = system(command);
	return PyLong_FromLong(sts);
}

int for_python()
{	
	double 


	for (double a_error_rate = 0.2; a_error_rate > 0; a_error_rate-=0.01)
	{
		for (double b_error_rate = 0.2; b_error_rate > 0; b_error_rate -=0.01)
		{

		/* 
			Codes
		*/
		sym* code = code_five_qubit();
		sym* logicals = code_five_qubit_logicals();

		unsigned n_qubits = 5, n_logicals = 1, distance = 3;
		unsigned n_codes_searched = 1000;

		/*
			Error Model
		*/
		// Setup the error model
		const unsigned n_models = 2;
		error_model_f error_model = error_model_multi_composition;

		iid_model_data bf_a;
		bf_a.n_qubits = 3;
		bf_a.p_error = a_error_rate;
		
		iid_model_data bf_b;
		bf_b.n_qubits = 2;
		bf_b.p_error = b_error_rate;
		

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
		//sym_print(random_code);
		//sym_print(random_logicals);
		//printf("5 Qubit Perfect Code\n");
		//characterise_print(probabilities, logicals->length);

		//printf("5 Qubit Random Code\n");
		//characterise_print(probabilities_r, logicals->length);	
		char file_name[50];
		sprintf(file_name, "data/5_perf_a:%e_b:%e.txt", a_error_rate, b_error_rate);
		characterise_save(probabilities, logicals->length, file_name);

		sprintf(file_name, "data/5_rand_a:%e_b:%e.txt", a_error_rate, b_error_rate);
		characterise_save(probabilities_r, logicals->length, file_name);
		//printf("Logical Error Channel\n");	
		//std::cout << lc << std::endl;

		// Free allocated objects
		error_model_multi_free(&model_data);
		free(probabilities);
		destabilisers_free(decoder_data, 1 << (code->height));
		sym_free(code);
		sym_free(logicals);
		}
	}
	return 0;	
}