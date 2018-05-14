#ifndef DECODERS
#define DECODERS

#include <string.h>
#include <math.h>
#include "../sym.h"

#include "../destabilisers.h"

//----------------------------------------------------------------------------------------
// The Decoder Base 'class'
//----------------------------------------------------------------------------------------

typedef sym* (*decoder_call_f)(void*, const sym*);

typedef void (*decoder_param_free_f)(void*);

typedef struct {
	// The decoder parameters
	void* decoder_params; // The parameters for that particular decoder

	// V table
	decoder_call_f call_decoder; // Called to invoke the decoder on some syndrome
	decoder_param_free_f free_decoder_params; // Called to free the decoder parameters object
} decoder;

// DECLARATIONS ----------------------------------------------------------------------------------------

/*
	decoder_create
	Base model constructor for decoders, no arguments,
	Allocates memory for the error model and sets the default destructor
	Returns a pointer to a new error model object on the heap
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
void decoder_param_free_default(void* v_decoder);

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
	d->
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
void decoder_param_free_default(void* v_decoder)
{
	decoder* d = (decoder*)v_decoder;
	free(d->decoder_params);
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
void decoder_call(decoder* d, sym* syndrome)
{
	return d->call_decoder(d, syndrome);
}

/*
	decoder_free
	Destructor dispatch method for a decoder, frees the decoder and any associated parameters
	:: decoder* d :: The decoder object to be freed
	Returns nothing
*/
void decoder_free(decoder* d)
{
	d->free_decoder_params(d);	
	free(d);
	return;
}

#endif