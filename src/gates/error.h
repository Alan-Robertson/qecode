#ifndef GATE_ERROR
#define GATE_ERROR

#include "gates.h"
#include "../error_models/error_models.h"

/*
 * gate* gate_create_error
 * This gate applies an error model over some number of qubits
 * This differs from the IID gate that applies a single error model to each qubit individually
 * :: const uint32_t n_qubits :: The number of qubits this gate acts on
 * :: error_model* em :: The error model to apply
 * Returns a new gate object
 */
gate* gate_create_error(
	const uint32_t n_qubits,
	error_model* em);

/*
 * gate* gate_create_error_wdata
 * This gate applies an error model over some number of qubits
 * This differs from the IID gate that applies a single error model to each qubit individually
 * This version of the constructor also passes any additional objects that might be required by the error model
 * :: const uint32_t n_qubits :: The number of qubits this gate acts on
 * :: error_model* em :: The error model to apply
 * :: void* error_data :: A pointer to the data to be passed to the error model
 * Returns a new gate object
 */
gate* gate_create_error_wdata(
	const uint32_t n_qubits, 
	error_model* em, 
	void* error_data);

/*
 * gate* gate_create_error
 * This gate applies an error model over some number of qubits
 * This differs from the IID gate that applies a single error model to each qubit individually
 * :: const uint32_t n_qubits :: The number of qubits this gate acts on
 * :: error_model* em :: The error model to apply
 * Returns a new gate object
 */
gate* gate_create_error(
	const uint32_t n_qubits,
	error_model* em)
{
	return gate_create(n_qubits, NULL, em, NULL);
}

/*
 * gate* gate_create_error_wdata
 * This gate applies an error model over some number of qubits
 * This differs from the IID gate that applies a single error model to each qubit individually
 * This version of the constructor also passes any additional objects that might be required by the error model
 * :: const uint32_t n_qubits :: The number of qubits this gate acts on
 * :: error_model* em :: The error model to apply
 * :: void* error_data :: A pointer to the data to be passed to the error model
 * Returns a new gate object
 */
gate* gate_create_error_wdata(
	const uint32_t n_qubits, 
	error_model* em, 
	void* error_data)
{
	return gate_create(n_qubits, NULL, em, error_data);
}

#endif