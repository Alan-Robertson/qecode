#ifndef DECODER_LOGICAL_DESTABILISER
#define DECODER_LOGICAL_DESTABILISER

#include "decoders.h"
#include "../logical.h"

// "Inheriting" from the destabiliser decoder
#include "destabiliser.h"

// DECLARATIONS ----------------------------------------------------------------------------------------

/*
	decoder_create_destabiliser
	Base model constructor for a destabiliser decoder
	Allocates memory for the error model and sets the default destructor
	Returns a pointer to a new error model object on the heap
*/
decoder* decoder_create_destabiliser();


/*
	logicals_as_destabilisers:
	Converts a set of logical operators to the same format as used by the destabilisers
	:: const sym* logicals :: The logical operators in question
	Returns an array of pointers to sym objects that are the destabilisers ordered by stabiliser
	i.e: the ith stabiliser will have a destabiliser located at [i]
*/
sym** logicals_as_destabilisers(const sym* logicals);

// FUNCTION DEFINITIONS ----------------------------------------------------------------------------------------
/*
	decoder_create_destabiliser
	Base model constructor for a destabiliser decoder
	Allocates memory for the error model and sets the default destructor
	Returns a pointer to a new error model object on the heap
*/
decoder* decoder_create_logical_destabiliser(const sym* logicals)
{
	decoder* d = decoder_create();
	decoder_params_destabiliser_t* dp = (decoder_params_destabiliser_t*)malloc(sizeof(decoder_params_destabiliser_t));

	dp->destabilisers = logical_as_destabilisers(logicals);
	dp->n_destabilisers = logicals->length;

	d->params = dp;
	d->call = decoder_call_destabiliser;
	d->param_free  = decoder_free_params_destabiliser;

	return d;
}

#endif