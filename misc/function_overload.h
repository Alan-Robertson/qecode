#include <stdio.h>
#include <stdarg.h>

#define STR_VAL(A) #A

#define CAT(A, B) CAT2(A, B)
#define CAT2(A, B) A ## B

#define COUNT_PARMS2(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _, ...) _
#define COUNT_PARMS(...)\
	COUNT_PARMS2(__VA_ARGS__, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1)
