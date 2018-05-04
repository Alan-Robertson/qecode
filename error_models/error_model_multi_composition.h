// Multi Model Composition -------------------------------------------------------------------------------
 
struct error_model_params_multi_composition {
	unsigned n_models;
	unsigned* model_split;
	error_model_f* error_models;
	void** error_models_params;
};

error_model* error_model_create_multi_composition(unsigned n_models, ...);
double error_model_call_multi_composition(const sym* error, void* v_model_params);
void error_model_free_multi_composition(void* v_model_params);

error_model* error_model_create_multi_composition(unsigned n_models, ...)
{
	error_model* m = error_model_create();
	
	// The parameters
	struct multi_composition_error_model_params* model_params = (struct multi_composition_error_model_params*)malloc(sizeof(struct multi_composition_error_model_params));
	
	// Fill in the params
	model_params->n_models = n_models;
	model_params->model_split = (unsigned*)malloc(sizeof(unsigned) * model_params->n_models);
	model_params->error_models = (error_model_f*)malloc(sizeof(error_model_f) * model_params->n_models);
	model_params->error_models_params = (void**)malloc(sizeof(void*) * model_params->n_models);

	unsigned counter = 0;

	va_list argv;
	va_start(argv, n_models);

	enum mode {MODEL_SPLIT_e, ERROR_MODELS_e, MODEL_params_e, TERM_e};
	unsigned current_mode = MODEL_SPLIT_e;

	while (current_mode != TERM_e)
	{
		switch (current_mode)
		{
			case MODEL_SPLIT_e:
				model_params->model_split[counter] = va_arg(argv, unsigned);
			break;

			case ERROR_MODELS_e:
				model_params->error_models[counter] = va_arg(argv, error_model_f);
			break;

			case MODEL_params_e:
				model_params->error_models_params[counter] = va_arg(argv, void*);
			break;
		}

		counter++;

		if (counter == model_params->n_models)
		{
			current_mode++;
			counter = 0;
		}
	}
	va_end(argv);

	m->model_params = model_params;
	m->model_call = error_model_call_multi_composition;
	m->param_free = error_model_free_multi_composition;

	return m;
}


double error_model_call_multi_composition(const sym* error, void* v_model_params)
{
	double prob = 1;
	unsigned current_qubit = 0;
	struct multi_composition_error_model_params* model_params = (struct multi_composition_error_model_params*)v_model_params;

	for (size_t i = 0; i < model_params->n_models; i++)
	{
		sym* partial_error = sym_create(1, model_params->model_split[i] * 2);

		for (size_t j = 0; j < model_params->model_split[i]; j++)
		{
			sym_set(partial_error, 0, j, sym_get(error, 0, current_qubit + j));
			sym_set(partial_error, 0, j + model_params->model_split[i], sym_get(error, 0, (error->length / 2) + current_qubit + j));
		}

		prob *= model_params->error_models[i](partial_error, model_params->error_models_params[i]);
		current_qubit += model_params->model_split[i];
		
		sym_free(partial_error);
	}	
	return prob;
}

void error_model_free_multi_composition(void* v_model_params)
{
	struct multi_composition_error_model_params* model_params = (struct multi_composition_error_model_params*)v_model_params;
	free(model_params->model_split);
	free(model_params->error_models);
	free(model_params->error_models_params);
	free(model_params);
	return;
}
