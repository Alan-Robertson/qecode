#ifndef SYMPLECTIC_ITERATOR
#define SYMPLECTIC_ITERATOR

// LIBRARIES ----------------------------------------------------------------------------------------

#include "sym.h"

// STRUCT OBJECTS ----------------------------------------------------------------------------------------

/*
	sym:
	The symplectic matrix iterator
	:: sym* state :: Number of rows in the matrix
	:: unsigned max_weight :: Maximum hamming weight for the iterator
	:: unsigned curr_weight :: Current hamming weight for the iterator
	:: long long counter :: Counter position at the current hamming weight
	:: long long max_counter :: Maximum counter position for this hamming weight
*/

typedef struct {
	sym* state; // The current state of our sym iterator
	unsigned length;
	unsigned max_weight; // The maximum weight we will be testing
	unsigned curr_weight; // Our current weight
	long long counter; // Current counter state
	long long max_counter; 
} sym_iter; 

// FUNCTION DECLARATIONS ----------------------------------------------------------------------------------------
/* 
    sym_iter_create:
	Creates a new symplectic matrix object
	:: const unsigned length :: Length of the iterator in bits (2 * qubits)
	Returns a heap pointer to the new iterator
*/
sym_iter* sym_iter_create(const unsigned length);

/* 
    sym_iter_create_range:
	Creates a new symplectic matrix object
	:: const unsigned length :: Length of the iterator in bits (2 * qubits)
	:: const unsigned min_weight :: Minimum hamming weight for the iterator
	:: const unsigned max_weight :: Maximum hamming weight for the iterator
	Returns a heap pointer to the new iterator
*/
sym_iter* sym_iter_create_range(const unsigned length, const unsigned min_weight, const unsigned max_weight);


/*
	sym_iter_next:
	Updates the state of the sym iterator
	:: sym_iter* siter :: The iterator whose state is to be updated
	Returns a boolean value, true indicates that the iterator was updated, false indicates that the end of the range has been reached
*/
bool sym_iter_next(sym_iter* siter);

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
	sym_iter_max_counter:
	Calculate (n, k) to determine the number of elements in the iterator with the same hamming weight 
	:: unsigned length::
	:: unsigned weight::
	Returns an unsigned long long containing the result
*/
unsigned long long sym_iter_max_counter(unsigned length, unsigned current_weight);

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

/*
	sym_iter_free:
	Frees a symplectic iterator object
	:: sym_iter* siter :: Pointer to the object to be freed
	No return
*/
void sym_iter_free(sym_iter* siter);

// FUNCTION DEFINITIONS ----------------------------------------------------------------------------------------
/* 
    sym_iter_create:
	Creates a new symplectic matrix object
	:: const unsigned length :: Length of the iterator in bits (2 * qubits)
	Returns a heap pointer to the new iterator
*/
sym_iter* sym_iter_create(const unsigned length)
{
	sym_iter* siter = sym_iter_create_range(length, 0, length);
}

/* 
    sym_iter_create_range:
	Creates a new symplectic matrix object
	:: const unsigned length :: Length of the iterator in bits (2 * qubits)
	:: const unsigned min_weight :: Minimum hamming weight for the iterator
	:: const unsigned max_weight :: Maximum hamming weight for the iterator
	Returns a heap pointer to the new iterator
*/
sym_iter* sym_iter_create_range(const unsigned length, const unsigned min_weight, const unsigned max_weight)
{
	// Allocate memory for the state iterator
	sym_iter* siter = (sym_iter*)malloc(sizeof(sym_iter));
	
	// Create the state currently occupied by the state iterator
	siter->state = sym_create(1, length);

	siter->length = length;
	siter->curr_weight = min_weight; // Our current weight
	siter->counter = 0; // Current counter state
	siter->max_counter = sym_iter_max_counter(length, min_weight); // Current maximum counter

	siter->max_weight = max_weight;

	return siter;
}

/*
	sym_iter_next:
	Updates the state of the sym iterator
	:: sym_iter* siter :: The iterator whose state is to be updated
	Returns a boolean value, true indicates that the iterator was updated, false indicates that the end of the range has been reached
*/
bool sym_iter_next(sym_iter* siter)
{
	if (siter->counter < siter->max_counter)
	{
		// Cast from iterator to long long
		long long val = sym_iter_ll_from_state(siter);

		// Bill Gosper Hamming Weight generator
		long long c = val & -val;
		long long r = val + c;
		
		val = (c != 0) ? (((r^val) >> 2) / c) | r : 0;
		
		// Push the result back to the state
		sym_iter_state_from_ll(siter, val);

		siter->counter++;
		return true;
	}
	else
	{
		if (siter->curr_weight < siter->max_weight)
		{
			siter->curr_weight++;
			siter->counter = 1; // Current counter state
			siter->max_counter = sym_iter_max_counter(siter->length, siter->curr_weight);

			long long val = (1ll << (long long)(siter->curr_weight)) - 1ll;
			sym_iter_state_from_ll(siter, val);
			return true;
		}
		else
		{
			return false;
		}
	}
}

/*
	sym_iter_ll_from_state:
	Casts the current state of the iterator to long long
	:: const sym_iter* siter :: The iterator whose state is to be cast
	Returns a long long representation of the state of the iterator
*/
long long sym_iter_ll_from_state(const sym_iter* siter)
{
	long long val = 0;
	for (int i = 0; i < sym_matrix_bytes(siter->state); i++)
	{
		val <<= 8ll;
		val += (BYTE)(siter->state->matrix[i]);
	}
	val >>= ((siter->length % 8) ? 8 - (siter->length % 8) : 0);
	return val;
}

/*
	sym_iter_state_from_ll:
	Casts from long long to the state of the iterator
	:: sym_iter* siter :: The iterator whose state is to be cast
	:: const long long val :: The long long val 
	Does not return anything, updates the value of the iterator in place.
*/
void sym_iter_state_from_ll(sym_iter* siter, long long val)
{
	val <<= ((siter->length % 8) ? 8 - (siter->length % 8) : 0);
	for (int i = sym_matrix_bytes(siter->state)- 1; i >= 0; i--)
	{
		siter->state->matrix[i] = (BYTE)(val);
		val >>= 8ll;
	}
	return;
}

/*
	sym_iter_left_shift:
	Shifts an entire sym array left by the specified number of bits
	:: sym_iter* siter :: The iterator to shift
	:: const unsigned shift :: The number of bits to shift by
	Does not return anything, value is modified in place.

void sym_iter_left_shift(sym_iter* siter, const unsigned shift)
{
	unsigned carry = 0;
	for (int i = 0; i < sym_matrix_bytes(siter->state); i++)
	{
		long long t_carry = siter->state->matrix[i] & (BYTE)(((unsigned) -1) - ((1 << (8 - shift)) - 1));
		siter->state->matrix[i] <<= shift;
		siter->state->matrix[i] += (BYTE)carry;

		carry = t_carry;
	}
	return;
}*/

/*
	sym_iter_right_shift:
	Shifts an entire sym array right by the specified number of bits
	:: sym_iter* siter :: The iterator to shift
	:: const unsigned shift :: The number of bits to shift by
	Does not return anything, value is modified in place.

void sym_iter_right_shift(sym_iter* siter, const unsigned shift)
{
	unsigned carry = 0;
	for (int i = 0; i >= 0; i--)
	{
		unsigned t_carry = siter->state->matrix[i] & (((1 << 8) - 1) - ((1 << shift) - 1));
		siter->state->matrix[i] <<= shift;
		siter->state->matrix[i] += carry;

		carry = t_carry;
	}
	return;
}*/

/*
	sym_iter_max_counter:
	Calculate (n, k) to determine the number of elements in the iterator with the same hamming weight 
	:: unsigned length::
	:: unsigned weight::
	Returns an unsigned long long containing the result
*/
unsigned long long sym_iter_max_counter(unsigned length, unsigned current_weight)
{
    long long result = 1;

    // If current_weight greater than length - weight then it's faster to calculate length - weight and it's symmetric!
    current_weight = current_weight > length - current_weight ? length - current_weight : current_weight;
   
    for (unsigned j = 1; j <= current_weight; j++, length--)
    {
        if (length % j == 0)
        {
            result *= length / j;
        } 
        else if (result % j == 0)
        {
            result = result / j * length;
        }
        else
        {
            result = (result * length) / j;
        }
    }
    return result;
}

/*
	sym_iter_free:
	Frees a symplectic iterator object
	:: sym_iter* siter :: Pointer to the object to be freed
	No return
*/
void sym_iter_free(sym_iter* siter)
{
	sym_free(siter->state);
	free(siter);
}

#endif