#ifndef DECODER_DESTABILISER
#define DECODER_DESTABILISER

#include "decoders.h"
#include "../destabilisers.h"

// Destabiliser Decoder -------------------------------------------------------------------------------------

typedef struct {
	unsigned n_destabilisers;
	sym** destabilisers;
} decoder_params_destabiliser_t;

// DECLARATIONS ----------------------------------------------------------------------------------------

/*
	decoder_create_destabiliser
	Base model constructor for a destabiliser decoder
	Allocates memory for the error model and sets the default destructor
	Returns a pointer to a new error model object on the heap
*/
decoder* decoder_create_destabiliser(const sym* code, const sym* logicals);

/*
	decoder_call_destabiliser
	Determines the correction procedure given a syndrome and a set of destabilisers
	:: decoder* d:: The decoder object 
	:: const sym* syndrome :: The  syndrome passed to the decoder
	Returns the correction suggested by the decoder
*/
sym* decoder_call_destabiliser(void* v_params, const sym* syndrome);

/*
	decoder_free_params_destabiliser
	Destructor a destabiliser decoder
	:: decoder* d :: The decoder object whose parameters are to be freed
	Returns nothing
*/
void decoder_free_params_destabiliser(void* v_decoder);


// FUNCTION DEFINITIONS ----------------------------------------------------------------------------------------
/*
	decoder_create_destabiliser
	Base model constructor for a destabiliser decoder
	Allocates memory for the error model and sets the default destructor
	Returns a pointer to a new error model object on the heap
*/
decoder* decoder_create_destabiliser(const sym* code, const sym* logicals)
{

	decoder* d = decoder_create();
	decoder_params_destabiliser_t* dp = (decoder_params_destabiliser_t*)malloc(sizeof(decoder_params_destabiliser_t));

	dp->destabilisers = destabilisers_generate(code, logicals);
	dp->n_destabilisers = code->height;

	d->params = dp;
	
	// Construct the vtable
	d->call = decoder_call_destabiliser;
	d->param_free = decoder_free_params_destabiliser;

	return d;
}

/*
	decoder_call_destabiliser
	Determines the correction procedure given a syndrome and a set of destabilisers
	:: void* v_params:: The pointer to the params object 
	:: const sym* syndrome :: The  syndrome passed to the decoder
	Returns the correction suggested by the decoder
*/
sym* decoder_call_destabiliser(void* v_params, const sym* syndrome)
{
	decoder_params_destabiliser_t* params = (decoder_params_destabiliser_t*)(v_params);
	
	sym* correction = sym_create(1, params->destabilisers[0]->length);

	for (int i = 0; i < syndrome->height; i++)
	{
		if (sym_get(syndrome, i, 0)) 
		{
			sym_add_in_place(correction, params->destabilisers[i]);
		}
	}
	return correction;
}

/*
	decoder_free_params_destabiliser
	Destructor a destabiliser decoder
	:: void* v_params :: The decoder params object to be freed
	Returns nothing
*/
void decoder_free_params_destabiliser(void* v_params)
{

	decoder_params_destabiliser_t* params = (decoder_params_destabiliser_t*)v_params;

	for (unsigned long long i = 0; i < params->n_destabilisers; i++)
	{
		if (NULL != params->destabilisers[i])
		{
			sym_free(params->destabilisers[i]);
		}
	}
	free(params->destabilisers);
	free(params);
	return;
}

#endif