#ifndef GATE_MEASUREMENT
#define GATE_MEASUREMENT

// ----------------------------------------------------------------------------------------
// DIRECTIVES
// ----------------------------------------------------------------------------------------

#include "../sym.h"
#include "gate_result.h"
#include "gate.h"

// ----------------------------------------------------------------------------------------
// Structs
// ----------------------------------------------------------------------------------------

typedef struct {
    uint32_t n_bits;
} gate_measurement_t;

// ----------------------------------------------------------------------------------------
// FUNCTION DECLARATIONS 
// ----------------------------------------------------------------------------------------

/*
 * gate_measure_X
 * Performs a measurement in the X basis by checking the commutation relations with any pauli operators that have been applied 
 * :: const sym* initial state :: The state to measure
 * :: const void* gate_data :: Any additional gate data that can be given to a gate
 * :: const unsigned* target_qubits ::  An array of qubits that this gate acts on
 * Returns a new sym object containing the measurement outcomes
 */
sym* gate_measure_X(const sym* initial_state, void* gate_data, const unsigned* target_qubits);

/*
 * gate_measure_Y
 * Performs a measurement in the Y basis by checking the commutation relations with any pauli operators that have been applied 
 * :: const sym* initial state :: The state to measure
 * :: const void* gate_data :: Any additional gate data that can be given to a gate
 * :: const unsigned* target_qubits ::  An array of qubits that this gate acts on
 * Returns a new sym object containing the measurement outcomes
 */
sym* gate_measure_Y(const sym* initial_state, void* gate_data, const unsigned* target_qubits);

/*
 * gate_measure_Z
 * Performs a measurement in the Z basis by checking the commutation relations with any pauli operators that have been applied 
 * :: const sym* initial state :: The state to measure
 * :: const void* gate_data :: Any additional gate data that can be given to a gate
 * :: const unsigned* target_qubits ::  An array of qubits that this gate acts on
 * Returns a new sym object containing the measurement outcomes
 */
sym* gate_measure_Z(const sym* initial_state, void* gate_data, const unsigned* target_qubits);

// ----------------------------------------------------------------------------------------
// FUNCTION DEFINITIONS
// ----------------------------------------------------------------------------------------

/*
 * gate_measure_X
 * Performs a measurement in the X basis by checking the commutation relations with any pauli operators that have been applied 
 * :: const sym* initial state :: The state to measure
 * :: const void* gate_data :: Any additional gate data that can be given to a gate
 * :: const unsigned* target_qubits ::  An array of qubits that this gate acts on
 * Returns a new sym object containing the measurement outcomes
 */
sym* gate_measure_X(const sym* initial_state, void* gate_data, const unsigned* target_qubits)
{
    uint32_t n_bits = ((gate_measurement_t*)gate_data)->n_bits;
    
    // Sym object containing our measurement data
    sym* measurement_outcome = sym_create(1, n_bits);

    for (uint32_t i = 0; i < n_bits; i++)
    { // If the paulis anti-commute with X, then count it as a '1', else it's a '0'
        sym_set(measurement_outcome, 0, i, sym_get_Z(initial_state, 0, target_qubits[i]));
    }

    return measurement_outcome;
}

/*
 * gate_measure_Y
 * Performs a measurement in the Y basis by checking the commutation relations with any pauli operators that have been applied 
 * :: const sym* initial state :: The state to measure
 * :: const void* gate_data :: Any additional gate data that can be given to a gate
 * :: const unsigned* target_qubits ::  An array of qubits that this gate acts on
 * Returns a new sym object containing the measurement outcomes
 */
sym* gate_measure_Y(const sym* initial_state, void* gate_data, const unsigned* target_qubits)
{
    uint32_t n_bits = ((gate_measurement_t*)gate_data)->n_bits;

    // Sym object containing our measurement data
    sym* measurement_outcome = sym_create(1, n_bits);

    for (uint32_t i = 0; i < n_bits; i++)
    { // If the paulis anti-commute with Y, then count it as a '1', else it's a '0'
        sym_set(measurement_outcome, 0, i, sym_get_X(initial_state, 0, target_qubits[i]) ^ sym_get_Z(initial_state, 0, target_qubits[i]));
    }

    return measurement_outcome;
}

/*
 * gate_measure_Z
 * Performs a measurement in the Z basis by checking the commutation relations with any pauli operators that have been applied 
 * :: const sym* initial state :: The state to measure
 * :: const void* gate_data :: Any additional gate data that can be given to a gate
 * :: const unsigned* target_qubits ::  An array of qubits that this gate acts on
 * Returns a new sym object containing the measurement outcomes
 */
sym* gate_measure_Z(const sym* initial_state, void* gate_data, const unsigned* target_qubits)
{
    uint32_t n_bits = ((gate_measurement_t*)gate_data)->n_bits;

    // Sym object containing our measurement data
    sym* measurement_outcome = sym_create(1, n_bits);

    for (uint32_t i = 0; i < n_bits; i++)
    { // If the paulis anti-commute with Z, then count it as a '1', else it's a '0'
        sym_set(measurement_outcome, 0, i, sym_get_X(initial_state, 0, target_qubits[i]));
    }

    return measurement_outcome;
}

#endif