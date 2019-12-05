#ifndef SYMPLECTIC
#define SYMPLECTIC

// ----------------------------------------------------------------------------------------
// DIRECTIVES
// ----------------------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>

// ----------------------------------------------------------------------------------------
// MACROS
// ----------------------------------------------------------------------------------------

// Defines an effective "byte" type for bit field manipulation and readability
#define BYTE unsigned char

// Calculate the number of bytes required to store the symplectic matrix
#define MATRIX_BYTES(s) ((((s)->height) * ((s)->length)) % 8 ? ((((s)->height) * ((s)->length) ) / 8) + 1 : (((s)->height) * ((s)->length)) / 8) 

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

// ----------------------------------------------------------------------------------------
// STRUCTS
// ----------------------------------------------------------------------------------------

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
    unsigned n_qubits;
    BYTE* matrix;
    size_t mem_size;
} sym;

// ----------------------------------------------------------------------------------------
// FUNCTION DECLARATIONS 
// ----------------------------------------------------------------------------------------

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
    sym_xor:
    Xors the value with the bit at the specified position and saves it
    :: sym* s :: Pointer to the matrix in question
    :: const unsigned height :: Height of the element in question
    :: const unsigned length :: Length of the element in question
    :: const BYTE value :: The value to be xored
    No return type
*/
void sym_xor(sym* s, const unsigned height, const unsigned length, const BYTE value);

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
    sym_partial_add:
    Adds two symplectic matrices that are not of the same size
    :: const sym* a :: One of the matrices to be added 
    :: const sym* b :: The other matrix
    :: const unsigned* target_bits :: The mapping from each bit in b to the corresponding bit in a
    Returns a heap pointer to a new matrix containing the result
    If the matrices are incompatible or NULL, this function returns NULL
*/
sym* sym_partial_add(const sym* a, const sym* b, const unsigned* target_bits);

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
 *  sym_ancilla:
 *  Applies an error to a given code and returns the syndrome
 *  :: const sym* code :: The error correcting code being used
 *  :: const sym* error :: The error being applied
 *  Returns null if the error does not match the physical dimensions of the code, or if a pointer is invalid 
 */
sym* sym_sydrome(const sym* code, const sym* error);

/*
 * sym_row_commutation
 * Determines if two rows in two sym objects commute
 * :: const sym* a :: The first sym object
 * :: const sym* b :: The second sym object
 * :: const row_a :: The row from the first sym object
 * :: const row_b :: The row from the second sym object
 * Returns an unsigned value 0 or 1, where 0 indicates that the rows commute, while 1
 * Indicates that they do not commute.
 */
unsigned sym_row_commutes(
    const sym* a, 
    const sym* b, 
    const unsigned row_a, 
    const unsigned row_b);

/*
 * sym_row_column_commutes
 * Determines if one row and one column commute
 * :: const sym* a :: The first sym object
 * :: const sym* b :: The second sym object
 * :: const row_a :: The row from the first sym object
 * :: const column_b :: The column from the second sym object
 * Returns an unsigned value 0 or 1, where 0 indicates that the rows commute, while 1
 * Indicates that they do not commute.
 */
unsigned sym_row_column_commutes(
    const sym* a, 
    const sym* b, 
    const unsigned row_a, 
    const unsigned column_b);

/* 
 * sym_transpose:
 * Performs a transpose operation on a symplectic matrix object
 * :: sym* s :: A symplectic matrix to be transposed
 * Returns an address on the heap pointing to the transposed matrix
 */
sym* sym_transpose(const sym* s); 

/* 
 * sym_row_xor:
 * XORs two rows together of the same symplectic matrix
 * :: sym* s :: A symplectic matrix 
 * :: const unsigned control :: The row to be xored into another 
 * :: const unsigned target :: The row that will be xored into
 * Operation occurs in place
 */
void sym_row_xor(sym* s, const unsigned control, const unsigned target);

/* 
 * sym_column_swap:
 * Swaps two columns of a symplectic matrix
 * :: sym* s :: A symplectic matrix 
 * :: const unsigned col_ :: The first column to be swapped
 * :: const unsigned col_b :: The second column to be swapped
 * Operation occurs in place
 */
void sym_column_swap(sym* code, const unsigned col_a, const unsigned col_b);

/*
 * sym_is_not_I
 * Checks whether the value of a gate on qubit i in a sym object is not the identity
 * :: const sym* s :: The sym object being checked
 * :: const unsigned i :: The column within the sym object
 * :: const unsigned j :: The row within the sym object
 * Returns a single byte containing either 0 if it is I, or 1 if it is not I
 */
BYTE sym_is_not_I(const sym* s, const unsigned i, const unsigned j);

/*
 * sym_is_I
 * Checks whether the value of a gate on qubit i in a sym object is the identity
 * :: const sym* s :: The sym object being checked
 * :: const unsigned i :: The column within the sym object
 * :: const unsigned j :: The row within the sym object
 * Returns a single byte containing either 1 if it is I, or 0 if it is not I 
 */
BYTE sym_is_I(const sym* s, const unsigned i, const unsigned j);

/*
 * sym_is_X
 * Checks whether the value of a gate on qubit i in a sym object is X
 * :: const sym* s :: The sym object being checked
 * :: const unsigned i :: The column within the sym object
 * :: const unsigned j :: The row within the sym object
 * Returns a single byte containing either 1 if it is X, or 0 if it is not X 
 */
BYTE sym_is_X(const sym* s, const unsigned i, const unsigned j);

/*
 * sym_is_Y
 * Checks whether the value of a gate on qubit i in a sym object is Y
 * :: const sym* s :: The sym object being checked
 * :: const unsigned i :: The column within the sym object
 * :: const unsigned j :: The row within the sym object
 * Returns a single byte containing either 1 if it is Y, or 0 if it is not Y 
 */
BYTE sym_is_Y(const sym* s, const unsigned i, const unsigned j);

/*
 * sym_is_Z
 * Checks whether the value of a gate on qubit i in a sym object is Z
 * :: const sym* s :: The sym object being checked
 * :: const unsigned i :: The column within the sym object
 * :: const unsigned j :: The row within the sym object
 * Returns a single byte containing either 1 if it is Z, or 0 if it is not Z 
 */
BYTE sym_is_Z(const sym* s, const unsigned i, const unsigned j);

/*
 * sym_anticommutes_X
 * Checks whether the paulis applied to a particular qubit anti-commute with pauli X
 * :: const sym* s :: The sym object being checked
 * :: const unsigned i :: The column within the sym object
 * :: const unsigned j :: The row within the sym object
 * Returns a single byte containing either 1 if it is Y or Z, or 0 if it is X or I 
 */
BYTE sym_anticommutes_X(const sym* s, const unsigned i, const unsigned j);

/*
 * sym_anticommutes_Y
 * Checks whether the paulis applied to a particular qubit anti-commute with pauli Y
 * :: const sym* s :: The sym object being checked
 * :: const unsigned i :: The column within the sym object
 * :: const unsigned j :: The row within the sym object
 * Returns a single byte containing either 1 if it is X or Z, or 0 if it is Y or I 
 */
BYTE sym_anticommutes_Y(const sym* s, const unsigned i, const unsigned j);

/*
 * sym_anticommutes_Z
 * Checks whether the paulis applied to a particular qubit anti-commute with pauli Z
 * :: const sym* s :: The sym object being checked
 * :: const unsigned i :: The column within the sym object
 * :: const unsigned j :: The row within the sym object
 * Returns a single byte containing either 1 if it is X or Y, or 0 if it is Z or I 
 */
BYTE sym_anticommutes_Z(const sym* s, const unsigned i, const unsigned j);

/*
 *  sym_weight_type_partial:
 *  Returns the weight of a symplectic matrix object with specified bounds, this function is wrapped by the 
 *  other weight functions and serves as a more generic type
 *  :: const sym* s :: Pointer to the object to be weighed
 *  :: const char type :: The type of weight being checked, either I, X, Y, Z or \0
 *  :: unsigned start :: The start of where the weight is being checked
 *  :: unsigned end :: The end of where the weight is being checked
 *  Returns the weight as an unsigned integer
 */
uint32_t sym_weight_type_partial(const sym* s, const char type, unsigned start, unsigned end);

/*
 *  sym_weight:
 *  Returns the weight of a symplectic matrix object
 *  :: const sym* s :: Pointer to the object to be weighed
 *  Returns the weight as an unsigned integer
 */
uint32_t sym_weight(const sym* s);

/*
 *  sym_weight_X:
 *  Returns the weight of a symplectic matrix object, only counting X paulis
 *  :: const sym* s :: Pointer to the object to be weighed
 *  Returns the weight as an unsigned integer
 */
uint32_t sym_weight_X(const sym* s);

/*
 *  sym_weight_Y:
 *  Returns the weight of a symplectic matrix object, only counting Y paulis
 *  :: const sym* s :: Pointer to the object to be weighed
 *  Returns the weight as an unsigned integer
 */
uint32_t sym_weight_Y(const sym* s);

/*
 *  sym_weight_Z:
 *  Returns the weight of a symplectic matrix object, only counting Z paulis
 *  :: const sym* s :: Pointer to the object to be weighed
 *  Returns the weight as an unsigned integer
 */
uint32_t sym_weight_Z(const sym* s);

/*
 * sym_row_copy
 * Copies a row from one sym object to a row on another sym object
 * :: sym* s :: The sym object to be copied to
 * :: const sym* t :: The sym object to be copied from
 * :: const unsigned s_row :: The row to be copied to
 * :: const unsigned t_row :: The row to be copied from
 * The operation happens in place and nothing is returned
 */
void sym_row_copy(sym* s, const sym* t, const unsigned s_row, const unsigned t_row);

/*
    sym_sym_to_sym_non_varg:
    Copies a state between two sym objects with differing numbers of qubits
    :: sym* target :: Target to be copied to
    :: sym const* control :: The sym object to be copied from
    :: uint32_t* target_qubits :: The list of qubits to be copied to
    :: uint32_t* control_qubits :: The list of qubits to be copied from
    The operation is performed in place, returns nothing
*/
void sym_sym_to_sym_non_varg(sym* target, sym const* control, uint32_t* target_qubits, uint32_t* control_qubits);

/*
    sym_sym_to_sym_non_varg:
    Copies a state between two sym objects with differing numbers of qubits
    Can also be used to relabel qubits
    :: sym* target :: Target to be copied to
    :: sym const* control :: The sym object to be copied from
    :: uint32_t* target_qubits :: The list of qubits to be copied to
    :: uint32_t* control_qubits :: The list of qubits to be copied from
    The operation is performed in place, returns nothing
*/
void sym_sym_to_sym(sym* target, sym const* control, ...);

/*
    sym_to_ll:
    Provides a long long representation of a sym object, useful for indexing
    :: const sym* s :: Pointer to the sym object to be represented
    Returns an unsigned long long
*/
long long sym_to_ll(const sym* s);
/*
    ll_to_sym:
    Given a long long representation of a sym object, constructs the sym object
    :: unsigned long long ll :: The long long that is to be used to build the sym object
    :: const unsigned height :: height of the sym object
    :: const unsigned length :: length of the sym object
    Returns a sym object
*/
sym* ll_to_sym(unsigned long long ll, const unsigned height, const unsigned length);

/*
    ll_to_sym:
    Given a long long representation of a sym object, constructs the sym object
    :: unsigned long long ll :: The long long that is to be used to build the sym object
    :: const unsigned height :: height of the sym object
    :: const unsigned n_qubits :: length of the sym object in qubits
    Returns a sym object
*/
sym* ll_to_sym_n_qubits(unsigned long long ll, const unsigned height, const unsigned n_qubits);

/*
    ll_to_sym_t:
    Given a long long representation of a sym object, constructs the sym object transposed
    :: unsigned long long ll :: The long long that is to be used to build the sym object
    :: const unsigned height :: height of the sym object
    :: const unsigned length :: length of the sym object
    Returns a sym object
*/
sym* ll_to_sym_t(unsigned long long ll, const unsigned height, const unsigned length);

/*
    sym_print:
    Prints a symplectic matrix object
    :: sym* s :: The object whose matrix is to be printed
    No return, prints to stdout
*/
void sym_print(const sym* s);


/*
    sym_print_pauli:
    Prints a symplectic matrix object as a series of Paulis
    :: sym* s :: The object whose matrix is to be printed
    No return, prints to stdout
*/
void sym_print_pauli(const sym* s);

/*
    sym_clear:
    Clears the matrix of a symplectic matrix object
    :: sym* s :: Pointer to the symplectic matrix to be cleared
    No return
*/
void sym_clear(sym* s);

/*
    sym_free:
    Frees a symplectic matrix object
    :: sym* s :: Pointer to the object to be freed
    No return
*/
void sym_free(sym* s);

// ----------------------------------------------------------------------------------------
// FUNCTION DEFINITIONS
// ----------------------------------------------------------------------------------------

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
    s->n_qubits = length / 2;
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
    for (int32_t i = 0; i < height; i++)
    {
        for (int32_t j = 0; j < length; j++)
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
    sym_get_X:
    Returns the value of a given qubit in the X block
    :: const sym* s :: Pointer to the matrix in question
    :: const unsigned height :: Height of the element in question
    :: const unsigned qubit_index :: The qubit being queried
    Returns a BYTE type with the required value
*/
BYTE sym_get_X(const sym* s, const unsigned height, const unsigned qubit_index)
{
    return sym_get(s, height, qubit_index);
}

/* 
    sym_get_Z:
    Returns the value of a given qubit in the Z block
    :: const sym* s :: Pointer to the matrix in question
    :: const unsigned height :: Height of the element in question
    :: const unsigned qubit_index :: The qubit being queried
    Returns a BYTE type with the required value
*/
BYTE sym_get_Z(const sym* s, const unsigned height, const unsigned qubit_index)
{
    return sym_get(s, height, qubit_index + s->n_qubits);
}

/* 
    sym_set:
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
    sym_set_I:
    Saves the specified value to that position in the matrix
    :: sym* s :: Pointer to the matrix in question
    :: const unsigned height :: Height of the element in question
    :: const unsigned length :: Length of the element in question
    :: const BYTE value :: The value to be stored
    No return type
*/
void sym_set_I(sym* s, const unsigned height, const unsigned qubit_index)
{
    ELEMENT_SET(s, height, qubit_index, 0);
    ELEMENT_SET(s, height, qubit_index + s->n_qubits, 0);
    return;
}


/* 
    sym_set_X:
    Saves the specified value to that position in the matrix
    :: sym* s :: Pointer to the matrix in question
    :: const unsigned height :: Height of the element in question
    :: const unsigned length :: Length of the element in question
    :: const BYTE value :: The value to be stored
    No return type
*/
void sym_set_X(sym* s, const unsigned height, const unsigned qubit_index, const BYTE value)
{
    sym_set(s, height, qubit_index, value);
    return;
}

/* 
    sym_set_Z:
    Saves the specified value to that position in the matrix
    :: sym* s :: Pointer to the matrix in question
    :: const unsigned height :: Height of the element in question
    :: const unsigned length :: Length of the element in question
    :: const BYTE value :: The value to be stored
    No return type
*/
void sym_set_Z(sym* s, const unsigned height, const unsigned qubit_index, const BYTE value)
{
    ELEMENT_SET(s, height, qubit_index + s->n_qubits, value);
    return;
}

/* 
    sym_xor:
    Xors the value with the bit at the specified position and saves it
    :: sym* s :: Pointer to the matrix in question
    :: const unsigned height :: Height of the element in question
    :: const unsigned length :: Length of the element in question
    :: const BYTE value :: The value to be xored
    No return type
*/
void sym_xor(sym* s, const unsigned height, const unsigned length, const BYTE value)
{
    ELEMENT_XOR(s, height, length, value);
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
    sym_partial_add:
    Adds two symplectic matrices that are not of the same size
    Adds the second sym object to a copy of the first, the target bits is a map from the ith element in b to the target[i]th element in a
    This preserves relations between X and Z bits, it is not agnostic to them.
    :: const sym* a :: One of the matrices to be added 
    :: const sym* b :: The other matrix
    :: const unsigned* target_bits :: The mapping from each bit in b to the corresponding bit in a
    Returns a heap pointer to a new matrix containing the result
    If the matrices are incompatible or NULL, this function returns NULL
*/
sym* sym_partial_add(const sym* a, const sym* b, const unsigned* target_bits)
{
    // Check that the heights and lengths are valid before proceeding
    if (   a == NULL 
        || b == NULL
        || target_bits == NULL
        || a->height != b->height)
    {
        printf("Incorrect Matrix Dimensions for Partial Addition\n");
        return NULL;
    }   
    sym* added = sym_copy(a);

    for (size_t i = 0; i < added->height; i++)
    {
        // For each element within b, add it to the corresponding qubit in a
        for (size_t j = 0; j < b->n_qubits; j++)
        {
            // Add the X components
            sym_set_X(added, i, target_bits[j], sym_get_X(added, i, target_bits[j]) ^ sym_get_X(b, i, j));

            // Add the Z components
            sym_set_Z(added, i, target_bits[j], sym_get_Z(added, i, target_bits[j]) ^ sym_get_Z(b, i, j));
        }
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
            for (int32_t k = 0; k < a->length; k++)
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
    
    // Also the number of qubits
    const int32_t half_length = code->n_qubits;

    sym* syndrome = sym_create(code->height, 1);
    for (int32_t i = 0; i < error->length; i++)
    {
        if (ELEMENT_GET(error, 0, i)) // If there is no error on this qubit, skip it
        {
            for (int32_t j = 0; j < syndrome->height; j++)
            {
                ELEMENT_XOR(syndrome, j, 0, ELEMENT_GET(code, j, (i + code->n_qubits) % code->length) & ELEMENT_GET(error, 0, i % code->length));
            }
        }
    }
    return syndrome;
}

/*
 * sym_row_commutes
 * Determines if two rows in two sym objects commute
 * :: const sym* a :: The first sym object
 * :: const sym* b :: The second sym object
 * :: const row_a :: The row from the first sym object
 * :: const row_b :: The row from the second sym object
 * Returns an unsigned value 0 or 1, where 0 indicates that the rows commute, while 1
 * Indicates that they do not commute.
 */
unsigned sym_row_commutes(
    const sym* a, 
    const sym* b, 
    const unsigned row_a, 
    const unsigned row_b)
{
    // Check the validity of the arguments
    if (a == NULL 
        || b == NULL 
        || a->length != b->length
        || a->height <= row_a
        || b->height <= row_b)
    {
        printf("Incorrect sizes of sym objects.\n");
        return 2;
    }

    unsigned commutes = 0;

    for (size_t i = 0; i < a->length; i++)
    {
        commutes += sym_get(a, row_a, i) & sym_get(b, row_b, (i + a->n_qubits) % a->length);
    }
    return commutes % 2;
}

/*
 * sym_row_column_commutes
 * Determines if one row and one column commute
 * :: const sym* a :: The first sym object
 * :: const sym* b :: The second sym object
 * :: const row_a :: The row from the first sym object
 * :: const column_b :: The column from the second sym object
 * Returns an unsigned value 0 or 1, where 0 indicates that the rows commute, while 1
 * Indicates that they do not commute.
 */
unsigned sym_row_column_commutes(
    const sym* a, 
    const sym* b, 
    const unsigned row_a, 
    const unsigned column_b)
{
    // Check the validity of the arguments
    if (a == NULL 
        || b == NULL 
        || a->length != b->height
        || a->height <= row_a
        || b->length <= column_b)
    {
        printf("sym_row_column_commutes: Incorrect sizes of sym objects.\n");
        printf("Pointers: [%p %p]\n", a, b);
        printf("Heights Match: [%u %u]\n", a->length, b->height);
        printf("Coordinate Checks: [%u %u] [%u %u]\n", a->height, row_a, b->length, column_b);
        return 2;
    }

    unsigned commutes = 0;

    for (size_t i = 0; i < a->length; i++)
    {
        commutes += sym_get(a, row_a, i) & sym_get(b, (i + a->n_qubits) % a->length, column_b);
    }
    return commutes % 2;
}

/* 
 * sym_transpose:
 * Performs a transpose operation on a symplectic matrix object
 * :: sym* s :: A symplectic matrix to be transposed
 * Returns an address on the heap pointing to the transposed matrix
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

/* 
 * sym_row_xor:
 * XORs two rows together of the same symplectic matrix
 * :: sym* s :: A symplectic matrix 
 * :: const unsigned control :: The row to be xored into another 
 * :: const unsigned target :: The row that will be xored into
 * Operation occurs in place
 */
void sym_row_xor(sym* s, const unsigned control, const unsigned target)
{
    for (uint32_t i = 0; i < s->length; i++)
    {
        sym_xor(s, target, i, sym_get(s, control, i));
    }
    return;
}

/* 
 * sym_column_swap:
 * Swaps two columns of a symplectic matrix
 * :: sym* s :: A symplectic matrix 
 * :: const unsigned col_ :: The first column to be swapped
 * :: const unsigned col_b :: The second column to be swapped
 * Operation occurs in place
 */
void sym_column_swap(sym* code, const unsigned col_a, const unsigned col_b)
{
    // Check that the columns are distinct
    if (col_a == col_b)
    {
        return;
    }

    // XOR swap the two bits
    for (uint32_t i = 0; i < code->height; i++)
    {
        sym_xor(code, i, col_a, sym_get(code, i, col_b));
        sym_xor(code, i, col_b, sym_get(code, i, col_a));
        sym_xor(code, i, col_a, sym_get(code, i, col_b));
    }
    return;
}

/*
 * sym_is_not_I
 * Checks whether the value of a gate on qubit i in a sym object is not the identity
 * :: const sym* s :: The sym object being checked
 * :: const unsigned i :: The column within the sym object
 * :: const unsigned j :: The row within the sym object
 * Returns a single byte containing either 0 if it is I, or 1 if it is not I
 */
BYTE sym_is_not_I(const sym* s, const unsigned i, const unsigned j)
{
    return ((ELEMENT_GET(s, i, j)) || ELEMENT_GET(s, i, j + s->n_qubits));  
}

/*
 * sym_is_I
 * Checks whether the value of a gate on qubit i in a sym object is the identity
 * :: const sym* s :: The sym object being checked
 * :: const unsigned i :: The column within the sym object
 * :: const unsigned j :: The row within the sym object
 * Returns a single byte containing either 1 if it is I, or 0 if it is not I 
 */
BYTE sym_is_I(const sym* s, const unsigned i, const unsigned j)
{
    return (!(ELEMENT_GET(s, i, j)) && !(ELEMENT_GET(s, i, j + s->n_qubits)));  
}

/*
 * sym_is_X
 * Checks whether the value of a gate on qubit i in a sym object is X
 * :: const sym* s :: The sym object being checked
 * :: const unsigned i :: The column within the sym object
 * :: const unsigned j :: The row within the sym object
 * Returns a single byte containing either 1 if it is X, or 0 if it is not X 
 */
BYTE sym_is_X(const sym* s, const unsigned i, const unsigned j)
{
    return (ELEMENT_GET(s, i, j) && !(ELEMENT_GET(s, i, j + s->n_qubits))); 
}

/*
 * sym_is_Y
 * Checks whether the value of a gate on qubit i in a sym object is Y
 * :: const sym* s :: The sym object being checked
 * :: const unsigned i :: The column within the sym object
 * :: const unsigned j :: The row within the sym object
 * Returns a single byte containing either 1 if it is Y, or 0 if it is not Y 
 */
BYTE sym_is_Y(const sym* s, const unsigned i, const unsigned j)
{
    return (ELEMENT_GET(s, i, j) && ELEMENT_GET(s, i, j + s->n_qubits));    
}

/*
 * sym_is_Z
 * Checks whether the value of a gate on qubit i in a sym object is Z
 * :: const sym* s :: The sym object being checked
 * :: const unsigned i :: The column within the sym object
 * :: const unsigned j :: The row within the sym object
 * Returns a single byte containing either 1 if it is Z, or 0 if it is not Z 
 */
BYTE sym_is_Z(const sym* s, const unsigned i, const unsigned j)
{
    return (!(ELEMENT_GET(s, i, j)) && ELEMENT_GET(s, i, j + s->n_qubits)); 
}

/*
 * sym_anticommutes_X
 * Checks whether the paulis applied to a particular qubit anti-commute with pauli X
 * :: const sym* s :: The sym object being checked
 * :: const unsigned i :: The column within the sym object
 * :: const unsigned j :: The row within the sym object
 * Returns a single byte containing either 1 if it is Y or Z, or 0 if it is X or I 
 */
BYTE sym_anticommutes_X(const sym* s, const unsigned i, const unsigned j)
{
    return (BYTE)(sym_is_Z(s, i, j) || sym_is_Y(s, i, j));
}

/*
 * sym_anticommutes_Y
 * Checks whether the paulis applied to a particular qubit anti-commute with pauli Y
 * :: const sym* s :: The sym object being checked
 * :: const unsigned i :: The column within the sym object
 * :: const unsigned j :: The row within the sym object
 * Returns a single byte containing either 1 if it is X or Z, or 0 if it is Y or I 
 */
BYTE sym_anticommutes_Y(const sym* s, const unsigned i, const unsigned j)
{
    return (BYTE)(sym_is_Z(s, i, j) || sym_is_X(s, i, j));
}

/*
 * sym_anticommutes_Z
 * Checks whether the paulis applied to a particular qubit anti-commute with pauli Z
 * :: const sym* s :: The sym object being checked
 * :: const unsigned i :: The column within the sym object
 * :: const unsigned j :: The row within the sym object
 * Returns a single byte containing either 1 if it is X or Y, or 0 if it is Z or I 
 */
BYTE sym_anticommutes_Z(const sym* s, const unsigned i, const unsigned j)
{
    return (BYTE)(sym_is_X(s, i, j) || sym_is_Y(s, i, j));
}

/*
    sym_weight_type_partial:
    Returns the weight of a symplectic matrix object with specified bounds, this function is wrapped by the 
    other weight functions and serves as a more generic type
    :: const sym* s :: Pointer to the object to be weighed
    :: const char type :: The type of weight being checked, either I, X, Y, Z or \0
    :: unsigned start :: The start of where the weight is being checked
    :: unsigned end :: The end of where the weight is being checked
    Returns the weight as an unsigned integer
*/
uint32_t sym_weight_type_partial(const sym* s, const char type, unsigned start, unsigned end)
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
    sym_weight_hamming:
    Returns the classical hamming weight of a symplectic matrix object with specified bounds, this function is wrapped by the 
    other weight functions and serves as a more generic type
    :: const sym* s :: Pointer to the object to be weighed
    Returns the weight as an unsigned integer
*/
uint32_t sym_weight_hamming(const sym* s)
{
    unsigned weight = 0;
    
    for (size_t i = 0; i < s->height; i++)
    {
        for (size_t j = 0; j < s->length; j++)
        {
            weight += sym_get(s, i, j);
        }
    }
    return weight;
}

/*
 *  sym_weight:
 *  Returns the weight of a symplectic matrix object
 *  :: const sym* s :: Pointer to the object to be weighed
 *  Returns the weight as an unsigned integer
 */
uint32_t sym_weight(const sym* s)
{
    return sym_weight_type_partial(s, '\0', 0, s->length);
}

/*
 *  sym_weight_X:
 *  Returns the weight of a symplectic matrix object, only counting X paulis
 *  :: const sym* s :: Pointer to the object to be weighed
 *  Returns the weight as an unsigned integer
 */
uint32_t sym_weight_X(const sym* s)
{
    return sym_weight_type_partial(s, 'X', 0, s->length);
}

/*
 *  sym_weight_Y:
 *  Returns the weight of a symplectic matrix object, only counting Y paulis
 *  :: const sym* s :: Pointer to the object to be weighed
 *  Returns the weight as an unsigned integer
 */
uint32_t sym_weight_Y(const sym* s)
{
    return sym_weight_type_partial(s, 'Y', 0, s->length);
}

/*
 *  sym_weight_Z:
 *  Returns the weight of a symplectic matrix object, only counting Z paulis
 *  :: const sym* s :: Pointer to the object to be weighed
 *  Returns the weight as an unsigned integer
 */
uint32_t sym_weight_Z(const sym* s)
{
    return sym_weight_type_partial(s, 'Z', 0, s->length);
}


/*
 * sym_row_copy
 * Copies a row from one sym object to a row on another sym object
 * :: sym* s :: The sym object to be copied to
 * :: const sym* t :: The sym object to be copied from
 * :: const unsigned s_row :: The row to be copied to
 * :: const unsigned t_row :: The row to be copied from
 * The operation happens in place and nothing is returned
 */
void sym_row_copy(sym* s, const sym* t, const unsigned s_row, const unsigned t_row)
{
    if (s->length != t->length)
    {
        printf("Rows in sym_row_copy operation are not the same length\n");
        return;
    }
    for (size_t i = 0; i < s->length; i++)
    {
        sym_set(s, s_row, i, sym_get(t, t_row, i));
    }
}

/*
    sym_sym_to_sym_non_varg:
    Copies a state between two sym objects with differing numbers of qubits
    :: sym* target :: Target to be copied to
    :: sym const* control :: The sym object to be copied from
    :: uint32_t* target_qubits :: The list of qubits to be copied to
    :: uint32_t* control_qubits :: The list of qubits to be copied from
    The operation is performed in place, returns nothing
*/
void sym_sym_to_sym_non_varg(sym* target, sym const* control, uint32_t* target_qubits, uint32_t* control_qubits)
{
    // The number of target qubits is the lesser of the number of control or target qubits
    uint32_t n_target_qubits = target->length < control->length ? target->length : control->length;

    for (int i = 0; i < n_target_qubits; i++)
    {
        for (int j = 0; j < target->height; j++)
        {
            sym_set(target, j, target_qubits[i], sym_get(control, j, control_qubits[i])); // Copy X elements
            sym_set(target, j, target_qubits[i] + target->n_qubits, sym_get(control, j, control_qubits[i] + control->n_qubits)); // Copy Z elements
        }
    }
    return;
}


/*
    sym_sym_to_sym_non_varg:
    Copies a state between two sym objects with differing numbers of qubits
    Can also be used to relabel qubits
    :: sym* target :: Target to be copied to
    :: sym const* control :: The sym object to be copied from
    :: uint32_t* target_qubits :: The list of qubits to be copied to
    :: uint32_t* control_qubits :: The list of qubits to be copied from
    The operation is performed in place, returns nothing
*/
void sym_sym_to_sym(sym* target, sym const* control, ...)
{
    // Create the va_list
    va_list args;
    va_start(args, control);
    // Allocate memory for the target qubit array
    unsigned* target_qubits = (unsigned*)malloc(sizeof(unsigned) * target->n_qubits);
    unsigned* control_qubits = (unsigned*)malloc(sizeof(unsigned) * control->n_qubits);

    // For each qubit that we are expecting, take the next variadic argument and copy it to the array
    for (unsigned i = 0; i < target->n_qubits; i++)
    {
        target_qubits[i] = va_arg(args, uint32_t);
    }

    for (unsigned i = 0; i < control->n_qubits; i++)
    {
        control_qubits[i] = va_arg(args, uint32_t);
    }
    
    // Add the new gate using the regular function
    sym_sym_to_sym_non_varg(target, control, target_qubits, control_qubits);

    free(target_qubits);
    free(control_qubits);
    return;
}

/*
    sym_resize:
    Copies a state between two sym objects with differing numbers of qubits
    :: sym* target :: Target to be copied to
    :: sym const* control :: The sym object to be copied from
    :: uint32_t* target_qubits :: The list of qubits to be copied to
    :: uint32_t* control_qubits :: The list of qubits to be copied from
    Returns nothing
*/
sym* sym_resize(sym const* s, uint32_t n_qubits)
{
    sym* resized = sym_create(s->height, n_qubits * 2);
    uint32_t n_to_copy = s->length < n_qubits ? s->length : n_qubits;

    for (int i = 0; i < n_to_copy; i++)
    {
        for (int j = 0; j < s->height; j++)
        {
            sym_set(resized, j, i, sym_get(s, j, i)); // Copy X elements
            sym_set(resized, j, i + resized->n_qubits, sym_get(s, j, i + s->n_qubits)); // Copy Z elements
        }
    }

    return resized;
}

/*
    sym_to_ll:
    Provides a long long representation of a sym object, useful for indexing
    :: const sym* s :: Pointer to the sym object to be represented
    Returns an unsigned long long
*/
long long sym_to_ll(const sym* s)
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

/*
    ll_to_sym:
    Given a long long representation of a sym object, constructs the sym object
    :: unsigned long long ll :: The long long that is to be used to build the sym object
    :: const unsigned height :: height of the sym object
    :: const unsigned length :: length of the sym object
    Returns a sym object
*/
sym* ll_to_sym(unsigned long long ll, const unsigned height, const unsigned length)
{
    if ((length * height) / 8 > 64)
    {
        printf("Requested sym object is too large to be filled by a single unsigned long long type\n");
        return NULL;    
    }

    sym* s = sym_create(height, length);
    ll <<= (((s->length * s->height) % 8) ? 8 - ((s->length * s->height) % 8) : 0);
    for (int32_t i = s->mem_size - 1; i >= 0; i--)
    {
        s->matrix[i] = ll & 0xFF;
        ll >>= 8;
    }
    return s;
}

/*
    ll_to_sym:
    Given a long long representation of a sym object, constructs the sym object
    :: unsigned long long ll :: The long long that is to be used to build the sym object
    :: const unsigned height :: height of the sym object
    :: const unsigned n_qubits :: length of the sym object in qubits
    Returns a sym object
*/
sym* ll_to_sym_n_qubits(unsigned long long ll, const unsigned height, const unsigned n_qubits)
{
    return ll_to_sym(ll, height, n_qubits * 2);
}

/*
    ll_to_sym_t:
    Given a long long representation of a sym object, constructs the sym object transposed
    :: unsigned long long ll :: The long long that is to be used to build the sym object
    :: const unsigned height :: height of the sym object
    :: const unsigned length :: length of the sym object
    Returns a sym object
*/
sym* ll_to_sym_t(unsigned long long ll, const unsigned height, const unsigned length)
{
    sym* s = ll_to_sym(ll, height, length);
    sym* t = sym_transpose(s);
    sym_free(s);
    return t;
}

/*
    sym_is_empty:
    Checks if the total weight of a sym object is zero
    :: const sym* s :: The sym object to check
    Returns true if there are no paulis on the sym object, or false otherwise
*/
uint32_t sym_is_empty(const sym* s)
{
    for (uint32_t i = 0; i < s->mem_size; i++)
    {
        if (s->matrix[i] != 0)
        {
            return false;
        }
    }
    return true;
}

/*
    sym_clear:
    Clears the matrix of a symplectic matrix object
    :: sym* s :: Pointer to the symplectic matrix to be cleared
    No return
*/
void sym_clear(sym* s)
{
    memset(s->matrix, 0, s->mem_size);
    return;
}


/* 
    sym_multi_free:
    Frees a vargs collection of sym objects
    :: const size_t n_gates :: The number of sym objects to be freed
    :: VARGS :: The pointers to the sym objects to be freed
    Does not return anything
*/
void sym_multi_free(const size_t n_sym_objects, ...)
{

    va_list args;
    va_start(args, n_sym_objects);

    for (size_t i = 0; i < n_sym_objects; i++)
    {
        sym_free(va_arg(args, sym*));
    }
    va_end(args);

    return;
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
                if (j == (s->n_qubits) - 1)
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


/*
    sym_print_pauli:
    Prints a symplectic matrix object
    :: sym* s :: The object whose matrix is to be printed
    No return, prints to stdout
*/
void sym_print_pauli(const sym* s)
{
    for (size_t i = 0; i < s->height; i++) 
    {
        printf("[");
        for (size_t j = 0; j < s->length / 2; j++)
        {
            switch(
                ((int)sym_get(s, i, j) << 1)
                + (int)sym_get(s, i, j + s->n_qubits)) 
            {
                case 0:
                    printf("I");
                    break;

                case 1:
                    printf("Z");
                    break;

                case 2:
                    printf("X");
                    break;

                case 3:
                    printf("Y");
                    break;
            }
        }
        printf("]\n");
    }
    return;
}

#endif