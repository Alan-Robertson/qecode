#ifndef STATEREP
#define STATEREP

#include "dmatrix.h"
#include "sym.h"
#include "sym_iter.h"
#include "errors.h"
#include "decoders.h"
#include "logical.h"
#include <iostream>
#include <math.h>
using Eigen::MatrixXcd;
using Eigen::VectorXcd;


MatrixXcd channel_logical(const sym* code, 
						const sym* logicals, 
						double (*error_model)(const sym*, void*), 
						void* model_data, 
						sym* (*decoder)(const sym*, void*),
						void* decoder_data)
{
	const unsigned krauss_size = (1 << (logicals->length/2));
	const unsigned channel_size = (1 << (logicals->length/2)) * (1 << (logicals->length/2));
	
	MatrixXcd sum_logical_operator = dmatrix_zeros(krauss_size, krauss_size);
	MatrixXcd channel = dmatrix_zeros(channel_size, channel_size);


	sym_iter* physical_error = sym_iter_create(code->length);	
	while (sym_iter_next(physical_error)) {
		// Calculate the probability of the error occurring
		double error_prob = error_model(physical_error->state, model_data);

		// What syndrome is caused by this error
		sym* syndrome = sym_syndrome(code, physical_error->state);

		// Use the decoder to determine the recovery operator
		sym* recovery = decoder(syndrome, decoder_data);

		//  Determine the overall impact of the correction
		sym* corrected = sym_add(recovery, physical_error->state);

		// Find the logical operations associated with the corrected state
		sym* logical_state = logical_error(corrected, logicals);

		// Get the density matrix representation of the logical state
		MatrixXcd logical_operator = dmatrix_sym_to_matrix(logical_state);

		// Add this particular matrix with the appropriate weighting to the sum
		sum_logical_operator += sqrt(error_prob) * logical_operator;
		
		sym_free(syndrome);
		sym_free(recovery);
		sym_free(corrected);
		sym_free(logical_state);
	}
	sym_iter_free(physical_error);

	// Vectorise the Krauss operators to get the channel
	channel = dmatrix_vectorise(sum_logical_operator);

	return channel;
}

MatrixXcd channel_physical(const sym* code, 
						const sym* logicals, 
						double (*error_model)(const sym*, void*), 
						void* model_data, 
						sym* (*decoder)(const sym*, void*),
						void* decoder_data)
{
	const unsigned krauss_size = (1 << (code->length/2));
	const unsigned channel_size = (1 << (code->length/2)) * (1 << (code->length/2));
	
	MatrixXcd sum_physical_operator = dmatrix_zeros(krauss_size, krauss_size);
	MatrixXcd channel = dmatrix_zeros(channel_size, channel_size);

	sym_iter* physical_error = sym_iter_create(code->length);	
	while (sym_iter_next(physical_error)) {
		// Calculate the probability of the error occurring
		double error_prob = error_model(physical_error->state, model_data);
		if (0 != error_prob)
		{
			// What syndrome is caused by this error
			sym* syndrome = sym_syndrome(code, physical_error->state);

			// Use the decoder to determine the recovery operator
			sym* recovery = decoder(syndrome, decoder_data);

			//  Determine the overall impact of the correction
			sym* corrected = sym_add(recovery, physical_error->state);

			// Get the density matrix representation of the logical state
			MatrixXcd physical_operator = dmatrix_sym_to_matrix(corrected);

			// Add this particular matrix with the appropriate weighting to the sum
			sum_physical_operator += sqrt(error_prob) * physical_operator; 

			sym_free(syndrome);
			sym_free(recovery);
			sym_free(corrected);
		}
	}
	sym_iter_free(physical_error);

	// Vectorise the Krauss operators to get the channel
	channel = dmatrix_vectorise(physical_operator);
	
	return channel;
}

MatrixXcd logical_closure(const sym* code, 
						const sym* logicals, 
						double (*error_model)(const sym*, void*), 
						void* model_data, 
						sym* (*decoder)(const sym*, void*),
						void* decoder_data)
{
	MatrixXcd sum_logical_operator = dmatrix_zeros(1 << (logicals->length/2), 1 << (logicals->length/2));

	sym_iter* physical_error = sym_iter_create(code->length);	
	while (sym_iter_next(physical_error)) {
		// Calculate the probability of the error occurring
		double error_prob = error_model(physical_error->state, model_data);

		// What syndrome is caused by this error
		sym* syndrome = sym_syndrome(code, physical_error->state);

		// Use the decoder to determine the recovery operator
		sym* recovery = decoder(syndrome, decoder_data);

		//  Determine the overall impact of the correction
		sym* corrected = sym_add(recovery, physical_error->state);

		// Find the logical operations associated with the corrected state
		sym* logical_state = logical_error(corrected, logicals);

		// Get the density matrix representation of the logical state
		MatrixXcd logical_operator = dmatrix_sym_to_matrix(logical_state);

		// Add this particular matrix with the appropriate weighting to the sum
		sum_logical_operator += error_prob* logical_operator * logical_operator.adjoint();
		
		sym_free(syndrome);
		sym_free(recovery);
		sym_free(corrected);
		sym_free(logical_state);
	}
	sym_iter_free(physical_error);

	return sum_logical_operator;
}

#endif