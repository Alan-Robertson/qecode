#include "errors.h"

/* 
    error_create_simple:
    Creates a simple 1 qubit error
    :: const char pauli :: What error is being created, takes 'I', 'X', 'Y' or 'Z'
    :: const unsigned qubit :: Which qubit the error is being applied on
    :: const sym* code :: The code the error is being applied to
    Returns a sym* object representing the error string
*/
sym* error_create_simple(const char pauli, const unsigned qubit, const sym* code)
{
    if ( code == NULL || qubit > code->length / 2) 
    {
        return NULL;
    }

    sym* error = sym_create(1, code->length);
    switch(pauli)
    {
        case 'I':
        break;

        case 'X':
            sym_set(error, 0, qubit, 1);
        break;

        case 'Z':
            sym_set(error, 0, qubit + (code->length / 2), 1);
        break;

        case 'Y':
            sym_set(error, 0, qubit, 1);
            sym_set(error, 0, qubit + (code->length / 2), 1);
        break;

        default:
        printf("Not a valid Pauli operator, please use I, X, Y or Z");
        return NULL;
    }
    return error;
}

/* 
    error_add:
    Adds an error to an existing error string
    :: const char pauli :: What error is being created, takes 'I', 'X', 'Y' or 'Z'
    :: const unsigned qubit :: Which qubit the error is being applied on
    :: sym* error :: The error that this error is being added to
    Nothing returned, however modifies the contents of error
*/
void error_add(const char pauli, const unsigned qubit, sym* error)
{
    if ( error == NULL || qubit > error->length / 2) {
        return;
    }

    switch(pauli)
    {
        case 'I':
        break;

        case 'X':
            sym_set_Z(error, 0, qubit, 1);
        break;

        case 'Z':
            sym_set_X(error, 0, qubit, 1);
        break;

        case 'Y':
            sym_set_X(error, 0, qubit, 1);
            sym_set_Z(error, 0, qubit, 1);
        break;

        default:
        printf("Not a valid Pauli operator, please use I, X, Y or Z");
        return;
    }
    return;
}

sym* error_str_to_sym(const char* pauli_string)
{
    int len = strlen(pauli_string);

    sym* error = sym_create(1, len * 2);

    for (int i = 0; i < len; i++){
        error_add(pauli_string[i], i, error);
    }
    return error;
}

bool error_inc(char* error, const size_t len)
{
    
    switch(error[len - 1]) 
    {
        case 'I':
            error[len - 1] = 'X';
            return true;
        break;

        case 'X':
            error[len - 1] = 'Y';
            return true;
        break;

        case 'Y':
            error[len - 1] = 'Z';
            return true;
        break;

        case 'Z':
            if (len) // len is not 0
            {
                error[len - 1] = 'I';
                return error_inc(error, len - 1);
            } 
            else 
            {
                return false;
            }
        break;
    }
    return false;
}

char* error_sym_to_str(const sym* error)
{
    if (error == NULL) {
        return NULL;
    }
    char* error_string = (char*)malloc(sizeof(char) * error->length / 2 + 1);

    for (int i = 0; i < error->length / 2; i++)
    {
        bool X = sym_get(error, 0, i);
        bool Z = sym_get(error, 0, i + error->length / 2);

        if (!X && !Z) // I
        {
            error_string[i] = 'I';
        }

        if (X && !Z) // X ERROR
        {
            error_string[i] = 'X';
        }

        if (X && Z) // Y ERROR
        {
            error_string[i] = 'Y';
        }

        if (!X && Z) // Z ERROR
        {
            error_string[i] = 'Z';
        }
    }
    error_string[error->length / 2] = '\0';
    return error_string;
}

int error_str_to_int(const char* error_string, const int len)
{
    int base = 1;
    int sum = 0;
    for (int i = 0; i < len; i++)
    {
        switch(error_string[i])
        {
            case 'I':
                sum += 0 * base;
            break;
            case 'X':
                sum += 1 * base;
            break;
            case 'Y':
                sum += 2 * base;
            break;
            case 'Z':
                sum += 3 * base;
            break;
        }
        base *= 4;
    }   
    return sum;
}

// Create an error directly from an integer
sym* error_int_to_sym(const unsigned* integer_arr, const size_t len)
{
    const int n_bytes = (!(len % 8)) ? (len / 8) : (len / 8) + 1;
    sym* error = sym_create(1, len);
    memcpy(error->matrix, integer_arr, n_bytes);
    return error;
}
