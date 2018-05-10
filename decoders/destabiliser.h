#ifndef DECODER_DESTABILISER
#define DECODER_DESTABILISER

// Destabiliser Decoder -------------------------------------------------------------------------------------
typedef struct{
	sym** destabilisers;
} decoder_params_destabilisers_t;

decoder* decoder_create_destabiliser();
sym* decoder_call_destabiliser(void* v_decoder, const sym* syndrome);
sym* decoder_free_destabiliser();

decoder* decoder_create_destabiliser()
{

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

sym* decoder_free_destabiliser()

#endif