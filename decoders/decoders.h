#ifndef DECODERS
#define DECODERS

#include <string.h>
#include <math.h>
#include "../sym.h"

#include "../destabilisers.h"
#include "../error_models/error_models.h"

//----------------------------------------------------------------------------------------
// The Decoder Base 'class'
//----------------------------------------------------------------------------------------
struct decoder;

typedef sym* (*decoder_call_f)(void*, const sym*);

typedef void (*decoder_param_free_f)(void*);

typedef struct decoder {
	// The decoder parameters
	void* params; // The parameters for that particular decoder

	// V table
	decoder_call_f call; // Called to invoke the decoder on some syndrome
	decoder_param_free_f param_free; // Called to free the decoder parameters object
} decoder;

// DECLARATIONS ----------------------------------------------------------------------------------------

/*
	decoder_create
	Base model constructor for decoders, no arguments,
	Allocates memory for the decoder and sets the default destructor
	Returns a pointer to a new decoder object on the heap
*/
decoder* decoder_create();

/*
	decoder_param_free_default
	Default destructor for decoder parameters
	Use this if none of the parameters have been allocated to heap memory
	Else implement your own method and set decoder->param_free to point to it
	:: decoder* d :: The decoder object whose parameters are to be freed
	Returns nothing
*/
void decoder_param_free_default(void* v_params);

/*
	decoder_call
	Dispatch method to call the decoder
	:: decoder* d:: The decoder object 
	:: const sym* syndrome :: The  syndrome passed to the decoder
	Returns the correction suggested by the decoder
*/
sym* decoder_call(decoder* d, const sym* syndrome);

/*
	decoder_free
	Destructor dispatch method for a decoder, frees the decoder and any associated parameters
	:: decoder* d :: The decoder object to be freed
	Returns nothing
*/
void decoder_free(decoder* d);

// FUNCTION DEFINITIONS ----------------------------------------------------------------------------------------

/*
	decoder_create
	Base model constructor for decoders, no arguments,
	Allocates memory for the error model and sets the default destructor
	Returns a pointer to a new error model object on the heap
*/
decoder* decoder_create()
{
	decoder* d = (decoder*)malloc(sizeof(decoder));
	d->param_free = decoder_param_free_default;
	return d;
};

/*
	decoder_param_free_default
	Default destructor for decoder parameters
	Use this if none of the parameters have been allocated to heap memory
	Else implement your own method and set decoder->param_free to point to it
	:: decoder* d :: The decoder object whose parameters are to be freed
	Returns nothing
*/
void decoder_param_free_default(void* v_params)
{
	free(v_params);
	return;
};

// Dispatch methods ------------------------

/*
	decoder_call
	Dispatch method to call the decoder
	:: decoder* d:: The decoder object 
	:: const sym* syndrome :: The  syndrome passed to the decoder
	Returns the correction suggested by the decoder
*/
sym* decoder_call(decoder* d, sym* syndrome)
{
	return d->call(d->params, syndrome);
}

/*
	decoder_free
	Destructor dispatch method for a decoder, frees the decoder and any associated parameters
	:: decoder* d :: The decoder object to be freed
	Returns nothing
*/
void decoder_free(decoder* d)
{
	d->param_free(d->params);	
	free(d);
	return;
}

#endif