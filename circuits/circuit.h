#ifndef CIRCUIT_STRUCT
#define CIRCUIT_STRUCT
#include "../sym.h" 
#include "../gates/gates.h"
#include "error_probabilities.h"

// ----------------------------------------------------------------------------------------
// STRUCT OBJECTS 
// ----------------------------------------------------------------------------------------

// Forward declaration of the circuit object
struct circuit;

// Circuit run struct
typedef double* (*circuit_run_f)(struct circuit*, double*, gate*);

// Circuit free struct
typedef void (*circuit_param_free_f)(void*);

/*
 *   circuit_element:
 *   An individual element in the circuit list
 *   :: gate* gate_operation :: The gate being applied
 *   :: unsigned* target_qubits :: The qubits this gate is to be applied to
 *   :: struct circuit_element* next :: The next circuit element
 */
struct circuit_element
{
    gate* gate_operation; // The gate object to be applied
    unsigned* target_qubits; // An array containing the target qubits
    struct circuit_element* next; // The next gate to be applied
};
typedef struct circuit_element circuit_element;

/*
 *  circuit:
 *  A list of circuit elements
 *  :: unsigned n_gates :: The number of gates being applied
 *  :: struct circuit_element* start :: The first element in the list
 *  :: struct circuit_element* end :: The last element in the list
 */
typedef struct circuit
{
    unsigned n_qubits; // Number of qubits in the circuit
    unsigned n_gates; // Number of gates in the circuit
    circuit_element* start; // The starting gate of the circuit
    circuit_element* end; // The final gate of the circuit
    circuit_run_f circuit_operation; // The function that runs the circuit
    circuit_param_free_f circuit_param_free; // The function to free the circuit
    void* circuit_data; // Any other data that the circuit might require
} circuit;

// ----------------------------------------------------------------------------------------
// FUNCTION DECLARATIONS
// ----------------------------------------------------------------------------------------

/*
 *  circuit_create:
 *  Creates a new circuit object
 *  Returns a heap pointer to the new circuit
 */
circuit* circuit_create(unsigned n_qubits);

/* 
 *  circuit_add_non_varg:
 *  Adds a gate to an existing circuit
 *  :: circuit* c :: The circuit the gate is being added to
 *  :: gate* g :: The gate being added
 *  :: unsigned* target_qubits :: The qubits the gate is to be applied to
 *  Does not return anything
 */
void circuit_add_non_varg(circuit* c, gate* g, unsigned* target_qubits);

/* 
 *  circuit_add:
 *  Adds a gate to an existing circuit uses vargs for ease of readability
 *  :: circuit* c :: The circuit the gate is being added to
 *  :: gate* g :: The gate being added
 *  :: ... :: Variadic target qubits
 *  Does not return anything
 */
void circuit_add_gate(circuit* c, gate* g, ...);

/* 
 *  circuit_add_non_varg_start:
 *  Adds a gate to an existing circuit, places this gate at the start of the circuit 
 *  :: circuit* c :: The circuit the gate is being added to
 *  :: gate* g :: The gate being added
 *  :: unsigned* target_qubits :: The qubits the gate is to be applied to
 *  Does not return anything
 */
void circuit_add_non_varg_start(circuit* c, gate* g, unsigned* target_qubits);

/* 
 *  circuit_add_start:
 *  Adds a gate to an existing circuit uses vargs for ease of readability
 *  Places this gate at the start of the circuit
 *  :: circuit* c :: The circuit the gate is being added to
 *  :: gate* g :: The gate being added
 *  :: ... :: Variadic target qubit indicies
 *  Does not return anything
 */
void circuit_add_gate_start(circuit* c, gate* g, ...);

/*
 * circuit_run
 * Dispatch method to call the run operation of the circuit
 * :: circuit* c :: The circuit to be run
 * :: double* initial_error_rates :: The initial error rates passed to the circuit
 * :: gate* noise :: The noise model to be applied at each point on the circuit
 * Returns a new error rate object
 */
double* circuit_run(circuit* c, double* initial_error_rates, gate* noise);

/*
 * circuit_param_free
 * Dispatch method to call free function on the circuit parameters
 * :: circuit* c :: The circuit whose parameters are to be freed
 *  Returns nothing
 */
void circuit_param_free(circuit* c);

/* 
    circuit_run_default:
    Applies a circuit to an existing set of error probabilities
    :: double* initial_error_rates :: The error rates before the circuit is applied
    :: circuit* c :: The circuit the gate is being added to
    :: error_model* noise :: The noise to be applied
    Returns a heap pointer to the new set of error rates
*/
double* circuit_run_default(circuit* c, double* initial_error_rates, gate* noise);

/*
 *  circuit_param_free_default:
 *  Frees the parameters associated with a quantum circuit object
 *  :: void* c :: Pointer to the circuit params to be freed
 *  No return
 */
void circuit_param_free_default(void* params);

/* 
    circuit_run_noiseless:
    Applies a circuit to an existing set of error probabilities
    :: double* initial_error_rates :: The error rates before the circuit is applied
    :: circuit* c :: The circuit the gate is being added to
    Returns a heap pointer to the new set of error rates
*/
double* circuit_run_noiseless(circuit* c, double* initial_error_rates);


/*
    circuit_free:
    Frees a quantum circuit object
    :: circuit* c :: Pointer to the circuit to be freed
    No return
*/
void circuit_free_default(circuit* c);

// ----------------------------------------------------------------------------------------
// FUNCTION DEFINITIONS 
// ----------------------------------------------------------------------------------------

/* 
 *   circuit_create:
 *   Creates a new circuit object
 *   Returns a heap pointer to the new circuit
 */
circuit* circuit_create(const unsigned n_qubits)
{
    circuit* c = (circuit*)malloc(sizeof(circuit));
    c->circuit_operation = circuit_run_default;
    c->circuit_param_free = circuit_param_free_default;
    c->n_qubits = n_qubits;
    c->n_gates = 0;
    c->start = NULL;
    c->end = NULL;
    return c;
}

double* circuit_run(circuit* c, double* initial_error_rates, gate* noise)
{
    return c->circuit_operation(c, initial_error_rates, noise);
}

/*
 * circuit_param_free
 * Dispatch method to call free function on the circuit parameters
 * :: circuit* c :: The circuit whose parameters are to be freed
 *  Returns nothing
 */
void circuit_param_free(circuit* c)
{
    c->circuit_param_free(c->circuit_data);
}

/* 
    circuit_add_gate:
    Adds a gate to an existing circuit uses vargs for ease of readability
    :: circuit* c :: The circuit the gate is being added to
    :: gate* g :: The gate being added
    :: ... :: Variadic target qubit indicies
    Does not return anything
*/
void circuit_add_gate(circuit* c, gate* g, ...)
{
    // Create the va_list
    va_list args;
    va_start(args, g);

    // Allocate memory for the target qubit array
    unsigned* target_qubits = (unsigned*)malloc(sizeof(unsigned) * g->n_qubits);

    // For each qubit that we are expecting, take the next variadic argument and copy it to the array
    for (unsigned i = 0; i < g->n_qubits; i++)
    {
        target_qubits[i] = va_arg(args, unsigned);
    }
    
    // Add the new gate using the regular function
    circuit_add_non_varg(c, g, target_qubits);

    free(target_qubits);
    return;
}

/* 
    circuit_add_non_varg:
    Adds a gate to an existing circuit, places this gate at the end of the circuit
    :: circuit* c :: The circuit the gate is being added to
    :: gate* g :: The gate being added
    :: unsigned* target_qubits :: The qubits the gate is to be applied to
    Does not return anything
*/
void circuit_add_non_varg(circuit* c, gate* g, unsigned* target_qubits)
{
    circuit_element* ce = (circuit_element*)malloc(sizeof(circuit_element));
    ce->gate_operation = g;
    ce->target_qubits = (unsigned*)malloc(sizeof(unsigned) * g->n_qubits);
    memcpy(ce->target_qubits, target_qubits, g->n_qubits * sizeof(unsigned));
    ce->next = NULL;
    
    if (NULL != c->start)
    {
        c->end->next = ce;
        c->end = ce;
    }
    else
    {
        c->start = ce;
        c->end = ce;
    }
    c->n_gates++;
    return;
}

/* 
    circuit_add_start:
    Adds a gate to an existing circuit uses vargs for ease of readability
    Places this gate at the start of the circuit
    :: circuit* c :: The circuit the gate is being added to
    :: gate* g :: The gate being added
    :: ... :: Variadic target qubit indicies
    Does not return anything
*/
void circuit_add_gate_start(circuit* c, gate* g, ...)
{
    // Create the va_list
    va_list args;
    va_start(args, g);

    // Allocate memory for the target qubit array
    unsigned* target_qubits = (unsigned*)malloc(sizeof(unsigned) * g->n_qubits);

    // For each qubit that we are expecting, take the next variadic argument and copy it to the array
    for (unsigned i = 0; i < g->n_qubits; i++)
    {
        target_qubits[i] = va_arg(args, unsigned);
    }
    va_end(args);
    
    // Add the new gate using the regular function
    circuit_add_non_varg_start(c, g, target_qubits);

    free(target_qubits);
    return;
}

/* 
    circuit_add_non_varg_start:
    Adds a gate to an existing circuit, places this gate at the start of the circuit 
    :: circuit* c :: The circuit the gate is being added to
    :: gate* g :: The gate being added
    :: unsigned* target_qubits :: The qubits the gate is to be applied to
    Does not return anything
*/
void circuit_add_non_varg_start(circuit* c, gate* g, unsigned* target_qubits)
{
    circuit_element* ce = (circuit_element*)malloc(sizeof(circuit_element));
    ce->gate_operation = g;
    ce->target_qubits = (unsigned*)malloc(sizeof(unsigned) * g->n_qubits);
    memcpy(ce->target_qubits, target_qubits, g->n_qubits * sizeof(unsigned));
    
    // Add to the start of the list
    ce->next = c->start;
    c->start = ce;
    c->n_gates++;
    return;
}


/* 
 *  circuit_run_noiseless:
 *  Applies a circuit to an existing set of error probabilities
 *  :: double* initial_error_rates :: The error rates before the circuit is applied
 *  :: circuit* c :: The circuit the gate is being added to
 *  :: const unsigned n_qubits :: The number of qubits
 *  Returns a heap pointer to the new set of error rates
 */
double* circuit_run_noiseless(circuit* c, double* initial_error_rates)
{
    double* error_rate = error_probabilities_copy(c->n_qubits, initial_error_rates);

    unsigned long n_bytes = sizeof(double) * (1 << (c->n_qubits * 2));
    circuit_element* ce = c->start;

    while(ce != NULL)
    {
        double* tmp_error_rate = gate_apply(c->n_qubits, error_rate, ce->gate_operation, ce->target_qubits);
        memcpy(error_rate, tmp_error_rate, n_bytes);
        free(tmp_error_rate);

        ce = ce->next;
    }

    return error_rate;
}


/* 
 *  circuit_run_default:
 *  Applies a circuit to an existing set of error probabilities
 *  :: double* initial_error_rates :: The error rates before the circuit is applied
 *  :: circuit* c :: The circuit the gate is being added to
 *  :: error_model* noise :: The noise to be applied
 *  Returns a heap pointer to the new set of error rates
 */
double* circuit_run_default(circuit* c, double* initial_error_rates, gate* noise)
{

    if (NULL == noise)
    {
        return circuit_run_noiseless(c, initial_error_rates);
    }

    // Noise should act on a single qubit
    if (noise->n_qubits != 1)
    {
        printf("Noise should act on a single qubit!\n");
        return NULL;
    }

    double* error_rate = error_probabilities_copy(c->n_qubits, initial_error_rates);
    unsigned long n_bytes = (1ull << (c->n_qubits * 2)) * sizeof(double);

    circuit_element* ce = c->start;

    while (NULL != ce)
    {
        // Gate operation
        double* tmp_error_rate = gate_apply(c->n_qubits, error_rate, ce->gate_operation, ce->target_qubits);
        memcpy(error_rate, tmp_error_rate, n_bytes);
        free(tmp_error_rate);
    
        // Environmental Noise operations
        for (unsigned i = 0; i < c->n_qubits; i++)
        {
            uint8_t participant = 0;
            // Check that the qubit doesn't participate
            for (uint32_t j = 0; j < ce->gate_operation->n_qubits && !participant; j++)
            {
                if (i == ce->target_qubits[j])
                {
                    participant = 1;
                }
            }

            // If the qubit hasn't participated in the gate, apply the environmental noise
            if (!participant) 
            {
                double* tmp_error_rate = gate_apply(c->n_qubits, error_rate, noise, &i);
                memcpy(error_rate, tmp_error_rate, n_bytes);
                free(tmp_error_rate);   
            }
        }

        ce = ce->next;
    }

    return error_rate;
}


/*
 *  circuit_param_free_default:
 *  Frees the parameters associated with a quantum circuit object
 *  :: void* c :: Pointer to the circuit params to be freed
 *  No return
 */
void circuit_param_free_default(void* params)
{
    if (params != NULL)
    {
        free(params);
    }
    return;
}


/*
 *  circuit_free:
 *  Frees a quantum circuit object
 *  :: circuit* c :: Pointer to the circuit to be freed
 *  No return
 */
void circuit_free(circuit* c)
{
    circuit_element* ce = c->start;
    while(ce != NULL)
    {
        circuit_element* ce_next = ce->next;
        free(ce->target_qubits);
        free(ce);
        ce = ce_next;
    }
    circuit_param_free(c);
    free(c);
}

#endif