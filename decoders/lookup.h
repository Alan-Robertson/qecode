#ifndef DECODER_LOOKUP
#define DECODER_LOOKUP

#include "decoders.h"
#include "destabiliser.h"
#include "logical_destabiliser.h"

//----------------------------------------------------------------------------------------
// Function Declarations
//----------------------------------------------------------------------------------------

/*
	decoder_create_lookup
	Base model constructor for a lookup based decoder, allocates memory and sets the vtable up appropriately.
	:: const uint32_t n_bits :: The number of syndrome bits to be passed to this decoder
	Returns a pointer to a new decoder object on the heap
*/
decoder* decoder_create_lookup();

/*
	decoder_call_lookup
	Determines the correction procedure given a syndrome and a lookup decoder
	:: void* v_params:: The pointer to the params object 
	:: const sym* syndrome :: The  syndrome passed to the decoder
	Returns the correction suggested by the decoder
*/
sym* decoder_call_lookup(void* v_params, const sym* syndrome);

/*
	decoder_free_params_lookup
	Destructor for a lookup decoder
	:: void* v_params :: The decoder params object to be freed
	Returns nothing
*/
void decoder_free_params_lookup(void* v_params);

/*
	decoder_lookup_insert
	Inserts the given sym object at the given syndrome
	:: void* v_params :: The decoder params
	:: const sym* syndrome :: The syndrome used for the decoder table
	:: const sym* value :: The sym state used for that particular value in the table
	Returns nothing, operation occurs in place
*/
void decoder_lookup_insert(decoder* d, const sym* syndrome, const sym* value);

/*
	decoder_lookup_print
	Prints the contents of the lookup decoder
	:: decoder* d :: The decoder
	Returns nothing, prints to stdout
*/
void decoder_lookup_print(decoder* d);


//----------------------------------------------------------------------------------------
// The Lookup Decoder `Class'
//----------------------------------------------------------------------------------------

typedef struct {
	unsigned n_syndrome_bits;
	sym** recovery_operators;
} decoder_params_lookup_t;


//----------------------------------------------------------------------------------------
// Function Definitions
//----------------------------------------------------------------------------------------

/*
	decoder_create_lookup
	Base model constructor for a lookup based decoder, allocates memory and sets the vtable up appropriately.
	:: const uint32_t n_bits :: The number of syndrome bits to be passed to this decoder
	Returns a pointer to a new decoder object on the heap
*/
decoder* decoder_create_lookup(const uint32_t n_bits)
{
	decoder* d = decoder_create();
	decoder_params_lookup_t* dp = (decoder_params_lookup_t*)malloc(sizeof(decoder_params_tailored_t));

	dp->recovery_operators = (sym**)calloc(decoder_entries_in_table(n_bits), sizeof(sym*));
	dp->n_syndrome_bits = n_bits;

	// Link the params to the decoder
	d->params = dp;

	// Setup the vtable
	d->call = decoder_call_lookup;
	d->param_free = decoder_free_params_lookup;
	return d;
}

/*
	decoder_call_lookup
	Determines the correction procedure given a syndrome and a lookup decoder
	:: void* v_params:: The pointer to the params object 
	:: const sym* syndrome :: The  syndrome passed to the decoder
	Returns the correction suggested by the decoder, if nothing is there, returns NULL
*/
sym* decoder_call_lookup(void* v_params, const sym* syndrome)
{
	decoder_params_tailored_t* params = (decoder_params_tailored_t*)v_params;
	sym* recovery_operator = NULL;
	if (params->recovery_operators[sym_to_ll(syndrome)] != NULL)
	{
		recovery_operator = sym_copy(params->recovery_operators[sym_to_ll(syndrome)]);
	}
	return recovery_operator;
}

/*
	decoder_free_params_lookup
	Destructor for a lookup decoder
	:: void* v_params :: The decoder params object to be freed
	Returns nothing
*/
void decoder_free_params_lookup(void* v_params)
{
	decoder_params_tailored_t* params = (decoder_params_tailored_t*)v_params;

	for (unsigned long long i = 0; i < (1ull << (params->n_syndrome_bits)); i++)
	{
		
		if (NULL != params->recovery_operators[i])
		{
			sym_free(params->recovery_operators[i]);
		}
	}
	free(params->recovery_operators);
	free(params);
	return;
}

/*
	decoder_lookup_insert
	Inserts the given sym object at the given syndrome
	:: const sym* syndrome :: The syndrome used for the decoder table
	:: const sym* value :: The sym state used for that particular value in the table
	Returns nothing, operation occurs in place
*/
void decoder_lookup_insert(decoder* d, const sym* syndrome, const sym* value)
{
	decoder_params_tailored_t* params = (decoder_params_tailored_t*)d->params;
	uint64_t target_index = sym_to_ll(syndrome);

	// If we already have a recovery operation stored at that index, remove it
	if (NULL != params->recovery_operators[target_index])
	{
		sym_free(params->recovery_operators[target_index]);
	}

	// Copy the new value in
	params->recovery_operators[target_index] = sym_copy(value);
	return;
}


/*
	decoder_lookup_print
	Prints the contents of the lookup decoder
	:: decoder* d :: The decoder
	Returns nothing, prints to stdout
*/
void decoder_lookup_print(decoder* d)
{
	sym_iter* siter = sym_iter_create(((decoder_params_tailored_t*)(d->params))->n_syndrome_bits);
	while (sym_iter_next(siter))
	{
		sym* recovery_operator = decoder_call_lookup(d->params, siter->state);

		sym_print(siter->state);
		sym_print_pauli(recovery_operator);

		sym_free(recovery_operator);
	}
	sym_iter_free(siter);
}


#endif