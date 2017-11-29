#include "sym.h"

typedef struct {
	sym* state; // The current state of our sym iterator
	unsigned length;
	unsigned max_weight; // The maximum weight we will be testing
	unsigned curr_weight; // Our current weight
	long long counter; // Current counter state
	long long max_counter; // Current maximum counter 
} sym_iter; 

// Calculates binomial coefficients
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


sym_iter* sym_iter_create_range(unsigned length, unsigned min_weight, unsigned max_weight)
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

sym_iter* sym_iter_create(unsigned length)
{
	sym_iter* siter = sym_iter_create_range(length, 0, length);
}


// Update the state of the iterator
bool sym_iter_next(sym_iter* siter)
{
		if (siter->counter < siter->max_counter)
		{
			// Bill Gosper Hamming Weight generator
			long long val = sym_iter_ll_from_state(sym_iter* siter);
			c = val & -val;
			r = val + c;
			val = (((r^val) >> 2) / c) | r;

			sym_iter_state_from_ll(siter, val);

			siter->counter++;
			return true;
		}
		else
		{
			if (siter->curr_weight < siter->max_weight)
			{
				siter->curr_weight++;
				siter->counter = 0; // Current counter state
				siter->max_counter = sym_iter_max_counter(siter->length, siter->curr_weight);
				return true;
			}
			else
			{
				return false;
			}
		}
}

long long sym_iter_ll_from_state(sym_iter* siter)
{
	long long val = 0;
	for (int i = 0; i < siter->length / 8; i++)
	{
		val <<= 8;
		val += BYTE(siter->state->matrix[i]);
	}
	printf("%lld", val);
	return val;
}

void sym_iter_state_from_ll(sym_iter* siter, long long val)
{
	for (int i = 0; i < siter->length / 8; i++)
	{
		val <<= 8;
		val += int(siter->state->matrix[i]);
	}
	return;
}