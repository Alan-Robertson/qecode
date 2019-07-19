#ifndef SYMPLECTIC_ITERATOR
#define SYMPLECTIC_ITERATOR

// ----------------------------------------------------------------------------------------
// LIBRARIES
// ----------------------------------------------------------------------------------------

#include "sym.h"

// ----------------------------------------------------------------------------------------
// STRUCT OBJECTS
// ----------------------------------------------------------------------------------------

/*
    sym_iter:
    The symplectic matrix iterator
    :: sym* state :: Current state of the iterator
    :: unsigned max_weight :: Maximum hamming weight for the iterator
    :: unsigned curr_weight :: Current hamming weight for the iterator
    :: long long counter :: Counter position at the current hamming weight
    :: long long max_counter :: Maximum counter position for this hamming weight
*/

typedef struct {
    sym* state; // The current state of our sym iterator
    uint32_t length;
    int32_t max_weight; // The maximum weight we will be testing
    int32_t curr_weight; // Our current weight
    int64_t ll_counter; // Current counter state
    int64_t max_ll_counter; // The maximum state allowed
} sym_iter; 

// ----------------------------------------------------------------------------------------
// FUNCTION DECLARATIONS
// ----------------------------------------------------------------------------------------

/* 
    sym_iter_create:
    Creates a new symplectic matrix object iterator
    :: const unsigned length :: Length of the iterator in bits (2 * qubits)
    Returns a heap pointer to the new iterator
*/
sym_iter* sym_iter_create(const uint32_t length);

/* 
    sym_iter_create_n_qubits:
    Creates a new symplectic matrix object
    :: const unsigned n_qubits :: Number of qubits to iterate over
    Returns a heap pointer to the new iterator
*/
sym_iter* sym_iter_create_n_qubits(const uint32_t length);

/* 
    sym_iter_create_n_qubits_range:
    Creates a new symplectic matrix object iterator
    :: const unsigned n_qubits :: Number of qubits to iterate over
    :: const unsigned min_weight :: Minimum number of qubits for the iterator
    :: const unsigned max_weight :: Maximum number of qubits for the iterator
    Returns a heap pointer to the new iterator
*/
sym_iter* sym_iter_create_n_qubits_range(const uint32_t length, const uint32_t min_weight, const uint32_t max_weight);

/* 
    sym_iter_create_range:
    Creates a new symplectic matrix object iterator
    :: const unsigned length :: Length of the iterator in bits (2 * qubits)
    :: const unsigned min_weight :: Minimum hamming weight for the iterator
    :: const unsigned max_weight :: Maximum hamming weight for the iterator
    Returns a heap pointer to the new iterator
*/
sym_iter* sym_iter_create_range(const uint32_t length, const int32_t min_weight, const uint32_t max_weight);

/*
    sym_iter_next:
    Updates the state of the sym iterator iterator
    :: sym_iter* siter :: The iterator whose state is to be updated
    Returns a boolean value, true indicates that the iterator was updated, false indicates that the end of the range has been reached
*/
uint8_t sym_iter_next(sym_iter* siter);

/*
    sym_iter_update:
    Updates the tracking parameters for the sym iterator
    Do this manually if you've changed the state without calling next
    :: sym_iter* siter :: The iterator whose state is to be cast 
    Does not return anything, updates the values of the iterator in place.
*/
void sym_iter_update(sym_iter* siter);

/*
    sym_iter_ll_from_state:
    Casts the current state of the iterator to long long
    :: const sym_iter* siter :: The iterator whose state is to be cast
    Returns a long long representation of the state of the iterator
*/
long long sym_iter_ll_from_state(const sym_iter* siter);

/*
    sym_iter_state_from_ll:
    Casts from long long to the state of the iterator
    :: sym_iter* siter :: The iterator whose state is to be cast
    :: const long long val :: The long long val 
    Does not return anything, updates the value of the iterator in place.
*/
void sym_iter_state_from_ll(sym_iter* siter, long long val);

/*
    sym_iter_max_ll_counter:
    Calculate (n, k) to determine the number of elements in the iterator with the same hamming weight 
    :: unsigned length::
    :: unsigned weight::
    Returns an unsigned long long containing the result
*/
long long sym_iter_max_ll_counter(uint32_t length, uint32_t current_weight);


/*
    sym_iter_free:
    Frees a symplectic iterator object
    :: sym_iter* siter :: Pointer to the object to be freed
    No return
*/
void sym_iter_free(sym_iter* siter);

/*
    sym_iter_right_shift:
    Shifts an entire sym array right by the specified number of bits
    :: sym_iter* siter :: The iterator to shift
    :: const unsigned shift :: The number of bits to shift by
    Does not return anything, value is modified in place.
*/
//void sym_iter_left_shift(sym_iter* siter, const unsigned shift);

/*
    sym_iter_left_shift:
    Shifts an entire sym array left by the specified number of bits
    :: sym_iter* siter :: The iterator to shift
    :: const unsigned shift :: The number of bits to shift by
    Does not return anything, value is modified in place.
*/
//void sym_iter_right_shift(sym_iter* siter, const unsigned shift);

#endif