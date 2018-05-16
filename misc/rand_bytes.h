#ifndef RAND_BYTES
#define RAND_BYTES

#include <stdio.h>
#include <stdlib.h>
#include <time.h>


// STRUCT OBJECTS ----------------------------------------------------------------------------------------

/*
	rand_bytes:
	Struct to generate and consume random random bytes, this allows the re-use of random bytes and an arbitrary number
	of random bytes to be created without concerns for how to package the bytes
	:: unsigned consumed :: The number of random bytes that have already been used
	:: unsigned total :: The total number of random bytes that have been allocated
	:: BYTE* bytes :: The random bytes in question
	This object should be freed using the 'rand_bytes_free' function
*/
typedef struct 
{
	unsigned consumed;
	unsigned total;
	BYTE* bytes;
} rand_bytes;

// FUNCTION DECLARATIONS ----------------------------------------------------------------------------------------

/* 
    rand_bytes_create:
	Creates a new chunk of random bytes
	:: const unsigned n_bytes :: Total number of bytes to be allocated
	Returns a rand_bytes object with no bytes consumed
*/
rand_bytes rand_bytes_create(const unsigned n_bytes);

/* 
    rand_bytes_consume:
	Consumes the specified number of random bytes
	:: rand_bytes b :: The rand bytes object to be used
	:: const unsigned n_bytes :: Total number of bytes to be consumed
	Returns a BYTE pointer to the start of the region of memory that the bytes are to be taken from
*/
BYTE* rand_bytes_consume(rand_bytes b, const unsigned n_bytes);

/* 
    rand_bytes_free:
	Frees a rand_bytes object
	:: rand_bytes b :: The rand bytes object to be freed
	No object returned
*/
void rand_bytes_free(rand_bytes b);

// FUNCTION DEFINITIONS ----------------------------------------------------------------------------------------

/* 
    rand_bytes_create:
	Creates a new chunk of random bytes
	:: const unsigned n_bytes :: Total number of bytes to be allocated
	Returns a rand_bytes object with no bytes consumed
*/
rand_bytes rand_bytes_create(const unsigned n_bytes)
{
	rand_bytes b;
	b.consumed = 0;
	b.total = n_bytes;
	b.bytes = (BYTE*)malloc(sizeof(BYTE) * n_bytes);

	for (size_t i = 0; i < b.total; i++)
	{
		b.bytes[i] = (BYTE)rand();
	}
	return b;
}

/* 
    rand_bytes_consume:
	Consumes the specified number of random bytes
	:: rand_bytes b :: The rand bytes object to be used
	:: const unsigned n_bytes :: Total number of bytes to be consumed
	Returns a BYTE pointer to the start of the region of memory that the bytes are to be taken from
*/
BYTE* rand_bytes_consume(rand_bytes b, const unsigned n_bytes)
{
	if (n_bytes + b.consumed > b.total)
	{
		printf("Too many bytes requested!\n");
		return NULL;
	}
	else
	{
		b.consumed += n_bytes;
		return b.bytes + (b.consumed - n_bytes);
	}
}

/* 
    rand_bytes_free:
	Frees a rand_bytes object
	:: rand_bytes b :: The rand bytes object to be freed
	No object returned
*/
void rand_bytes_free(rand_bytes b)
{
	free(b.bytes);
}


#endif