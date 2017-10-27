#include "errors.h"

int main()
{
	q_value p = {0.5, 0};
	unsigned basis_u[] = {1, 0};
	unsigned basis_d[] = {0, 1};

	q_state* s = create_q_state(1);
	create_and_add_basis(s, p, basis_u);
	create_and_add_basis(s, p, basis_d);
	q_matrix* kraus = kraus_operator(s);
	matrix_print(kraus);
	free_q_state(s);

	return 0;
}