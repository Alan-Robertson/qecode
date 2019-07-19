#ifndef CIRCUIT_UNITARY
#define CIRCUIT_UNITARY

/*
 * create_unitary
 * If the circuit passed is composed only of cliffords then this will create a new circuit that is reversed in order,
 * Applying both circuits will result in an overall unitary operations (sans noise)
 * :: circuit* c :: The circuit to create the unitary from 
 * Returns a new circuit where the gate order has been reversed
 */
circuit* create_unitary(circuit* c);


/*
 * create_unitary
 * If the circuit passed is composed only of cliffords then this will create a new circuit that is reversed in order,
 * Applying both circuits will result in an overall unitary operations (sans noise)
 * :: circuit* c :: The circuit to create the unitary from 
 * Returns a new circuit where the gate order has been reversed
 */
circuit* create_unitary(circuit* c)
{
	// Create the new circuit
	circuit* unitary = circuit_create(c->n_qubits);

	// Copy the other circuit, creating a new one where the gates are applied in reverse order
	circuit_element* citer = c->start;
	while (NULL != citer)
	{
		circuit_add_non_varg_start(unitary, citer->gate_operation, citer->target_qubits);
		citer = citer->next;
	}

	// Return the new gate
	return unitary;
}

#endif