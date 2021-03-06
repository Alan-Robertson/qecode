#ifndef DMATRIXGEN
#define DMATRIXGEN
#include <iostream>
#include "Eigen/Dense"
#include "unsupported/Eigen/KroneckerProduct"
#include "math.h"
using Eigen::MatrixXcd;
using Eigen::VectorXcd;

MatrixXcd dmatrix_zeros(const unsigned long long height, const unsigned long long width);

MatrixXcd dmatrix_pauli_i()
{
    MatrixXcd m = MatrixXcd::Zero(2, 2);
    m(0,0).real(1);
    m(1,1).real(1);
    return m;
}

MatrixXcd dmatrix_pauli_x()
{
    MatrixXcd m = MatrixXcd::Zero(2, 2);
    m(1,0).real(1);
    m(0,1).real(1);
    return m;
}

MatrixXcd dmatrix_pauli_y()
{
    MatrixXcd m = MatrixXcd::Zero(2, 2);
    m(0,1).imag(-1.0);
    m(1,0).imag(1);
    return m;
}

MatrixXcd dmatrix_pauli_z()
{
    MatrixXcd m = MatrixXcd::Zero(2, 2);
    m(0,0).real(1);
    m(1,1).real(-1);
    return m;
}

MatrixXcd dmatrix_zeros(const unsigned long long height, const unsigned long long length)
{
    MatrixXcd m = MatrixXcd::Zero(height,length);
    return m;
}

MatrixXcd dmatrix_closure(MatrixXcd m)
{
    return m * m.adjoint();
}


MatrixXcd dmatrix_pauli_string(const char* str)
{
    MatrixXcd pi = dmatrix_pauli_i();
    MatrixXcd px = dmatrix_pauli_x();
    MatrixXcd py = dmatrix_pauli_y();
    MatrixXcd pz = dmatrix_pauli_z();

    MatrixXcd p = MatrixXcd::Zero(1, 1);
    p(0,0).real(1);

    const char* c = str;
    while (*c != '\0')
    {
        switch(*c)
        {
            case 'I':
                p = kroneckerProduct(p, pi).eval();
                break;

            case 'X':
                p = kroneckerProduct(p, px).eval();
                break;

            case 'Y':
                p = kroneckerProduct(p, py).eval();
                break;

            case 'Z':
                p = kroneckerProduct(p, pz).eval();
                break;

            default:
                std::cout << "Invalid pauli string!" << std::endl;
                return p;
        }
        c += 1;
    }
    return p;
}

MatrixXcd dmatrix_sym_to_matrix(const sym* s)
{
    MatrixXcd pi = dmatrix_pauli_i();
    MatrixXcd px = dmatrix_pauli_x();
    MatrixXcd py = dmatrix_pauli_y();
    MatrixXcd pz = dmatrix_pauli_z();
    
    MatrixXcd p = MatrixXcd::Zero(1,1);
    p(0,0).real(1);

    for (int i = 0; i < s->length / 2; i++)
    {
        switch(sym_get(s, 0, i) + (sym_get(s, 0, i + s->length / 2) << 1))
        {
            case 0:
                p = kroneckerProduct(p, pi).eval();
                break;
            case 1:
                p = kroneckerProduct(p, px).eval(); 
                break;
            case 2:
                p = kroneckerProduct(p, pz).eval(); 
                break;
            case 3:
                p = kroneckerProduct(p, py).eval(); 
                break;
        }
    }
    return p;
}

MatrixXcd dmatrix_vectorise(MatrixXcd* m)
{
    MatrixXcd m_bar = m->conjugate();
    return kroneckerProduct(*m, m_bar).eval();
}

/*
Eigen::VectorXcd dmatrix_state_from_vec(const std::vector<Eigen::VectorXcd> states)
{
    Eigen::VectorXcd k_state(1);
    k_state(0,0).real() = 1;
    for(std::vector<Eigen::VectorXcd>::iterator it = states.begin();
        it != states.end();
        it++;)
    {
        k_state = kroneckerProduct(k_state, *it).eval()
    }
}*/


#endif
