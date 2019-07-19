#ifndef GATES
#define GATES

#include <limits.h>

#include "../sym.h" 
#include "../sym_iter.h"

#include "gate_result.h"
#include "target_qubits.h"

#include "../circuits/error_probabilities.h"
#include "../error_models/error_models.h"

/* GATE_MULTITHREADING
 * Enables gate multi-threading 	
 */

/* N_THREADS #
 *	Specifies the number of threads used when multi-threading gate operations
 */

/* GATE_MAX_DEPTH #
 * Sets a maximum depth of non identity elements in pauli strings
 * Useful when looking at objects of 12 or more qubits, this will speed up computation by ignoring 
 * presumably low probability pauli strings with high pauli weights
 * As a general rule, this should be set to the number of qubits in the code block + some number of ancilla qubits
 * At which point the error is generally unrecoverable anyway
 * If you enable this, you will need to start considering that the results will not be normalised	
 */

// STRUCT OBJECTS ----------------------------------------------------------------------------------------

// Forward declaration of the gate type

//  Gate operation function pointer
// The second object is actually the gate object
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

// GATE_MAX_DEPTH -------------------------------------------------------------------------------------
// Enable to limit application over pauli strings to a certain depth
// This should be set to the value of the max depth
#ifdef GATE_MAX_DEPTH
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


gate_result* gate_apply_noise(const sym* initial_state, const void* gate_data, const unsigned* target_qubits);

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
	// This ordering is slightly more computationally efficient; 

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

	// Check if multi threaded
	#ifdef GATE_MULTITHREADING_ENABLED
		int32_t threads_used = N_THREADS;

		mthread_gate_operation_t thread_data[N_THREADS];
		pthread_t threads[N_THREADS]; // Our threads

		// If we've got a max gate depth
		#ifdef GATE_MAX_DEPTH 

			if (n_qubits > GATE_MAX_DEPTH)
			{
				// Generate an array containing the number of elements to iterate through for each weight
				uint32_t* binom_dist = (uint32_t*)malloc(sizeof(uint32_t) * (GATE_MAX_DEPTH * 2 + 1));
				int32_t avg = 0;
				
				for (uint32_t i = 0; i <= 2 * GATE_MAX_DEPTH; i++)
				{
					binom_dist[i] = sym_iter_binom(2 * n_qubits, i);
					avg += binom_dist[i];
				}
				
				avg /= (N_THREADS);

				// Now we attempt to evenly distribute joint segments of this array over the number of threads
				// And we're going to be greedy
				// Start from the far end; if we're peaked it's going to distribute the work more evenly, if we're not then 
				// It's symmetric
				int32_t current_position = 2 * GATE_MAX_DEPTH;
				int32_t current_delta = 0;
				for (uint32_t i = 0; i < N_THREADS && current_position >= 0; i++)
				{
					int32_t total = binom_dist[current_position];
					thread_data[i].start = 0;
					thread_data[i].end = current_position + 1;

					while (total + current_delta < avg && current_position >= 0)
					{
						current_position--;
						total += binom_dist[current_position];

						// If we reach the end and run out of threads
						// This may occur due to a rounding error when calculating the average
						if (current_position == 0)
						{ 
							current_position = 0;
							total = INT_MAX;
							current_delta = 0;
							threads_used = i + 1;
						}
					}
					
					thread_data[i].start = current_position;
					current_position--;
					current_delta = avg - total;

					// And the rest of the standard thread data
					thread_data[i].operation = applied_gate;
					thread_data[i].target_qubits = target_qubits;
					thread_data[i].n_qubits = n_qubits; 
					thread_data[i].initial_probabilities = initial_probabilities; 
					thread_data[i].final_probabilities = p_state_probabilities;
					thread_data[i].lock = &gate_lock;
				}

				free(binom_dist);
			}
			else // Number of qubits is smaller than the max gate depth, break up the problem evenly between the threads
			{
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
			}
		#else
			// Chunk the blocks
			uint64_t last_block = error_probabilities_entries_in_table(n_qubits);
			uint64_t block_size = last_block / N_THREADS;

			// Setup our thread data
			for (uint32_t i = 0; i < threads_used; i++)
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
		#endif

		// Release the threads
		for (uint32_t i = 0; i < threads_used; i++)
		{
			pthread_create(threads + i, NULL, gate_apply_m_thread, thread_data + i);
		}

		// Join the threads
		for (uint32_t i = 0; i < threads_used; i++)
		{
			pthread_join(threads[i], NULL);
		}
	#else // If not multithreading
		// Loop over all possible states
		// Different behaviour for max depth set or not
		#ifdef GATE_MAX_DEPTH
			sym_iter* initial_state = sym_iter_create_n_qubits_range(n_qubits, 0, GATE_MAX_DEPTH);
		#else
			sym_iter* initial_state = sym_iter_create_n_qubits(n_qubits);
		#endif
		while(sym_iter_next(initial_state))
		{
			// Save this value as we may be needing it quite a bit
			double initial_prob = initial_probabilities[sym_iter_ll_from_state(initial_state)];

			if (initial_prob > 0)
			{
				// Determine the state after the error has been applied 
				gate_result* operation_output = gate_apply_noise(initial_state->state, applied_gate, target_qubits);

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


// Multi-threading function for gate application
// To enable this you should include the following headers
// Either when you compile the code or or somewhere else
// #define GATE_MULTITHREADING
// #define N_THREADS #
// Don't forget to specify the number of threads!
#ifdef GATE_MULTITHREADING_ENABLED
void* gate_apply_m_thread(void* data)
{
	mthread_gate_operation_t* mthread_data = (mthread_gate_operation_t*)data;

	// If a maximum gate depth is set, change the behavior of this loop
	#ifdef GATE_MAX_DEPTH
		if (mthread_data->n_qubits > GATE_MAX_DEPTH)
		{
			// Loop over all states in the range
			sym_iter* siter = sym_iter_create_range(mthread_data->n_qubits * 2,  mthread_data->start, mthread_data->end);
			while (sym_iter_next(siter))
			{
				
				// Save this value as we may be needing it quite a bit
				double initial_prob = mthread_data->initial_probabilities[sym_iter_ll_from_state(siter)];

				// Save ourselves some time
				if (initial_prob > 0)
				{	
					gate_result* operation_output;
					if () // Check if this is actually an IID error
						// Determine the state after the error has been applied 
						operation_output = gate_apply_noise(siter->state, mthread_data->operation, mthread_data->target_qubits);
					else
					{
						operation_output = 
					}
					
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
				}
			}
			sym_iter_free(siter);
		}
		else
		{
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
					gate_result* operation_output = gate_apply_noise(initial_state, mthread_data->operation, mthread_data->target_qubits);

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
		}
	#else // No max depth set
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
				gate_result* operation_output = gate_apply_noise(initial_state, mthread_data->operation, mthread_data->target_qubits);

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
	#endif
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
		double initial_prob = initial_probabilities[sym_iter_ll_from_state(initial_state)];

		if (initial_prob > 0)
		{
			// Determine the state after the operation has been applied 

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
    gate_multi_free:
	Frees a vargs collection of gate objects
	:: const size_t n_gates :: The gate to be freed
	:: VARGS :: The pointers to the gate objects to be freed
	Does not return anything
*/
void gate_multi_free(const size_t n_gates, ...)
{

	va_list args;
    va_start(args, n_gates);

    for (size_t i = 0; i < n_gates; i++)
    {
    	gate_free(va_arg(args, gate*));
    }
    va_end(args);

    return;
}

/* 
    gate_free:
	Frees a gate object
	:: gate* g :: The gate to be freed
	Does not return anything
*/
void gate_free(gate* g)
{
	if (g->operation_data != NULL)
	{
		free(g->operation_data);
	}
	if (g->error_model_data != NULL)
	{
		free(g->error_model_data);
	}
	free(g);
}

// Applies an iid operation
gate_result* gate_apply_noise(const sym* initial_state, const void* gate_data, const unsigned* target_qubits)
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
