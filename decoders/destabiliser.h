#ifndef DECODER_DESTABILISER
#define DECODER_DESTABILISER

#include "decoders.h"

// Destabiliser Decoder -------------------------------------------------------------------------------------
typedef struct{
	unsigned n_syndrome_bits;
	sym** destabilisers;
} decoder_params_destabilisers_t;

decoder* decoder_create_destabiliser();
sym* decoder_call_destabiliser(void* v_decoder, const sym* syndrome);
sym* decoder_free_destabiliser(void* v_decoder);

decoder* decoder_create_destabiliser(sym** destabilisers, const unsigned n_syndrome_bits)
{

	decoder* d = decoder_create();
	decoder_params_destabilisers_t* dp = (decoder_params_destabilisers_t*)malloc(sizeof(decoder_params_destabilisers_t));

	dp->n_syndrome_bits = n_syndrome_bits;
	dp->destabilisers = (sym*)malloc(sizeof(sym*) * (1ull << n_syndrome_bits));

	for (unsigned long long i = 0; i < (1ull << (dp->n_syndrome_bits)); i++)
	{
		if (NULL != >destabilisers[i])
		{
			decoder_params->destabilisers[i] = sym_copy(destabilisers[i]);
		}
	}
	return d;
}

sym* decoder_call_destabiliser(void* v_decoder, const sym* syndrome)
{
	decoder_params_destabilisers_t* d = (decoder_params_destabiliser_t*)(((decoder*)v_decoder)->decoder_params);
	
	sym* correction = sym_create(1, d->destabilisers[0]->length);

	for (int i = 0; i < syndrome->height; i++)
	{
		if (sym_get(syndrome, i, 0)) 
		{
			sym_add_in_place(correction, d->destabilisers[i]);
		}
	}
	return correction;
}

void decoder_free_params_destabiliser(void* v_decoder)
{
	decoder* d = v_decoder;
	decoder_params_destabilisers_t* decoder_params = (decoder_params_destabilisers_t*)v_decoder->decoder_params;

	for (unsigned long long i = 0; i < (1ull << (decoder_params->n_syndrome_bits)); i++)
	{
		if (NULL != decoder_params->destabilisers[i])
		{
			sym_free(decoder_params->destabilisers[i]);
		}
	}

	free(decoder_params);

	return;
}

#endif