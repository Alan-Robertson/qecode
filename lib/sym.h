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

#endif