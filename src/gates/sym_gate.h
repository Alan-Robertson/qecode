#ifndef GATE_SYM
#define GATE_SYM

#include "../sym.h"

typedef gate_result
{
	sym** gate_results;
	uint32_t n_results;
	double* prob_results;
}

#endif