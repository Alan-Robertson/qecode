#ifndef Q_ERRORS
#define Q_ERRORS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define M_GET(q_matrix_ptr, h, l) ((q_matrix_ptr)->matrix[(h) * (q_matrix_ptr)->length + (l)])
#define M_GET_PTR(q_matrix_ptr, h, l) ((q_matrix_ptr)->matrix + ((h) * (q_matrix_ptr)->length + (l)))
#define M_SET(q_matrix_ptr, h, l, value) (M_GET(q_matrix_ptr, h, l) = (value))

#ifndef Q_HIGH_DIMS

// Q_VALUE ----------------------------------------------------------------------

typedef struct 
{
	double real;
	double imag;
} q_value;

q_value hermitian(q_value q)
{
	q_value a;
	a.real = q.real;
	a.imag = q.imag * -1;
	return a;
}

q_value q_value_multiply(q_value a, q_value b)
{
	q_value q;
	q.real = a.real * b.real - a.imag * b.imag;
	q.imag = a.imag * b.real + b.imag * a.real;
	return q;
}

q_value q_value_square(q_value a)
{
	q_value q;
	q.real = a.real * a.real - a.imag * a.imag;
	q.imag = 2 * a.imag * a.real;
	return q;
}

q_value q_value_add(q_value a, q_value b)
{
	q_value q;
	q.real = a.real + b.real;
	q.imag = a.imag + b.imag;
	return q;
}

void q_value_print(q_value a)
{
	printf("[%f + %fi]", a.real, a.imag);
}

// Q_PURE_STATE -----------------------------------------------------------------------

struct q_pure_state
{
	struct q_pure_state* next;	
	q_value prob;
	unsigned len_basis;
	unsigned* basis_state;
};
typedef struct q_pure_state q_pure_state;

q_pure_state* create_pure_state(q_value prob, unsigned* basis_state, unsigned len_basis)
{
	q_pure_state* p = (q_pure_state*)malloc(sizeof(q_pure_state));
	p->prob = prob; 
	p->len_basis = len_basis;
	p->basis_state = (unsigned*)malloc(sizeof(unsigned) * len_basis);
	memcpy(p->basis_state, basis_state, sizeof(unsigned) * len_basis);
	p->next = NULL;
	return p;
}

void q_pure_state_print(q_pure_state* p)
{
	q_value_print(p->prob);
	printf("[");
	for (int i = 0; i < p->len_basis; i++)
	{
		printf("%d", p->basis_state[i]);
	}
	printf("]\n");
}


void free_pure_state(q_pure_state* p)
{
	free(p->basis_state);
	free(p);
}


// Q_STATE -----------------------------------------------------------------------

typedef struct 
{
	unsigned n_dimensions;
	unsigned n_basis_states;
	unsigned len_basis;
	q_pure_state* start;
	q_pure_state* end;
} q_state;

q_state* create_q_state(unsigned n_dimensions)
{
	q_state* s = (q_state*)malloc(sizeof(q_state));
	s->n_dimensions = n_dimensions;
	s->len_basis = 1u << n_dimensions; // 2 ^ n_dimensions
	s->n_basis_states = 0;
	s->start = NULL;
	s->end = NULL;
	return s;
}

void q_state_print(q_state* q)
{
	for (q_pure_state* iter = q->start; iter != NULL; iter = iter->next)
	{
		q_pure_state_print(iter);	
	}
}


void free_q_state(q_state* q)
{
	q_pure_state* p = q->start;
	while(p != NULL)
	{
		q_pure_state* next = p->next;
		free_pure_state(p);
		p = next;
	}
	free(q);
}


void add_basis(q_state* state, q_pure_state* basis_state)
{
	
	if (state->start == NULL)
	{
		state->start = basis_state;
		state->end = basis_state;
	}
	else
	{
		state->end->next = basis_state;
		state->end = basis_state;
	}
	state->n_basis_states += 1;
}


void create_and_add_basis(q_state* q, q_value prob, unsigned* basis_state)
{
	q_pure_state* p = create_pure_state(prob, basis_state, q->len_basis);
	add_basis(q, p);
}


// Q_MATRIX ----------------------------------------------------------------

typedef struct 
{
	unsigned length;
	unsigned height;
	q_value* matrix; 
} q_matrix;

q_matrix* create_q_matrix(unsigned height, unsigned length)
{
	q_value* matrix = (q_value*)calloc(height * length, sizeof(q_value));
	q_matrix* m = (q_matrix*)malloc(sizeof(q_matrix));
	m->matrix = matrix;
	m->height = height;
	m->length = length;
	return m;
}

void free_matrix(q_matrix* m)
{
	free(m->matrix);
	free(m);
}

q_matrix* state_inner_product(q_pure_state* q, q_pure_state* p)
{
	q_matrix* product_matrix = create_q_matrix(q->len_basis, q->len_basis);
	for (int i = 0; i < q->len_basis; i++)
	{
		if (q->basis_state[i] != 0)
			{
			for (int j = 0; j < q->len_basis; j++)
			{
				if (p->basis_state[j] != 0){
					M_SET(product_matrix, i, j, q_value_multiply(q->prob,p->prob));
				}
			}
		}
	}
	return product_matrix;	
}

q_matrix* q_matrix_add(const q_matrix* a, const q_matrix* b)
{
	// Check that the height and length of a and b are equal
	q_matrix* addition_matrix = create_q_matrix(a->height,a->length);
	for (int i = 0; i < a->height; i++)
	{
		for (int j = 0; j < a->length; j++)
		{
			M_SET(addition_matrix, i, j, q_value_add(M_GET(a, i, j), M_GET(b, i, j)));
		}
	}
	return addition_matrix;
}


q_matrix* kraus_operator(const q_state* q)
{
	q_pure_state q_iter_dag;
	q_iter_dag.len_basis = q->len_basis;

	q_matrix* kraus_matrix = create_q_matrix(q->len_basis, q->len_basis);

	for (q_pure_state* q_iter = q->start; q_iter != NULL; q_iter = q_iter->next)
		{	
			q_iter_dag.prob = hermitian(q_iter->prob);
			q_iter_dag.basis_state = q_iter->basis_state;
			
			q_matrix* inner_product = state_inner_product(q_iter, &q_iter_dag);
			q_matrix* temp = q_matrix_add(kraus_matrix, inner_product);
			
			free_matrix(kraus_matrix);
			free_matrix(inner_product);
	
			kraus_matrix = temp;
		}

	return kraus_matrix;
}

q_matrix* pauli_channel(const q_state** krauss_matricies)
{
	q_pure_state q_iter_dag;
	q_iter_dag.len_basis = q->len_basis;

	q_matrix* kraus_matrix = create_q_matrix(q->len_basis, q->len_basis);

	for (q_pure_state* q_iter = q->start; q_iter != NULL; q_iter = q_iter->next)
		{	
			q_iter_dag.prob = hermitian(q_iter->prob);
			q_iter_dag.basis_state = q_iter->basis_state;
			
			q_matrix* inner_product = state_inner_product(q_iter, &q_iter_dag);
			q_matrix* temp = q_matrix_add(kraus_matrix, inner_product);
			
			free_matrix(kraus_matrix);
			free_matrix(inner_product);
	
			kraus_matrix = temp;
		}

	return kraus_matrix;
}


void matrix_print(const q_matrix* kraus)
{
	for (int i = 0; i < kraus->height; i++)
	{
		printf("[");
		for (int j = 0; j < kraus->length; j++)
		{
			q_value_print(M_GET(kraus, i, j));
		}
		printf("]\n");
	}
	return;
}


/*

// Q_DENSITY_OPERATOR ---------------------------------------------------


typedef struct 
{
	q_matrix* density_matrix;	
} q_density_operator;



// Q_KRAUS ----------------------------------------------------------------

typedef struct 
{
	unsigned n_operators;
	q_matrix** operators;
} q_kraus;


kraus_paulis* create_kraus(unsigned n_operators, unsigned matrix_height, unsigned matrix_length)
{
	kraus_paulis* k = (kraus_paulis*)malloc(sizeof(kraus_paulis));
	q_matrix** operators = (q_matrix**)malloc(sizeof(q_matrix*) * n_operators);

	k->operators = operators;

	for (int i = 0; i < n_operators; i++)
	{
		q_matrix[i] = create_q_matrix(matrix_height, matrix_length);
	}
	return k;
}

void free_kraus(kraus_paulis* k)
{
	for (int i = 0; i < k->n_operators; k++)
	{
		free_matrix(k->operators[i]);
	}
	free(k->operators);
	free(k);
}

typedef struct 
{
	unsigned dims;	
} choi_jamiolkowski;


q_state* create_q_state_valued(q_value* probabilities, unsigned** basis_states, unsigned n_states, unsigned len_states)
{
	q_state* s = (q_state*)malloc(sizeof(q_state));
	s->probs = (q_value*)malloc(sizeof(q_value) * n_states);
	s->basis_states = (unsigned**)malloc(sizeof(unsigned*) * n_states);


	for (int i = 0; i < n_states; i++)
	{
		s->basis_states[i] = (unsigned*)malloc(sizeof(unsigned*) * n_states);
		memcpy(s->basis_states[i], basis_states[i], len_states * sizeof(unsigned));
	}

	return s;
}



void calc_density_operator(q_state* q, unsigned n_states)
{
	for (int i = 0; i < q->len; i++)
	{

	}

}





void free_state(q_state* s)
{
	free(s->probs);
	free(s);
}




*/

#else

	// TODO: Use C bignum or similar for higher dimensional spaces

#endif
#endif