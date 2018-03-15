#ifndef CODEWORDS_FROM_STABILISER
#define CODEWORDS_FROM_STABILISER

#include "sym.h"
#include "circuit.h"


struct codeword_list 
{
	sym* codeword;
	codeword_list* next;
};


//sym* codewords_find(const sym* code, const sym* logicals);
void codeword_list_add(struct codeword_list* l, sym* codeword);
bool codeword_list_check(struct codeword_list* l, sym* codeword_candidate);
void codeword_list_free(struct codeword_list* l);
unsigned codeword_phase(sym* codeword_candidate);


// Given some initial codeword, find the rest of the codewords
sym* codewords_find(sym* basis,  sym* code, const sym* logicals)
{

	struct codeword_list* head = (struct codeword_list*)malloc(sizeof(struct codeword_list));
	head->codeword = basis; 
	unsigned n_codewords = 1;

	sym** stabilisers = (sym**)malloc(sizeof(sym*) * code->height);
	for (size_t i = 0; i < code->height; i++)
	{
		stabilisers[i] = sym_create(1, code->length);
		sym_row_copy(stabilisers[i], code, 0, i);
	}


	// Combinations of the stabilisers can be represented using a bitmask
	// Incrementing the mask is a new combination
	for (size_t bitmask = 0; bitmask < 1 << code->height; bitmask++)
	{
		unsigned phase = 0;
		sym* codeword_candidate = sym_copy(head->codeword);
		// Apply stabilisers to find other codewords
		for (size_t j = 0; j < code->height; j++)
		{
			if ( (1 << j) & bitmask)
			{
				sym_add_in_place(codeword_candidate,  stabilisers[j]);
				phase += codeword_phase(codeword_candidate);
			}
		}

			// Set the sign bit
		if (1 == phase % 2)
		{
			sym_set(codeword_candidate, 0, codeword_candidate->length/2, 1);
		}


		// If we don't already have this codeword, add it to the list
		if (codeword_list_check(head, codeword_candidate))
		{
			codeword_list_add(head, codeword_candidate);
			n_codewords++;
		}
	}

	// Copy the codewords to the sym object
	sym* codewords = sym_create(n_codewords, code->length);
	struct codeword_list* iter = head;
	for (size_t i = 0; i < n_codewords; i++)
	{	
		sym_row_copy(codewords, iter->codeword, i, 0);
		iter = iter->next;
	}

	for (size_t i = 0; i < code->height; i++)
	{
		sym_free(stabilisers[i]);
	}
	free(stabilisers);
	codeword_list_free(head);

	return codewords;
}

void codeword_list_add(struct codeword_list* l, sym* codeword)
{
	struct codeword_list* next = (codeword_list*)malloc(sizeof(codeword_list));
	next->next = l->next;
	l->next = next;
	next->codeword = codeword;
	return;
}

// Check if a new codeword already exists in the list
bool codeword_list_check(struct codeword_list* l, sym* codeword_candidate)
{
	for (struct codeword_list* iter = l; iter != NULL; iter = iter->next)
	{
		if (sym_to_ll(iter->codeword) == sym_to_ll(codeword_candidate))
		{
			return false;
		}
	}
	return true;
}

// Free a codeword list
void codeword_list_free(struct codeword_list* l)
{
	struct codeword_list* iter = l;
	while (iter != NULL)
	{
		struct codeword_list* tmp = iter;
		iter = iter->next;
		free(tmp);
	}
	return;
}

// Check the sign of the codeword
unsigned codeword_phase(sym* codeword_candidate)
{
	unsigned phase = 0;
	for (int i = 0 ; i < codeword_candidate->length / 2; i++)
	{
		if (sym_get(codeword_candidate, 0, i) && sym_get(codeword_candidate, 0, codeword_candidate->length / 2 + i))
		{
			phase++;
		}
		sym_set(codeword_candidate, 0, (codeword_candidate->length / 2) + i, 0);
	}
	return phase;
}


#endif