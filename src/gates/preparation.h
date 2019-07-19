#ifndef GATE_PREPARATION
#define GATE_PREPARATION

// ----------------------------------------------------------------------------------------
// DIRECTIVES
// ----------------------------------------------------------------------------------------

#include "../sym.h"
#include "gates.h"
#include "gate_result.h"


// ----------------------------------------------------------------------------------------
// Structs
// ----------------------------------------------------------------------------------------

typedef struct {
    uint8_t prepared_state;
} gate_preparation_t;

// ----------------------------------------------------------------------------------------
// FUNCTION DECLARATIONS 
// ----------------------------------------------------------------------------------------

/*
 * gate_create_prepare
 * Creates a new gate that prepares states in the X basis
 * :: const unsigned n_qubits ::
 * :: const uint8_t prepared_state :: Whether the state is prepared in the |0> or |1> state in the respective basis 
 * :: error_model* em :: Any error acting on the state preparation
 * Returns a new gate object that prepares a state in the given basis
 */
gate* gate_create_prepare_X(
    const unsigned n_qubits, 
    const uint8_t initial_state,
    error_model* em);

/*
 * gate_create_prepare
 * Creates a new gate that prepares states in the Y basis
 * :: const unsigned n_qubits ::
 * :: const uint8_t prepared_state :: Whether the state is prepared in the |0> or |1> state in the respective basis 
 * :: error_model* em :: Any error acting on the state preparation
 * Returns a new gate object that prepares a state in the given basis
 */
gate* gate_create_prepare_Y(
    const unsigned n_qubits, 
    const uint8_t initial_state,
    error_model* em);

/*
 * gate_create_prepare
 * Creates a new gate that prepares states in the Z basis
 * :: const unsigned n_qubits ::
 * :: const uint8_t prepared_state :: Whether the state is prepared in the |0> or |1> state in the respective basis 
 * :: error_model* em :: Any error acting on the state preparation
 * Returns a new gate object that prepares a state in the given basis
 */
gate* gate_create_prepare_Z(
    const unsigned n_qubits, 
    const uint8_t initial_state,
    error_model* em);

/*
 * gate_prepare_X
 * Prepares a state in the X basis
 * :: const sym* initial state :: The state to prepare
 * :: const void* gate_data :: Any additional gate data that can be given to a gate
 * :: const unsigned* target_qubits ::  An array of qubits that this gate acts on
 * Returns a new sym object containing the measurement outcomes
 */
gate_result* gate_prepare_X(const sym* initial_state, const void* gate_data, const unsigned* target_qubits);

/*
 * gate_prepare_Y
 * Prepares a state in the Y basis
 * :: const sym* initial state :: The state to prepare
 * :: const void* gate_data :: Any additional gate data that can be given to a gate
 * :: const unsigned* target_qubits ::  An array of qubits that this gate acts on
 * Returns a new sym object containing the measurement outcomes
 */
gate_result* gate_prepare_Y(const sym* initial_state, const void* gate_data, const unsigned* target_qubits);

/*
 * gate_prepare_Z
 * Prepares a state in the Z basis
 * :: const sym* initial state :: The state to prepare
 * :: const void* gate_data :: Any additional gate data that can be given to a gate
 * :: const unsigned* target_qubits ::  An array of qubits that this gate acts on
 * Returns a new sym object containing the measurement outcomes
 */
gate_result* gate_prepare_Z(const sym* initial_state, const void* gate_data, const unsigned* target_qubits);

// ----------------------------------------------------------------------------------------
// FUNCTION DEFINITIONS
// ----------------------------------------------------------------------------------------

/*
 * gate_create_prepare
 * Factory for creating different types of state preparation gates
 * :: const unsigned n_qubits ::
 * :: const uint8_t prepared_state :: Whether the state is prepared in the |0> or |1> state in the respective basis 
 * :: gate_operation_f operation :: The operation of the state preparation
 * :: error_model* em :: Any error acting on the state preparation
 * Returns a new gate object that prepares a state in the given basis
 */
gate* gate_create_prepare(
    const unsigned n_qubits, 
    const uint8_t prepared_state,
    gate_operation_f operation,
    error_model* em)
{
    gate_preparation_t* gate_data = (gate_preparation_t*)malloc(sizeof(gate_preparation_t));
    gate_data->prepared_state = prepared_state;
    gate* g = gate_create(n_qubits, operation, em, gate_data);
    return g;
}

/*
 * gate_create_prepare
 * Creates a new gate that prepares states in the X basis
 * :: const unsigned n_qubits ::
 * :: const uint8_t prepared_state :: Whether the state is prepared in the |0> or |1> state in the respective basis 
 * :: error_model* em :: Any error acting on the state preparation
 * Returns a new gate object that prepares a state in the given basis
 */
gate* gate_create_prepare_X(
    const unsigned n_qubits, 
    const uint8_t prepared_state,
    error_model* em)
{
    return gate_create_prepare(n_qubits, prepared_state, gate_prepare_X, em);
}

/*
 * gate_create_prepare
 * Creates a new gate that prepares states in the Y basis
 * :: const unsigned n_qubits ::
 * :: const uint8_t prepared_state :: Whether the state is prepared in the |0> or |1> state in the respective basis 
 * :: error_model* em :: Any error acting on the state preparation
 * Returns a new gate object that prepares a state in the given basis
 */
gate* gate_create_prepare_Y(
    const unsigned n_qubits, 
    const uint8_t prepared_state,
    error_model* em)
{
    return gate_create_prepare(n_qubits, prepared_state, gate_prepare_Y, em);
}

/*
 * gate_create_prepare
 * Creates a new gate that prepares states in the Z basis
 * :: const unsigned n_qubits ::
 * :: const uint8_t prepared_state :: Whether the state is prepared in the |0> or |1> state in the respective basis 
 * :: error_model* em :: Any error acting on the state preparation
 * Returns a new gate object that prepares a state in the given basis
 */
gate* gate_create_prepare_Z(
    const unsigned n_qubits, 
    const uint8_t prepared_state,
    error_model* em)
{
    return gate_create_prepare(n_qubits, prepared_state, gate_prepare_Z, em);
}


/*
 * gate_prepare_X
 * Prepares a state in the X basis
 * :: const sym* initial state :: The state to prepare
 * :: const void* gate_data :: Any additional gate data that can be given to a gate
 * :: const unsigned* target_qubits ::  An array of qubits that this gate acts on
 * Returns a new sym object containing the measurement outcomes
 */
gate_result* gate_prepare_X(const sym* initial_state, const void* gate_data, const unsigned* target_qubits)
{
    uint32_t n_qubits = ((gate*)gate_data)->n_qubits;

    uint8_t prepared_state = ((gate_preparation_t*)(((gate*)gate_data)->operation_data))->prepared_state;
    
    // Sym object containing our measurement data
    sym* preparation_outcome = sym_copy(initial_state);

    // Applying an X operation is the same as flipping the state in the Z basis
    for (uint32_t i = 0; i < n_qubits; i++)
    {   sym_set_X(preparation_outcome, 0, target_qubits[i], 0);
        sym_set_Z(preparation_outcome, 0, target_qubits[i], prepared_state);
    }
    return gate_result_create_single(1, preparation_outcome);
}

/*
 * gate_prepare_Y
 * Prepares a state in the Y basis
 * :: const sym* initial state :: The state to prepare
 * :: const void* gate_data :: Any additional gate data that can be given to a gate
 * :: const unsigned* target_qubits ::  An array of qubits that this gate acts on
 * Returns a new sym object containing the measurement outcomes
 */
gate_result* gate_prepare_Y(const sym* initial_state, const void* gate_data, const unsigned* target_qubits)
{
    uint32_t n_qubits = ((gate*)gate_data)->n_qubits;

    uint8_t prepared_state = ((gate_preparation_t*)(((gate*)gate_data)->operation_data))->prepared_state;
    
    // Sym object containing our measurement data
    sym* preparation_outcome = sym_copy(initial_state);

    // Applying an XZ operation is the same as flipping the state in the Y basis
    for (uint32_t i = 0; i < n_qubits; i++)
    {   sym_set_X(preparation_outcome, 0, target_qubits[i], prepared_state);
        sym_set_Z(preparation_outcome, 0, target_qubits[i], prepared_state);
    }

    return gate_result_create_single(1, preparation_outcome);
}

/*
 * gate_prepare_Z
 * Prepares a state in the Z basis
 * :: const sym* initial state :: The state to prepare
 * :: const void* gate_data :: Any additional gate data that can be given to a gate
 * :: const unsigned* target_qubits ::  An array of qubits that this gate acts on
 * Returns a new sym object containing the measurement outcomes
 */
gate_result* gate_prepare_Z(const sym* initial_state, const void* gate_data, const unsigned* target_qubits)
{
    uint32_t n_qubits = ((gate*)gate_data)->n_qubits;

    uint8_t prepared_state = ((gate_preparation_t*)(((gate*)gate_data)->operation_data))->prepared_state;
    
    // Sym object containing our measurement data
    sym* preparation_outcome = sym_copy(initial_state);

    // Applying an X operation is the same as flipping a state in the Z basis
    for (uint32_t i = 0; i < n_qubits; i++)
    {   sym_set_X(preparation_outcome, 0, target_qubits[i], prepared_state);
        sym_set_Z(preparation_outcome, 0, target_qubits[i], 0);
    }

    return gate_result_create_single(1, preparation_outcome);
}

#endif