#ifndef CIRCUIT_DECODING
#define CIRCUIT_DECODING

#include "circuit.h"
#include "unitary.h"

/*
 * decoding_circuit
 * Given the encoding circuit, reverses it to get the decoding circuit
 * :: circuit* encode :: The circuit to create the unitary from 
 * Returns the decoding circuit
 */
circuit* decoding_circuit(circuit* encode);

/*
 * decoding_circuit
 * Given the encoding circuit, reverses it to get the decoding circuit
 * :: circuit* encode :: The circuit to create the unitary from 
 * Returns the decoding circuit
 */
circuit* decoding_circuit(circuit* encode)
{
	return create_unitary(encode);
}


#endif