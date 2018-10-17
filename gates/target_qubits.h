#ifndef GATES_TARGET_QUBITS
#define GATES_TARGET_QUBITS

uint32_t* target_qubits_create();
uint32_t* target_qubits_create_range(uint32_t low, uint32_t high);
void target_qubits_free(uint32_t* target_qubits);


uint32_t* target_qubits_create(uint32_t n_elements, ...)
{
	// Create the va_list
	va_list args;
    va_start(args, n_elements);

    // Allocate memory
    uint32_t* target_qubits = (uint32_t*)malloc(sizeof(uint32_t) * n_elements);
	
	// Copy over 
	// Pretty sure that this could be done with a memcpy, but that sounds implementation dependent
	for (uint32_t i = 0; i < n_elements; i++)
	{
		target_qubits[i] = va_arg(args, uint32_t);		
	}
	va_end(args);

	return target_qubits;
}

uint32_t* target_qubits_create_range(uint32_t low, uint32_t high)
{
	uint32_t* target_qubits = (uint32_t*)malloc(sizeof(uint32_t) * (high - low));
	for (int i = 0; i < high - low; i++)
	{
		target_qubits[i] = low + i;
	}
	return target_qubits;
}

void target_qubits_free(uint32_t* target_qubits)
{
	if (NULL != target_qubits)
	{
		free(target_qubits);	
	}
	return;
}


#endif