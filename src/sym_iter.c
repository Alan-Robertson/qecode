#include "sym_iter.h"

// ----------------------------------------------------------------------------------------
// FUNCTION DEFINITIONS
// ----------------------------------------------------------------------------------------

/* 
    sym_iter_create:
    Creates a new symplectic matrix object
    :: const unsigned length :: Length of the iterator in bits (2 * qubits)
    Returns a heap pointer to the new iterator
*/
sym_iter* sym_iter_create(const uint32_t length)
{
    // Because we iterate from 0 to the range specified
    // And because this consistently gets referenced by length = sym->length
    // And then decremented in the create range function
    // I really should have designed this a bit better in hindsight
    sym_iter* siter = sym_iter_create_range(length, 0, length + 1);
    return siter;
}

/* 
    sym_iter_create_n_qubits:
    Creates a new symplectic matrix object
    :: const unsigned length :: Length of the iterator in bits (2 * qubits)
    Returns a heap pointer to the new iterator
*/
sym_iter* sym_iter_create_n_qubits(const uint32_t n_qubits)
{
    sym_iter* siter = sym_iter_create_range(2 * n_qubits, 0, 2 * n_qubits + 1);
    return siter;
}


/* 
 *  sym_iter_create_n_qubits:
 *  Creates a new symplectic matrix object
 *  :: const unsigned length :: Length of the iterator in bits (2 * qubits)
 *  Returns a heap pointer to the new iterator
 */
sym_iter* sym_iter_create_n_qubits_range(const uint32_t n_qubits, const int32_t min_weight, const uint32_t max_weight)
{
    sym_iter* siter = sym_iter_create_range(2 * n_qubits, min_weight, 2 * max_weight + 1);
    return siter;
}


/* 
 *  sym_iter_create_range:
 *  Creates a new symplectic matrix object
 *  :: const unsigned length :: Length of the iterator in bits (2 * qubits)
 *  :: const unsigned min_weight :: Minimum hamming weight for the iterator
 *  :: const unsigned max_weight :: Maximum hamming weight for the iterator
 *  Returns a heap pointer to the new iterator
 */
sym_iter* sym_iter_create_range(const uint32_t length, const int32_t min_weight, const uint32_t max_weight)
{
    // Allocate memory for the state iterator
    sym_iter* siter = (sym_iter*)malloc(sizeof(sym_iter));
    
    // Create the state currently occupied by the state iterator
    siter->state = sym_create(1, length);
       
    siter->length = length;
    siter->curr_weight = min_weight - 1; // Our current weight

    // This is set up such that the first call to update will bump the
    // Iterator to the correct weight; it starts with one less than the required weight
    siter->ll_counter = sym_iter_max_ll_counter(length, min_weight - 1);
    siter->max_ll_counter = sym_iter_max_ll_counter(length, min_weight - 1); // Current maximum counter

    siter->max_weight = max_weight - 1;
    
    return siter;
}

/*
 *  sym_iter_next:
 *  Updates the state of the sym iterator
 *  :: sym_iter* siter :: The iterator whose state is to be updated
 *  Returns a boolean value, true indicates that the iterator was updated, false indicates that the end of the range has been reached
 */
uint8_t sym_iter_next(sym_iter* siter)
{
    if (siter->ll_counter < siter->max_ll_counter)
    {
        // Cast from iterator to long long
        long long val = sym_iter_ll_from_state(siter);

        // Bill Gosper Hamming Weight generator
        long long c = val & -val;
        long long r = val + c;
        
        val = (c != 0) ? (((r^val) >> 2) / c) | r : 0;
        
        // Push the result back to the state
        sym_iter_state_from_ll(siter, val);
            
        siter->ll_counter = val;
        return true;
    }
    else
    { // Update the hamming weight of the object
        if (siter->curr_weight < siter->max_weight)
        {
            siter->curr_weight++;
            // Generate the new string of 1s right alligned and of length equal to the current weight
            long long val = (1ll << (long long)(siter->curr_weight)) - 1ll;

            siter->ll_counter = val; // Current counter state
            siter->max_ll_counter = sym_iter_max_ll_counter(siter->length, siter->curr_weight);
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
 *  sym_iter_ll_from_state:
 *  Actually calculates the ll value from the state rather than just returning it
 *  :: const sym_iter* siter :: The iterator whose state is to be cast
 *  Returns a long long representation of the state of the iterator
 */
long long sym_iter_ll_from_state_calc(sym_iter* siter)
{
    long long val = 0;
    for (int i = 0; i < siter->state->mem_size; i++)
    {
        val <<= 8ll;
        val += (BYTE)(siter->state->matrix[i]);
    }
    val >>= ((siter->length % 8) ? 8 - (siter->length % 8) : 0);
    return val;
}

/*
 *  sym_iter_ll_from_state:
 *  Casts the current state of the iterator to long long
 *  :: const sym_iter* siter :: The iterator whose state is to be cast
 *  Returns a long long representation of the state of the iterator
 */
long long sym_iter_ll_from_state(const sym_iter* siter)
{    
    return siter->ll_counter;
}

/*
 *  sym_iter_state_from_ll:
 *  Casts from long long to the state of the iterator
 *  :: sym_iter* siter :: The iterator whose state is to be cast
 *  :: const long long val :: The long long val 
 *  Does not return anything, updates the value of the iterator in place.
 */
void sym_iter_state_from_ll(sym_iter* siter, long long val)
{
    val <<= ((siter->length % 8) ? 8 - (siter->length % 8) : 0);

    for (int i = siter->state->mem_size - 1; i >= 0; i--)
    {
        siter->state->matrix[i] = (BYTE)(val);
        val >>= 8ll;
    }

    return;
}

/*
    sym_iter_update:
    Updates the tracking parameters for the sym iterator
    Do this manually if you've changed the state without calling next
    :: sym_iter* siter :: The iterator whose state is to be cast 
    Does not return anything, updates the values of the iterator in place.
*/
void sym_iter_update(sym_iter* siter)
{
    siter->curr_weight = sym_weight_hamming(siter->state); // Determine the current weight
    siter->ll_counter = sym_iter_ll_from_state_calc(siter);
    siter->max_ll_counter = sym_iter_max_ll_counter(siter->length, siter->curr_weight);
    return;
}


/*
    sym_iter_max_ll_counter:
    Determine the maximum allowed state given the current weight and length before the weight needs to be increased 
    :: unsigned length::
    :: unsigned weight::
    Returns an unsigned long long containing the result
*/
long long sym_iter_max_ll_counter(uint32_t length, uint32_t current_weight)
{
    // Create the correctly weighted ll state
    if (current_weight == 0)
    {
        return 0;
    }

    long long unsigned result = (1 << current_weight) - 1;

    // Shift again till it's at the max weight;
    result <<= length - current_weight;

    return result;
}


/*
    sym_iter_value:
    Returns the long long representation of a symplectic matrix object
    :: sym_iter* siter :: Pointer to the iterator whose current value is to be determined
    Returns a long long value
*/
int64_t sym_iter_value(sym_iter* siter)
{   
    return sym_iter_ll_from_state(siter);
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


// Calculates binomial coefficients
/*
    sym_iter_max_counter:
    Calculate (n, k) to determine the number of elements in the iterator with the same hamming weight 
    :: unsigned length::
    :: unsigned weight::
    Returns an unsigned long long containing the result
*/
unsigned long long sym_iter_binom(unsigned length, unsigned weight)
{
    long long result = 1;
 
     // If current_weight greater than length - weight then it's faster to calculate length - weight and it's symmetric!
     weight = weight > length - weight ? length - weight : weight;
    
     for (unsigned j = 1; j <= weight; j++, length--)
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