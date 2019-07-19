#ifndef CODES_CANDIDATE
#define CODES_CANDIDATE
#include "../sym.h"

// Some candidate random codes for seven qubits that seem to perform reasonably well 

sym* code_candidate_seven_a()
{
        unsigned candidate[] = {
                1, 0, 0, 0, 1, 1, 1,   1, 1, 0, 0, 0, 0, 1,
                0, 1, 0, 1, 1, 0, 1,   1, 1, 0, 0, 0, 0, 1,
                0, 0, 1, 1, 0, 1, 0,   1, 1, 0, 0, 0, 0, 1,
                0, 0, 0, 0, 0, 0, 0,   0, 1, 1, 1, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 1, 0, 1,
                0, 0, 0, 0, 0, 0, 0,   1, 0, 1, 0, 0, 1, 0
        };
        sym* s = sym_create_valued(6, 14, candidate);
        return s;
}

sym* code_candidate_seven_a_logicals()
{
        unsigned candidate_logicals[] = 
        {
                0, 0,
                0, 0,
                0, 0,
                0, 0,
                1, 0,
                0, 0,
                1, 0,
                1, 1,
                1, 1,
                1, 0,
                0, 0,
                0, 0,
                0, 0,
                0, 1
        };
        sym* s = sym_create_valued(14, 2, candidate_logicals);
        return s;
}


sym* code_candidate_seven_b()
{
        unsigned candidate[] = {
                1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1,
                0, 1, 0, 1, 1, 0, 1, 1, 1, 0, 0, 0, 0, 1,
                0, 0, 1, 1, 0, 1, 0, 1, 1, 0, 0, 0, 0, 1,
                0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1,
                0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0
        };
        sym* s = sym_create_valued(6, 14, candidate);
        return s;
}

sym* code_candidate_seven_b_logicals()
{
        unsigned candidate_logicals[] = 
        {
                0, 0,
                0, 0,
                0, 0,
                0, 0,
                1, 0,
                0, 0,
                1, 0,
                1, 1,
                1, 1,
                1, 0,
                0, 0,
                0, 0,
                0, 0,
                0, 1
        };
        sym* s = sym_create_valued(14, 2, candidate_logicals);
        return s;
}

sym* code_candidate_seven_c()
{
        unsigned candidate[] = {
                1,0,0,0,1,0,0 ,0,1,1,0,0,0,1,
                0,1,0,0,1,1,1 ,0,0,0,0,0,0,0,
                0,0,1,1,1,1,1 ,0,0,0,0,0,0,0,
                0,0,0,0,0,0,0 ,0,0,1,1,0,0,0,
                0,0,0,0,0,0,0 ,1,1,1,0,1,0,0,
                0,0,0,0,0,0,0 ,0,0,0,0,0,1,1
        };
        sym* s = sym_create_valued(6, 14, candidate);
        return s;
}

sym* code_candidate_seven_c_logicals()
{
        unsigned candidate_logicals[] = 
        {
                0 ,0,
                0 ,0,
                0 ,0,
                0 ,0,
                0 ,0,
                1 ,0,
                1 ,0,
                1 ,0,
                0 ,1,
                0 ,1,
                0 ,0,
                0 ,0,
                0 ,0,
                0 ,1
        };
        sym* s = sym_create_valued(14, 2, candidate_logicals);
        return s;
}
#endif