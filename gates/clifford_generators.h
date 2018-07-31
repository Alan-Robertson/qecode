#ifndef GATE_CLIFFORD_GENERATORS
#define GATE_CLIFFORD_GENERATORS

#include "gates.h"
#include "gate_result.h"

#include "../sym.h"
#include "../error_models/error_models.h"

// ----------------------------------------------------------------------------------------
// STRUCT OBJECTS
// These aren't currently needed but are here for if they ever are
// ----------------------------------------------------------------------------------------

// typedef struct {} gate_data_cnot_t;
// typedef struct {} gate_data_hadamard_t;
// typedef struct {} gate_data_phase_t;
// typedef struct {} gate_data_identity_t;

// ----------------------------------------------------------------------------------------
// FUNCTION DECLARATIONS 
// ----------------------------------------------------------------------------------------

/*
 * gate_cnot
 * Implements a cnot gate on a sym object and returns the new sym object
 * :: const sym* initial state :: The state to apply the cnot to
 * :: const void* gate_data :: Any additional gate data that can be given to a gate (none needed here)
 * :: const unsigned* target_qubits ::  An array of qubits that this gate acts on, [0] is the control, [1] is the target
 * Returns a gate result object containing the new sym object as its only entry
 */
gate_result* gate_cnot(const sym* initial_state, const void* gate_data, const unsigned* target_qubits);

/*
 * gate_hadamard
 * Implements a hadamard gate on a sym object and returns the new sym object
 * :: const sym* initial state :: The state to apply the cnot to
 * :: const void* gate_data :: Any additional gate data that can be given to a gate (none needed here)
 * :: const unsigned* target_qubits ::  An array of qubits that this gate acts on, [0] is the target qubit
 * Returns a gate result object containing the new sym object as its only entry
 */
gate_result* gate_hadamard(const sym* initial_state, const void* gate_data, const unsigned* target_qubit);

/*
 * gate_phase
 * Implements a phase gate on a sym object and returns the new sym object
 * :: const sym* initial state :: The state to apply the cnot to
 * :: const void* gate_data :: Any additional gate data that can be given to a gate (none needed here)
 * :: const unsigned* target_qubits :: An array of qubits that this gate acts on, [0] is the target
 * Returns a gate result object containing the new sym object as its only entry
 */
gate_result* gate_phase(const sym* initial_state, const void* gate_data, const unsigned* target_qubit);

/*
 * gate_identity
 * Implements an identity gate on a sym object and returns the new sym object
 * :: const sym* initial state :: The state to apply the cnot to
 * :: const void* gate_data :: Any additional gate data that can be given to a gate (none needed here)
 * :: const unsigned* target_qubits ::  An array of qubits that this gate acts on, [0] is the target
 * Returns a gate result object containing the new sym object as its only entry
 */
gate_result* gate_identity(const sym* initial_state, const void* gate_data, const unsigned* target_qubits);

// ----------------------------------------------------------------------------------------
// FUNCTION DEFINITIONS
// ----------------------------------------------------------------------------------------

/*
 * gate_cnot
 * Implements a cnot gate on a sym object and returns the new sym object
 * :: const sym* initial state :: The state to apply the cnot to
 * :: const void* gate_data :: Any additional gate data that can be given to a gate (none needed here)
 * :: const unsigned* target_qubits ::  An array of qubits that this gate acts on, [0] is the control, [1] is the target
 * Returns a gate result object containing the new sym object as its only entry
 */
gate_result* gate_cnot(const sym* initial_state, const void* gate_data, const unsigned* target_qubits)
{
    sym* final_state = sym_copy(initial_state);
    uint32_t control = target_qubits[0];
    uint32_t target = target_qubits[1];

    sym_set_X(final_state, 0, target, sym_get_X(initial_state, 0, control) ^ sym_get_X(initial_state, 0, target));
    sym_set_Z(final_state, 0, control, sym_get_Z(initial_state, 0, target) ^ sym_get_Z(initial_state, 0, control));

    return gate_result_create_single(1, final_state);
}

/*
 * gate_hadamard
 * Implements a hadamard gate on a sym object and returns the new sym object
 * :: const sym* initial state :: The state to apply the cnot to
 * :: const void* gate_data :: Any additional gate data that can be given to a gate (none needed here)
 * :: const unsigned* target_qubits ::  An array of qubits that this gate acts on, [0] is the target qubit
 * Returns a gate result object containing the new sym object as its only entry
 */
gate_result* gate_hadamard(const sym* initial_state, const void* gate_data, const unsigned* target_qubit)
{
    sym* final_state = sym_copy(initial_state);
    uint32_t target = target_qubit[0];
    
    sym_set_X(final_state, 0, target, sym_get_Z(initial_state, 0, target));
    sym_set_Z(final_state, 0, target, sym_get_X(initial_state, 0, target));

    return gate_result_create_single(1, final_state);;
}

/*
 * gate_phase
 * Implements a phase gate on a sym object and returns the new sym object
 * :: const sym* initial state :: The state to apply the cnot to
 * :: const void* gate_data :: Any additional gate data that can be given to a gate (none needed here)
 * :: const unsigned* target_qubits :: An array of qubits that this gate acts on, [0] is the target
 * Returns a gate result object containing the new sym object as its only entry
 */
gate_result* gate_phase(const sym* initial_state, const void* gate_data, const unsigned* target_qubit)
{
    sym* final_state = sym_copy(initial_state);
    uint32_t target = target_qubit[0];

    sym_set_Z(final_state, 0, target, sym_get_Z(initial_state, 0, target) ^ sym_get_X(initial_state, 0, target));
    
    return gate_result_create_single(1, final_state);
}

/*
 * gate_identity
 * Implements an identity gate on a sym object and returns the new sym object
 * :: const sym* initial state :: The state to apply the cnot to
 * :: const void* gate_data :: Any additional gate data that can be given to a gate (none needed here)
 * :: const unsigned* target_qubits ::  An array of qubits that this gate acts on, [0] is the target
 * Returns a gate result object containing the new sym object as its only entry
 */
gate_result* gate_identity(const sym* initial_state, const void* gate_data, const unsigned* target_qubits)
{
	sym* final_state = sym_copy(initial_state);
	return gate_result_create_single(1, final_state);
}

#endif