#ifndef SYMBLAS
#define SYMBLAS

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>



int main () {
    using namespace boost::numeric::ublas;
    
    unsigned u_state[] = {1 0 0 1};
    matrix<double> m_state (1, 4);

    for (int i = 0; i < 4; i++)
    {
    	m_state (1, i) = u_state[i];
    }

    std::cout << m_state << std::endl;


    matrix<double> m (4, 4);



    for (unsigned i = 0; i < m.size1 (); ++ i)
        for (unsigned j = 0; j < m.size2 (); ++ j)
            m (i, j) = 3 * i + j;
    std::cout << m << std::endl;
}

#endif