#ifndef SYMPLECTIC
#define SYMPLECTIC

// LIBRARIES ----------------------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// MACROS -------------------------------------------------------------------------------------------

// Defines an effective "byte" type for bit field manipulation and readability
#define BYTE unsigned char

// Calculate the number of bytes required to store the symplectic matrix
#define MATRIX_BYTES(s) ((((s)->height) * ((s)->length)) % 8 ? (((s)->height) * ((s)->length) ) / 8 + 1 : (((s)->height) * ((s)->length)) / 8) 

// Given a matrix, find the byte containing the i, j'th element
#define BYTE_FROM_MATRIX(s, i, j) (((s)->length * (i) + (j)) / 8)

// Given a matrix, find the bit field offset for the i, j'th element
#define BIT_FROM_BYTE(s, i, j) (7 - ((s)->length * (i) + (j)) % 8)

// Takes an element from a sym matrix
#define ELEMENT_GET(s, i, j) (!!((s)->matrix[BYTE_FROM_MATRIX(s, i, j)] & (1 << BIT_FROM_BYTE(s, i, j))))

// Stores an element in a sym matrix
#define ELEMENT_SET(s, i, j, v) ((s)->matrix[BYTE_FROM_MATRIX((s), (i), (j))] = ((s)->matrix[BYTE_FROM_MATRIX((s), (i), (j))] & (~(1 << BIT_FROM_BYTE((s), (i), (j))))) ^ (v) << BIT_FROM_BYTE((s), (i), (j))) 

// Applies the XOR operator on an element in the matrix
#define ELEMENT_XOR(s, i, j, v) ((s)->matrix[BYTE_FROM_MATRIX((s), (i), (j))] ^= ((v) << BIT_FROM_BYTE((s), (i), (j))))

// STRUCT OBJECTS ----------------------------------------------------------------------------------------

/*
	sym:
	The symplectic matrix struct
	:: unsigned height :: Number of rows in the matrix
	:: unsigned length :: Number of columns in one block of the matrix (use SYM_LEN as a shorthand for the full number of columns)
	:: BYTE* matrix :: Points to the matrix on the heap
	:: size_t mem_size :: The number of bytes allocated in memory to the matrix object, useful for memcpy and memove
*/
typedef struct
{
	unsigned height;
	unsigned length;
	BYTE* matrix;
	size_t mem_size;
} sym;
// FOR SPEED: Replace the 1D array with an array of byte arrays to perform row operations faster

// FUNCTION DECLARATIONS ----------------------------------------------------------------------------------------
/* 
    sym_create:
	Creates a new symplectic matrix object
	:: const unsigned height :: Height of the matrix
	:: const unsigned length ::  length of the matrix
	Returns a heap pointer to the new matrix
*/
sym* sym_create(const unsigned height, const unsigned length);

/* 
    sym_create_valued:
	Creates a new symplectic matrix object
	:: const unsigned height :: Height of the matrix
	:: const unsigned length ::  length of the matrix
	:: const unsigned* values :: A 2D integer array of values to be stored
	Returns a heap pointer to the new matrix
*/
sym* sym_create_valued(const unsigned height, const unsigned length, const unsigned* values);

/* 
	sym_get:
	Returns the value of the matrix at that position
	:: const sym* s :: Pointer to the matrix in question
	:: const unsigned height :: Height of the element in question
	:: const unsigned length :: Length of the element in question
	Returns a boolean type with the required value
*/
BYTE sym_get(const sym*  s, const unsigned height, const unsigned length);

/* 
	sym_set:
	Saves the specified value to that position in the matrix
	:: sym* s :: Pointer to the matrix in question
	:: const unsigned height :: Height of the element in question
	:: const unsigned length :: Length of the element in question
	:: const BYTE value :: The value to be stored
	No return type
*/
void sym_set(sym* s, const unsigned height, const unsigned length, const BYTE value);

/*
	sym_add:
	Adds two symplectic matrices of the same size
	:: const sym* a :: One of the matrices to be added 
	:: const sym* b :: The other matrix
	Returns a heap pointer to a new matrix containing the result
	If the matrices are incompatible or NULL, this function returns NULL
*/
sym* sym_add(const sym* a, const sym*  b);

/*
	sym_add_in_place:
	Adds two symplectic matrices of the same size in place; the object 'a' will inherit the changes
	:: const sym* a :: One of the matrices to be added 
	:: const sym* b :: The other matrix
	No return type
*/
void sym_add_in_place(const sym* a, const sym* b);

/*
	sym_copy:
	Copies the contents of a sym object to a new sym object
	:: const sym* s :: The object to be copied 
	Returns a copy of the object
*/
sym* sym_copy(const sym* s);

/*
	sym_copy_in_place:
	Copies the contents of a sym object to another if the sizes match
	:: sym* d :: The object to be copied to
	:: const sym* s :: The object to be copied from
	Returns a copy of the object
*/
void sym_copy_in_place(sym* d, const sym* s);

/*
	sym_multiply:
	Multiplies two symplectic matrices of the appropriate size
	:: const sym* a :: One of the matrices to be multiplied
	:: const sym* b :: The other matrix
	Returns a heap pointer to a new matrix containing the result
	If the matrices are incompatible or NULL, this function returns NULL
*/
sym* sym_multiply(const sym* const a, const sym* const b);

/*
	sym_ancilla:
	Applies an error to a given code and returns the syndrome
	:: const sym* code :: The error correcting code being used
	:: const sym* error :: The error being applied
	Returns null if the error does not match the physical dimensions of the code, or if a pointer is invalid 
*/
sym* sym_sydrome(const sym* code, const sym* error);

/*
	sym_print:
	Prints a symplectic matrix object
	:: sym* s :: The object whose matrix is to be printed
	No return, prints to stdout
*/
void sym_print(const sym* s);

/*
	sym_free:
	Frees a symplectic matrix object
	:: sym* s :: Pointer to the object to be freed
	No return
*/
void sym_free(sym* s);

// FUNCTION DEFINITIONS ----------------------------------------------------------------------------------------
/* 
    sym_create:
	Creates a new symplectic matrix object
	:: unsigned height :: Height of the matrix
	:: unsigned length ::  length of the matrix
	Returns a heap pointer to the new matrix
*/
sym* sym_create(const unsigned height, const unsigned length)
{
	// Allocate heap memory for the new matrix
	sym* s = (sym*)malloc(sizeof(sym));
	// Store the height and length
	s->height = height;
	s->length = length;
	// Calculate the number of bytes required for the symplectic matrix representation
	// Storing this is faster than recalculating
	s->mem_size = MATRIX_BYTES(s);
	// Allocate the memory for this object
	s->matrix = (BYTE*)calloc(s->mem_size, sizeof(BYTE));
	return s;
}

/* 
    sym_create_valued:
	Creates a new symplectic matrix object
	:: unsigned height :: Height of the matrix
	:: unsigned length ::  length of the matrix
	:: const unsigned* values :: A 2D integer array of values to be stored
	Returns a heap pointer to the new matrix
*/
sym* sym_create_valued(const unsigned height, const unsigned length, const unsigned* values)
{
	sym* s = sym_create(height, length);
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < length; j++)
		{
			s->matrix[BYTE_FROM_MATRIX(s, i, j)] ^= (!!((BYTE)values[length * i + j])) << BIT_FROM_BYTE(s, i, j);
		}
	}
	return s;
}

/* 
	sym_get:
	Returns the value of the matrix at that position
	:: const sym* s :: Pointer to the matrix in question
	:: const unsigned height :: Height of the element in question
	:: const unsigned length :: Length of the element in question
	Returns a boolean type with the required value
*/
BYTE sym_get(const sym* s, const unsigned height, const unsigned length)
{
	return (BYTE) ELEMENT_GET(s, height, length);
}

/* 
	sym_get:
	Saves the specified value to that position in the matrix
	:: sym* s :: Pointer to the matrix in question
	:: const unsigned height :: Height of the element in question
	:: const unsigned length :: Length of the element in question
	:: const BYTE value :: The value to be stored
	No return type
*/
void sym_set(sym* s, const unsigned height, const unsigned length, const BYTE value)
{
	ELEMENT_SET(s, height, length, value);
	return;
}

/*
	sym_add:
	Adds two symplectic matrices of the same size
	:: const sym* a :: One of the matrices to be added 
	:: const sym* b :: The other matrix
	Returns a heap pointer to a new matrix containing the result
	If the matrices are incompatible or NULL, this function returns NULL
*/
sym* sym_add(const sym* a, const sym* b)
{
	// Check that the heights and lengths are valid before proceeding
	if (   a == NULL 
		|| b == NULL
		|| a->height != b->height 
		|| a->length != b->length)
	{
		printf("Incorrect Matrix Dimensions for Addition\n");
		return NULL;
	}

	// Create the matrix to store the result in
	sym* added = sym_create(a->height, a->length);

	// Calculate the result and store it
	for (size_t i = 0; i < added->mem_size; i++)
	{
		added->matrix[i] = a->matrix[i] ^ b->matrix[i];
	} 
	return added;
}

/*
	sym_add_in_place:
	Adds two symplectic matrices of the same size in place; the object 'a' will inherit the changes
	:: const sym* a :: One of the matrices to be added 
	:: const sym* b :: The other matrix
	No return type
*/
void sym_add_in_place(const sym* a, const sym* b)
{
	// Check that the heights and lengths are valid before proceeding
	if (   a == NULL 
		|| b == NULL
		|| a->height != b->height 
		|| a->length != b->length)
	{
		printf("Incorrect Matrix Dimensions for Addition\n");
		return;
	}

	// Calculate the result and store it
	for (size_t i = 0; i < a->mem_size; i++)
	{
		a->matrix[i] ^=  b->matrix[i];
	} 
	return;
}


/*
	sym_copy:
	Copies the contents of a sym object to a new sym object
	:: const sym* s :: The object to be copied 
	Returns a copy of the object
*/
sym* sym_copy(const sym* s)
{
	sym* s_copy = sym_create(s->height, s->length);
	memcpy(s_copy->matrix, s->matrix, s->mem_size);
	return s_copy;
}

/*
	sym_copy_in_place:
	Copies the contents of a sym object to another
	This is an unsafe function and should not be used unless the matrix sizes match
	:: const sym* s :: The object to be copied to
	:: const sym* s :: The object to be copied from
	Returns a copy of the object
*/
void sym_copy_in_place(sym* d, const sym* s)
{
	memcpy(d->matrix, s->matrix, s->mem_size);
}


unsigned sym_matrix_bytes(sym* s)
{
	return MATRIX_BYTES(s);
}

/*
	sym_multiply:
	Multiplies two symplectic matrices of the appropriate size
	:: const sym* a :: One of the matrices to be multiplied
	:: const sym* b :: The other matrix
	Returns a heap pointer to a new matrix containing the result
	If the matrices are incompatible or NULL, this function returns NULL
*/
sym* sym_multiply(const sym* a, const sym* b)
{
	// Check that the heights and lengths are valid before proceeding
	if ( a == NULL 
         || b == NULL
	     || a->length != b->height)
	{
		printf("Incorrect Matrix Dimensions for Multiplication\n");
		return NULL;
	}

	// Create the matrix to store the result
	sym* mult = sym_create(a->height, b->length);
	BYTE temp_byte = (BYTE) 0;

	// Calculate the result and store it
	for (size_t i = 0; i < a->height; i++)
	{
		for (size_t j = 0; j < b->length; j++) 
		{
			for (int k = 0; k < a->length; k++)
			{
				temp_byte ^= (ELEMENT_GET(a, i, k) & ELEMENT_GET(b, k, j));
			}
			ELEMENT_SET(mult, i, j, temp_byte);
			temp_byte = (BYTE) 0;
		}
	}
	return mult;
}

/*
	sym_syndrome:
	Applies an error to a given code and returns the syndrome
	:: const sym* code :: The error correcting code being used
	:: const sym* error :: The error being applied
	Returns null if the error does not match the physical dimensions of the code, or if a pointer is invalid 
	Else it returns a pointer to the syndrome
*/
		/* Matrix Multiplication version:
		sym* transpose = sym_transpose(error);
		sym* symplectic = sym_code_symplectic(error->length);
		sym* mult = sym_multiply(symplectic, transpose);
		sym* syndrome = sym_multiply(code, mult);

		sym_free(transpose);
		sym_free(symplectic);
		sym_free(mult);

		return syndrome;
	*/
sym* sym_syndrome(const sym* code, const sym* error)
{
	if ( code == NULL 
		|| error == NULL 
		|| error->length != code->length
		|| code->length % 2 != 0)
	{
		printf("Null pointer exception or matrices of incompatible sizes\n");
		return NULL;
	}

	const int half_length = code->length / 2;

	sym* syndrome = sym_create(code->height, 1);

	for (int i = 0; i <= error->length; i++)
	{
		if (ELEMENT_GET(error, 0, i)) // If there is no error on this qubit, skip it
		{
			for (int j = 0; j <= code->height; j++)
			{
				ELEMENT_XOR(syndrome, j, 0, ELEMENT_GET(code, j, (i + half_length) % code->length) & ELEMENT_GET(error, 0, i));
			}
		}
	}

	return syndrome;
}

/* 
sym_transpose:
Performs a transpose operation on a symplectic matrix object
:: sym* s :: A symplectic matrix to be transposed
Returns an address on the heap pointing to the transposed matrix
*/
sym* sym_transpose(const sym* s)
{
	sym* t = sym_create(s->length, s->height);
	for (size_t i = 0; i < s->height; i++)
	{
		for (size_t j = 0; j < s->length; j++)
		{
			sym_set(t, j, i, sym_get(s, i, j));
		}
	}
	return t;
}

/* sym_kron:
	Performs a Kronecker product on two sym matrix objects
*/
sym* sym_kron(sym* a, sym* b)
{
	if (!a || !b) // Either a or b is a null pointer
	{
		return NULL;
	}
	sym* kron = sym_create(a->height * b->height, a->length * b->length);

	for (size_t a_i = 0; a_i < a->height; a_i++)
	{
		for (size_t a_j = 0; a_j < a->length; a_j++)
		{
			if (ELEMENT_GET(a, a_i, a_j)) // Element is non zero
			{
				for (size_t b_i = 0; b_i < b->height; b_i++)
				{
					for (size_t b_j = 0; b_j < b->length; b_j++)
					{
						if (ELEMENT_GET(b, b_i, b_j))
						{
							ELEMENT_SET(kron, 
								a_i * b->height + b_i, 
								a_j * b->length + b_j, 
								1);
						}
					}
				}
			}
		}
	}
	return kron;
}


/*
	sym_weight:
	Returns the weight of a symplectic matrix object
	:: const sym* s :: Pointer to the object to be weighed
	Returns the weight as an unsigned integer
*/

BYTE sym_is_not_I(const sym* s, const unsigned i, const unsigned j){
	return ((ELEMENT_GET(s, i, j)) || ELEMENT_GET(s, i, j + s->length / 2));	
}

BYTE sym_is_I(const sym* s, const unsigned i, const unsigned j){
	return (!(ELEMENT_GET(s, i, j)) && !(ELEMENT_GET(s, i, j + s->length / 2)));	
}

BYTE sym_is_X(const sym* s, const unsigned i, const unsigned j){
	return (ELEMENT_GET(s, i, j) && !(ELEMENT_GET(s, i, j + s->length / 2)));	
}

BYTE sym_is_Y(const sym* s, const unsigned i, const unsigned j){
	return (ELEMENT_GET(s, i, j) && ELEMENT_GET(s, i, j + s->length / 2));	
}

BYTE sym_is_Z(const sym* s, const unsigned i, const unsigned j){
	return (!(ELEMENT_GET(s, i, j)) && ELEMENT_GET(s, i, j + s->length / 2));	
}


unsigned sym_weight_type_partial(const sym* s, const char type, unsigned start, unsigned end)
{
	unsigned weight = 0;
	BYTE (*is_pauli)(const sym* s, const unsigned i, const unsigned j);

	switch(type)
	{
		case 'I':
			is_pauli = sym_is_I;
			break;
		case 'X':
			is_pauli = sym_is_X;
			break;
		case 'Y':
			is_pauli = sym_is_Y;
			break;	
		case 'Z':
			is_pauli = sym_is_Z;
			break;
		case '\0':
			is_pauli = sym_is_not_I;
			break;
		default:
		printf("%c is not a recognised Pauli operator", type);
		return 0;
	}
	
	for (size_t i = 0; i < s->height; i++)
	{
		for (size_t j = start; j < end / 2; j++)
		{
			if (is_pauli(s, i, j))
			{
				weight++;
			}
		}
	}
	return weight;
}

/*
	sym_weight:
	Returns the weight of a symplectic matrix object
	:: const sym* s :: Pointer to the object to be weighed
	Returns the weight as an unsigned integer
*/
unsigned int sym_weight(const sym* s)
{
	return sym_weight_type_partial(s, '\0', 0, s->length);
}

/*
	sym_weight_X:
	Returns the weight of a symplectic matrix object, only counting X paulis
	:: const sym* s :: Pointer to the object to be weighed
	Returns the weight as an unsigned integer
*/
unsigned int sym_weight_X(const sym* s)
{
	return sym_weight_type_partial(s, 'X', 0, s->length);
}

/*
	sym_weight_Y:
	Returns the weight of a symplectic matrix object, only counting Y paulis
	:: const sym* s :: Pointer to the object to be weighed
	Returns the weight as an unsigned integer
*/
unsigned int sym_weight_Y(const sym* s)
{
	return sym_weight_type_partial(s, 'Y', 0, s->length);
}

/*
	sym_weight_Z:
	Returns the weight of a symplectic matrix object, only counting Z paulis
	:: const sym* s :: Pointer to the object to be weighed
	Returns the weight as an unsigned integer
*/
unsigned int sym_weight_Z(const sym* s)
{
	return sym_weight_type_partial(s, 'Z', 0, s->length);
}


void sym_row_copy(sym* s, const sym* t, const unsigned s_row, const unsigned t_row)
{
	for (size_t i = 0; i < s->length; i++)
	{
		sym_set(s, s_row, i, sym_get(t, t_row, i));
	}
}


unsigned long long sym_to_ll(const sym* s)
{
	if (s->mem_size > 8)
	{
		printf("Sym object is too large for a complete unsigned long long representation!\n Returning an approximation using the first 8 bytes.");
		printf("Change this object's mem_size to 8 or less to suppress this warning for an unsafe conversion\n");
		return (unsigned long long)s->matrix;
	}

	unsigned long long ll = 0;
	for (size_t i = 0; i < s->mem_size; i++)
	{
		ll <<= 8;
		ll += s->matrix[i];
	}
	ll >>= (((s->length * s->height) % 8) ? 8 - ((s->length * s->height) % 8) : 0);
	return ll;
}

sym* ll_to_sym(unsigned long long ll, const unsigned height, const unsigned length)
{
	if ((length * height) / 8 > 64)
	{
		printf("Requested sym object is too large to be filled by a single unsigned long long type\n");
		return NULL;	
	}

	sym* s = sym_create(height, length);
	ll <<= (((s->length * s->height) % 8) ? 8 - ((s->length * s->height) % 8) : 0);
	for (int i = s->mem_size - 1; i >= 0; i--)
	{
		s->matrix[i] = ll & 0xFF;
		ll >>= 8;
	}
	return s;
}

sym* ll_to_sym_t(unsigned long long ll, const unsigned height, const unsigned length)
{
	sym* s = ll_to_sym(ll, height, length);
	sym* t = sym_transpose(s);
	sym_free(s);
	return t;
}

/*
	sym_free:
	Frees a symplectic matrix object
	:: sym* s :: Pointer to the object to be freed
	No return
*/
void sym_free(sym* s)
{
	free(s->matrix);
	free(s);
	return;
}

/*
	sym_print:
	Prints a symplectic matrix object
	:: sym* s :: The object whose matrix is to be printed
	No return, prints to stdout
*/
void sym_print(const sym* s)
{
	for (size_t i = 0; i < s->height; i++) 
	{
		printf("[");
		for (size_t j = 0; j < s->length; j++)
		{
			printf("%d", (int)sym_get(s, i, j));
			if (s->length != 1 && s->height != 1 && j != s->length - 1) {
				if (j == (s->length / 2) - 1)
				{
					printf("|");
				} 
				else
				{
					printf(" ");
				}
			}
		}
		printf("]\n");
	}
	return;
}

#endif