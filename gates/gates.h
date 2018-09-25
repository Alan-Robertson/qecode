#ifndef GATES
#define GATES
#include "../sym.h" 
#include "../sym_iter.h"
#include "gate_result.h"
#include "../circuits/error_probabilities.h"
#include "../error_models/error_models.h"


// STRUCT OBJECTS ----------------------------------------------------------------------------------------

//  Gate operation function pointer
// typedef sym* (*gate_operation_f)(const sym*, void*, const unsigned* target_qubits);
typedef gate_result* (*gate_operation_f)(const sym*, const void*, const unsigned* target_qubits);

/*
 *	gate:
 *	The gate struct
 *	:: unsigned n_qubits :: Number of qubits in the gate
 *	:: error_model_f gate_error_model :: The error model to be applied when this gate is used
 *	:: void* model_data :: Data for the error model
 */
typedef struct {
	unsigned n_qubits; // Number of qubits the gate operates on
	gate_operation_f operation; // The gate operation performed (any absolute operation)
	error_model* gate_error_model; // The error operation performed (Any probabilistic operation)
	void* operation_data; // Any additional data to pass to the gate
	void* error_model_data; // Any additional data to pass to the noise
} gate;

// MULTITHREADING ----------------------------------------------------------------------------------------
// Enable to multithread on gate operations
#ifdef GATE_MULTITHREADING
	#ifdef N_THREADS
		#define GATE_MULTITHREADING_ENABLED
		#include <pthread.h>

		// Struct to contain the data needed for the multi-threading
		typedef struct 
		{
			long long start;
			long long end;
			const gate* operation;
			const unsigned* target_qubits;
			unsigned n_qubits;
			double* initial_probabilities;
			double* final_probabilities;
			pthread_mutex_t* lock;
		} mthread_gate_operation_t;

		// Multithreading gate application
		void* gate_apply_m_thread(void* data);

		// Gate lock
		pthread_mutex_t gate_lock;

	#endif
#endif

// FUNCTION DECLARATIONS ----------------------------------------------------------------------------------------
/*
    gate_create:
	Creates a new gate object
	:: const unsigned n_qubits :: Number of qubits in the gate
	:: const error_model_f error_model :: The error model to be applied when this gate is used
	:: void* model_data :: Data for the error model
	Returns a heap pointer to the new gate
*/
gate* gate_create(
	const unsigned n_qubits, 
	gate_operation_f operation,
	error_model* em,
	void* gate_data);

/* 
    gate_create:
	Creates a new gate object
	:: const unsigned n_qubits :: Number of qubits in the gate
	:: gate_operation_f operation :: The operation to be performed when the gate is applied
	Returns a heap pointer to the new gate
*/
gate* gate_create_noiseless(
	const unsigned n_qubits,
	gate_operation_f operation);

/* 
    gate_apply:
	Applies a gate object to an existing noise model this is comprised of both a noise operator and a gate operator
	:: const unsigned n_qubits :: Number of qubits in the gate
	:: const double* initial_probabilities :: The current probabilities for each noise operator
	:: const void* noisy_gate :: The gate to be applied
	Returns a heap pointer to a block of allocated memory containing the new probabilities
*/
double* gate_apply(
	const unsigned n_qubits,
	double* probabilities,
	const gate* g,
	const unsigned* target_qubits);

/* 
    gate_noise:
	Applies a gate object to an existing noise model
	:: const unsigned n_qubits :: Number of qubits in the gate
	:: const double* initial_probabilities :: The current probabilities for each noise operator
	:: const void* noisy_gate :: The gate to be applied
	Returns a heap pointer to a block of allocated memory containing the new probabilities
*/
double* gate_noise(const unsigned n_qubits, 
	double* probabilities, 
	const gate* g,
	const unsigned* target_qubits);

/* 
    gate_operator:
	Applies a gate object to an existing noise model
	:: const unsigned n_qubits :: Number of qubits in the gate
	:: const double* initial_probabilities :: The current probabilities for each noise operator
	:: const void* noisy_gate :: The gate to be applied
	Returns a heap pointer to a block of allocated memory containing the new probabilities
*/
double* gate_operator(const unsigned n_qubits, 
	double* probabilities, 
	const gate* g,
	const unsigned* target_qubits);


gate_result* gate_operation(const gate* g, sym* initial_state, const unsigned* target_qubits);

/* 
    gate_free:
	Frees a gate object
	:: gate* g :: The gate to be freed
	Does not return anything
*/
void gate_free(gate* g);


gate_result* gate_iid(const sym* initial_state, const void* gate_data, const unsigned* target_qubits);

// FUNCTION DEFINITIONS ----------------------------------------------------------------------------------------
/* 
    gate_create:
	Creates a new gate object
	:: const unsigned n_qubits :: Number of qubits in the gate
	:: const error_model em :: The error model to be applied when this gate is used
	Returns a heap pointer to the new gate
*/
gate* gate_create(
	const unsigned n_qubits,
	gate_operation_f operation,
	error_model* em,
	void* operation_data)
{
	gate* g = (gate*)malloc(sizeof(gate));

	// Add all the arguments to the appropriate fields of the struct
	g->n_qubits = n_qubits;
	g->operation = operation;
	g->gate_error_model = em;
	g->operation_data = operation_data;
	
	return g;
}

/* 
    gate_create:
	Creates a new gate object
	:: const unsigned n_qubits :: Number of qubits in the gate
	:: gate_operation_f operation :: The operation to be performed when the gate is applied
	Returns a heap pointer to the new gate
*/
gate* gate_create_noiseless(
	const unsigned n_qubits,
	gate_operation_f operation)
{
	return gate_create(n_qubits, operation, NULL, NULL);
}

/* 
    gate_create_independent:
	Creates a new gate object, copies the data rather than references it
	:: const unsigned n_qubits :: Number of qubits in the gate
	:: const error_model* em :: The error model to be applied when this gate is used
	:: void* operation_data :: Data for the operation
	Returns a heap pointer to the new gate
*/
gate* gate_create_independent(
	const unsigned n_qubits,
	gate_operation_f operation,
	error_model* em,
	void* operation_data)
{
	gate* g = (gate*)malloc(sizeof(gate));

	// Add all the arguments to the appropriate fields of the struct
	g->n_qubits = n_qubits;
	g->operation = operation;
	g->gate_error_model = error_model_copy(em);
	g->operation_data = operation_data;
	
	return g;
}

gate* gate_create_iid_noise(error_model* em)
{
	return gate_create(1, NULL, em, NULL);
}

/* 
    gate_apply:
	Applies a gate object to an existing noise model this is comprised of both a noise operator and a gate operator
	:: const unsigned n_qubits :: Number of qubits in the gate
	:: const double* initial_probabilities :: The current probabilities for each noise operator
	:: const void* noisy_gate :: The gate to be applied
	Returns a heap pointer to a block of allocated memory containing the new probabilities
*/
double* gate_apply(const unsigned n_qubits,
	double* probabilities,
	const gate* g,
	const unsigned* target_qubits)
{
	// Apply the gate operation	
	double* gate_operator_probabilities = gate_operator(n_qubits, probabilities, g, target_qubits);
	
	// Apply the noise operation associated with that gate
	double* gate_noise_probabilities = gate_noise(n_qubits, gate_operator_probabilities, g, target_qubits);

	// Free allocated memory that is no longer required
	free(gate_operator_probabilities);
	return gate_noise_probabilities;
}


/* 
    gate_noise:
	Applies a noise object to an existing noise model
	:: const unsigned n_qubits :: Number of qubits in the gate
	:: const double* initial_probabilities :: The current probabilities for each noise operator
	:: const gate* g:: The gate to be applied
	Returns a heap pointer to a block of allocated memory containing the new probabilities
*/
double* gate_noise(const unsigned n_qubits, 
	double* initial_probabilities, 
	const gate* applied_gate,
	const unsigned* target_qubits)
{
	// Allocate memory for the new output
	double* p_state_probabilities = error_probabilities_zeros(n_qubits);

	// Identity gate, no operation_output
	
	if (NULL == applied_gate->gate_error_model)
	{
		memcpy(p_state_probabilities, initial_probabilities, error_probabilities_bytes_in_table(n_qubits));
		return p_state_probabilities;
	}

	// Check if multithreaded
	#ifdef GATE_MULTITHREADING_ENABLED

		mthread_gate_operation_t thread_data[N_THREADS];
		pthread_t threads[N_THREADS]; // Our threads

		// Chunk the blocks
		uint64_t last_block = error_probabilities_entries_in_table(n_qubits);
		uint64_t block_size = last_block / N_THREADS;

		// Setup our thread data
		for (uint32_t i = 0; i < N_THREADS; i++)
		{
			/*
			 Struct to contain the data needed for the multi-threading
			 Replicated here for useful shorthand
				typedef struct 
				{
					long long start;
					long long end;
					const gate* operation;
					const unsigned* target_qubits;
					const unsigned n_qubits;
					double* initial_probabilities;
					double* final_probabilities;
					pthread_mutex_t* lock;
				} mthread_gate_operation_t;
			*/
			if (i == N_THREADS - 1) // Handle the last block separately
			{
				thread_data[i].start = i * block_size;
				thread_data[i].end = last_block;
				thread_data[i].operation = applied_gate;
				thread_data[i].target_qubits = target_qubits;
				thread_data[i].n_qubits = n_qubits; 
				thread_data[i].initial_probabilities = initial_probabilities; 
				thread_data[i].final_probabilities = p_state_probabilities;
				thread_data[i].lock = &gate_lock;
			}
			else
			{
				thread_data[i].start = i * block_size;
				thread_data[i].end = (i + 1) * block_size;
				thread_data[i].operation = applied_gate;
				thread_data[i].target_qubits = target_qubits;
				thread_data[i].n_qubits = n_qubits; 
				thread_data[i].initial_probabilities = initial_probabilities; 
				thread_data[i].final_probabilities = p_state_probabilities;
				thread_data[i].lock = &gate_lock;
			}
		}

		// Release the threads
		for (uint32_t i = 0; i < N_THREADS; i++)
		{
			pthread_create(threads + i, NULL, gate_apply_m_thread, thread_data + i);
		}

		// Join the threads
		for (uint32_t i = 0; i < N_THREADS; i++)
		{
			pthread_join(threads[i], NULL);
		}

	#else // If not multithreading
		// Loop over all possible states
		sym_iter* initial_state = sym_iter_create_n_qubits(n_qubits);
		while(sym_iter_next(initial_state))
		{
			// Save this value as we may be needing it quite a bit
			double initial_prob = initial_probabilities[sym_to_ll(initial_state->state)];

			if (initial_prob > 0)
			{
				// Determine the state after the error has been applied 
				gate_result* operation_output = gate_iid(initial_state->state, applied_gate, target_qubits);

				for (unsigned i = 0; i < operation_output->n_results; i++)
				{
					// Cumulatively determine the new probability of each state after the gate has been applied
					p_state_probabilities[sym_to_ll(operation_output->state_results[i])] += operation_output->prob_results[i] * initial_prob; 
				}

				// Free allocated memory
				gate_result_free(operation_output);
			}
		}
		sym_iter_free(initial_state);
	#endif

	
	return p_state_probabilities;
}


// Multithreading function for gate application
// To enable this you should include the following headers
// Either when you compile the code or or somewhere else
// #define GATE_MULTITHREADING
// #define N_THREADS <>
// Don't forget to specify the number of threads!
#ifdef GATE_MULTITHREADING_ENABLED
void* gate_apply_m_thread(void* data)
{
	mthread_gate_operation_t* mthread_data = (mthread_gate_operation_t*)data;

	// Loop over all possible states
	for (long long ll_state = mthread_data->start; ll_state < mthread_data->end; ll_state++)
	{
		// Convert our long long value to state
		sym* initial_state = ll_to_sym_n_qubits(ll_state, 1, mthread_data->n_qubits);

		// Save this value as we may be needing it quite a bit
		double initial_prob = mthread_data->initial_probabilities[sym_to_ll(initial_state)];

		// Save ourselves some time
		if (initial_prob > 0)
		{
			// Determine the state after the error has been applied 
			gate_result* operation_output = gate_iid(initial_state, mthread_data->operation, mthread_data->target_qubits);

			for (unsigned i = 0; i < operation_output->n_results; i++)
			{
				// Cumulatively determine the new probability of each state after the gate has been applied
				// Make sure to lock down this operation as the only write to the data
				pthread_mutex_lock(mthread_data->lock);

				mthread_data->final_probabilities[sym_to_ll(operation_output->state_results[i])] += operation_output->prob_results[i] * initial_prob; 

				pthread_mutex_unlock(mthread_data->lock);
			}
			// Free allocated memory
			gate_result_free(operation_output);
			sym_free(initial_state);
		}
	}
	return NULL;
}
#endif




// Wrapper
gate_result* gate_operation(const gate* g, sym* initial_state, const unsigned* target_qubits)
{
	return g->operation(initial_state, g, target_qubits);
}

/* 
    gate_operator:
	Applies a gate object to an existing noise model
	:: const unsigned n_qubits :: Number of qubits in the gate
	:: const double* initial_probabilities :: The current probabilities for each noise operator
	:: const void* noisy_gate :: The gate to be applied
	Returns a heap pointer to a block of allocated memory containing the new probabilities
*/
double* gate_operator(const unsigned n_qubits,
	double* initial_probabilities,
	const gate* applied_gate,
	const unsigned* target_qubits)
{
	// Allocate memory for the new output
	double* p_state_probabilities = error_probabilities_zeros(n_qubits);

	// Identity gate, no operation
	if (NULL == applied_gate->operation)
	{
		memcpy(p_state_probabilities, initial_probabilities, error_probabilities_bytes_in_table(n_qubits));
		return p_state_probabilities;
	}

	// Loop over all possible states
	sym_iter* initial_state = sym_iter_create_n_qubits(n_qubits);
	while(sym_iter_next(initial_state))
	{
		// Save this value as we may be needing it quite a bit
		double initial_prob = initial_probabilities[sym_to_ll(initial_state->state)];

		if (initial_prob > 0)
		{
			// Determine the state after the error has been applied 
		
			gate_result* operation_output = gate_operation(applied_gate, initial_state->state, target_qubits);

			for (unsigned i = 0; i < operation_output->n_results; i++)
			{
				// Cumulatively determine the new probability of each state after the gate has been applied
				p_state_probabilities[sym_to_ll(operation_output->state_results[i])] += operation_output->prob_results[i] * initial_prob; 

			}
			// Free allocated memory
			gate_result_free(operation_output);
		}
	}

	sym_iter_free(initial_state);

	return p_state_probabilities;
}

/* 
    gate_free:
	Frees a gate object
	:: gate* g :: The gate to be freed
	Does not return anything
*/
void gate_free(gate* g)
{
	free(g);
}

// Applies an iid error model
gate_result* gate_iid(const sym* initial_state, const void* gate_data, const unsigned* target_qubits)
{
	gate* g = (gate*)gate_data;
	gate_result* gr = gate_result_create(1 << (g->n_qubits * 2));

	uint32_t count = 0;
	sym_iter* gate_error = sym_iter_create_n_qubits(g->n_qubits);
	while (sym_iter_next(gate_error))
	{
		gr->state_results[count] = sym_partial_add(initial_state, gate_error->state, target_qubits);
		gr->prob_results[count] = error_model_call(g->gate_error_model, gate_error->state);
		count++;
	}
	sym_iter_free(gate_error);

	return gr;
}

#endif
