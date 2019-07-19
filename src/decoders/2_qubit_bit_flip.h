

//2 Qubit Bit Flip Decoder--------------------------------------------------------------------

sym* decoder_2_qubit_bit_flip(const sym* syndrome, void* decoder_data)
{
	//sym* recovery = sym_create(1, 4);
	sym* recovery_operator;

	switch (sym_get(syndrome, 0, 0))
	{
		case 0:
			// Do Nothing, no error detected
			recovery_operator = error_str_to_sym("II");
			return recovery_operator;
		break;

		case 1:
			// ZI error
			recovery_operator = error_str_to_sym("XI");
			return recovery_operator;
		break;
	}
	//return recovery;
	return NULL;
}