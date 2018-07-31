#ifndef GATE_CLIFFORD_GENERATORS
#define GATE_CLIFFORD_GENERATORS

#include "gates.h"
#include "gate_result.h"

#include "../sym.h"
#include "../error_models/error_models.h"


// ----------------------------------------------------------------------------------------
// FUNCTION DECLARATIONS 
// ---------------------------------------------------------------------------------------

/*
 * gate_pauli_x
 * Implements a pauli X gate on a sym object and returns the new sym object
 * :: const sym* initial state :: The state to apply the X to
 * :: const void* gate_data :: Any additional gate data that can be given to a gate (none needed here)
 * :: const unsigned* target_qubits ::  An array of qubits that this gate acts on, [0] is the target qubit
 * Returns a gate result object containing the new sym object as its only entry
 */
gate_result* gate_pauli_X(const sym* initial_state, const void* gate_data, const unsigned* target_qubit);

/*
 * gate_pauli_Z
 * Implements a pauli Z gate on a sym object and returns the new sym object
 * :: const sym* initial state :: The state to apply the Z to
 * :: const void* gate_data :: Any additional gate data that can be given to a gate (none needed here)
 * :: const unsigned* target_qubits ::  An array of qubits that this gate acts on, [0] is the target qubit
 * Returns a gate result object containing the new sym object as its only entry
 */
gate_result* gate_pauli_Z(const sym* initial_state, const void* gate_data, const unsigned* target_qubit);

/*
 * gate_pauli_Y
 * Implements a pauli Y gate on a sym object and returns the new sym object
 * :: const sym* initial state :: The state to apply the Y to
 * :: const void* gate_data :: Any additional gate data that can be given to a gate (none needed here)
 * :: const unsigned* target_qubits ::  An array of qubits that this gate acts on, [0] is the target qubit
 * Returns a gate result object containing the new sym object as its only entry
 */
gate_result* gate_pauli_y(const sym* initial_state, const void* gate_data, const unsigned* target_qubit);


// ----------------------------------------------------------------------------------------
// FUNCTION DEFINITIONS
// ----------------------------------------------------------------------------------------

gate_result* gate_pauli_X(const sym* initial_state, const void* gate_data, const unsigned* target_qubit)
{
    sym* final_state = sym_copy(initial_state);
    uint32_t target = target_qubit[0];
    
    sym_set_Z(final_state, 0, target, sym_get_Z(initial_state, 0, target) ^ 1);

    return gate_result_create_single(1, final_state);;
}

gate_result* gate_pauli_Z(const sym* initial_state, const void* gate_data, const unsigned* target_qubit)
{
    sym* final_state = sym_copy(initial_state);
    uint32_t target = target_qubit[0];
    
    sym_set_X(final_state, 0, target, sym_get_X(initial_state, 0, target) ^ 1);

    return gate_result_create_single(1, final_state);;
}

gate_result* gate_pauli_Y(const sym* initial_state, const void* gate_data, const unsigned* target_qubit)
{
    sym* final_state = sym_copy(initial_state);
    uint32_t target = target_qubit[0];
    
    sym_set_X(final_state, 0, target, sym_get_X(initial_state, 0, target) ^ 1);
    sym_set_Z(final_state, 0, target, sym_get_Z(initial_state, 0, target) ^ 1);

    return gate_result_create_single(1, final_state);;
}