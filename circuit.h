#ifndef CIRCUIT_STRUCT
#define CIRCUIT_STRUCT
#include "sym.h" 
#include "gates.h"
#include <stdarg.h>

// STRUCT OBJECTS ----------------------------------------------------------------------------------------

/*
	circuit_element:
	An individual element in the circuit list
	:: gate* gate_element :: The gate being applied
	:: unsigned* target_qubits :: The qubits this gate is to be applied to
	:: struct circuit_element* next :: The next circuit element
*/
struct circuit_element
{
	gate* gate_element;
	unsigned* target_qubits;
	struct circuit_element* next;
};

/*
	circuit:
	A list of circuit elements
	:: unsigned n_gates :: The number of gates being applied
	:: struct circuit_element* start :: The first element in the list
	:: struct circuit_element* end :: The last element in the list
*/
typedef struct
{
	unsigned n_qubits;
	unsigned n_gates;
	circuit_element* start;
	circuit_element* end;
} circuit;

// FUNCTION DECLARATIONS ----------------------------------------------------------------------------------------
/* 
    circuit_create:
	Creates a new circuit object
	Returns a heap pointer to the new circuit
*/
circuit* circuit_create(unsigned n_qubits);

/* 
    circuit_add_non_varg:
	Adds a gate to an existing circuit
	:: circuit* c :: The circuit the gate is being added to
	:: gate* g :: The gate being added
	:: unsigned* target_qubits :: The qubits the gate is to be applied to
	Does not return anything
*/
void circuit_add_non_varg(circuit* c, gate* g, unsigned* target_qubits);

/* 
    circuit_add:
	Adds a gate to an existing circuit uses vargs for ease of readability
	:: circuit* c :: The circuit the gate is being added to
	:: gate* g :: The gate being added
	:: ... :: Variadic target qubits
	Does not return anything
*/
void circuit_add_gate(circuit* c, gate* g, ...);

/* 
    circuit_run:
	Applies a circuit to an existing set of error probabilities
	:: double* initial_error_rates :: The error rates before the circuit is applied
	:: circuit* c :: The circuit the gate is being added to
	:: const unsigned n_qubits :: The number of qubits
	Returns a heap pointer to the new set of error rates
*/
double* circuit_run(circuit* c, double* initial_error_rates);

/*
	circuit_free:
	Frees a quantum circuit object
	:: circuit* c :: Pointer to the circuit to be freed
	No return
*/
void circuit_free(circuit* c);

// FUNCTION DEFINITIONS ----------------------------------------------------------------------------------------
/* 
    circuit_create:
	Creates a new circuit object
	Returns a heap pointer to the new circuit
*/
circuit* circuit_create(const unsigned n_qubits)
{
	circuit* c = (circuit*)malloc(sizeof(circuit));
	c->n_qubits = n_qubits;
	c->n_gates = 0;
	c->start = NULL;
	c->end = NULL;
}


/* 
    circuit_add_non_varg:
	Adds a gate to an existing circuit
	:: circuit* c :: The circuit the gate is being added to
	:: gate* g :: The gate being added
	:: unsigned* target_qubits :: The qubits the gate is to be applied to
	Does not return anything
*/
void circuit_add_non_varg(circuit* c, gate* g, unsigned* target_qubits)
{
	circuit_element* ce = (circuit_element*)malloc(sizeof(circuit_element));
	ce->gate_element = g;
	ce->target_qubits = (unsigned*)malloc(sizeof(unsigned) * g->n_qubits);
	memcpy(ce->target_qubits, target_qubits, g->n_qubits * sizeof(unsigned));
	ce->next = NULL;
	
	if (NULL != c->start)
	{
		c->end->next = ce;
		c->end = ce;
	}
	else
	{
		c->start = ce;
		c->end = ce;
	}
	c->n_gates++;
	return;
}

/* 
    circuit_add_gate:
	Adds a gate to an existing circuit uses vargs for ease of readability
	:: circuit* c :: The circuit the gate is being added to
	:: gate* g :: The gate being added
	:: ... :: Variadic target qubits
	Does not return anything
*/
void circuit_add_gate(circuit* c, gate* g, ...)
{
	va_list args;
	va_start(args, g);

	unsigned* target_qubits = (unsigned*)malloc(sizeof(unsigned) * g->n_qubits);

	for (unsigned i = 0; i < g->n_qubits; i++)
	{
		target_qubits[i] = va_arg(args, unsigned);
	}
	
	circuit_add_non_varg(c, g, target_qubits);
	free(target_qubits);
	return;
}

/* 
    circuit_run:
	Applies a circuit to an existing set of error probabilities
	:: double* initial_error_rates :: The error rates before the circuit is applied
	:: circuit* c :: The circuit the gate is being added to
	:: const unsigned n_qubits :: The number of qubits
	Returns a heap pointer to the new set of error rates
*/
double* circuit_run(circuit* c, double* initial_error_rates)
{
	const unsigned n_bytes = sizeof(double) * (1 << (2 * c->n_qubits));

	double* error_rate = (double*)calloc(1 << (2 * c->n_qubits), sizeof(double));
	memcpy(error_rate, initial_error_rates, n_bytes);

	circuit_element* ce = c->start;
	while(ce != NULL)
	{
		double* tmp_error_rate = gate_apply(c->n_qubits, error_rate, ce->gate_element, ce->target_qubits);
		memcpy(error_rate, tmp_error_rate, n_bytes);
		free(tmp_error_rate);
		ce = ce->next;
	}

	return error_rate;
}

/*
	circuit_free:
	Frees a quantum circuit object
	:: circuit* c :: Pointer to the circuit to be freed
	No return
*/
void circuit_free(circuit* c)
{
	circuit_element* ce = c->start;
	while(ce != NULL)
	{
		circuit_element* ce_next = ce->next;
		free(ce->target_qubits);
		free(ce);
		ce = ce_next;
	}
	free(c);
}

#endif