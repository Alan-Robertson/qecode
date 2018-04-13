#indef GATE_OPERATIONS
#define GATE_OPERATIONS



// STRUCT OBJECTS ----------------------------------------------------------------------------------------


//  Gate operation function pointer
typedef sym* (*gate_operation_f)(const sym*, void*);

/*
	gate:
	The gate struct
	:: unsigned n_qubits :: Number of qubits in the gate
	:: error_model_f error_model :: The error model to be applied when this gate is used
	:: void* model_data :: Data for the error model
*/
typedef struct {
	unsigned n_qubits;
	gate_operation_f operation;
	error_model_f error_model;
	void* error_model_data;
} gate;


#endif
