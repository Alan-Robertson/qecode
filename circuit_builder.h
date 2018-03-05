#ifndef CIRCUITBUILDER
#define CIRCUITBUILDER
#include "sym.h" 
#include "gates.h"

struct circuit_element
{
	gate* gate_element;
	unsigned* target_qubits;
	struct circuit_element* next;
};

typedef struct
{
	unsigned n_gates;
	circuit_element* start;
	circuit_element* end;
} circuit;


circuit* circuit_create()
{
	circuit* c = (circuit*)malloc(sizeof(circuit));
	c->n_gates = 0;
	c->start = NULL;
	c->end = NULL;
}

void circuit_add_gate(circuit* c, gate* g, unsigned* target_qubits)
{
	circuit_element* ce = (circuit_element*)malloc(sizeof(circuit_element));
	ce->gate_element = g;
	ce->target_qubits = (unsigned*)malloc(sizeof(unsigned) * g->n_qubits);
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

double* circuit_run(double* initial_error_rates, circuit* c, const unsigned n_qubits)
{
	const unsigned n_bytes = sizeof(double) * (1 << (2 * n));

	double* error_rate = (double*)calloc(1 << (2 * n_qubits), sizeof(double));
	memcpy(error_rate, initial_error_rates, n_bytes);

	circuit_element* ce = c->start;
	while(ce != NULL)
	{

		double* tmp_error_rate = gate_apply_noisy(n_qubits, error_rate, ce->gate_element, ce->target_qubits);
		memcpy(error_rate, tmp_error_rate, n_bytes);
		free(tmp_error_rate);
	}
	
	return error_rate;
}


void circuit_delete(circuit* c)
{
	circuit_element* ce = c->start;
	while(ce != NULL)
	{
		ce_next = ce->next;
		free(ce->target_qubits);
		free(ce);
		ce = ce_next;
	}
	free(c);
}

#endif