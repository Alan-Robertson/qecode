#ifndef CIRCUIT_DECODING
#define CIRCUIT_DECODING

#include "circuit.h"
#include "unitary.h"

/*
 * decoding_circuit_from_encoding
 * Given the encoding circuit, reverses it to get the decoding circuit
 * :: circuit* encode :: The circuit to create the unitary from 
 * Returns the decoding circuit
 */
circuit* decoding_circuit_from_encoding(circuit* encode);

/*
 * decoding_circuit_from_encoding
 * Given the encoding circuit, reverses it to get the decoding circuit
 * :: circuit* encode :: The circuit to create the unitary from 
 * Returns the decoding circuit
 */
circuit* decoding_circuit_from_encoding(circuit* encode)
{
	return create_unitary(encode);
}


#endif