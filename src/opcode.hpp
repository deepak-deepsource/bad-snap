#pragma once

#include "common.hpp"

namespace snap {

///  IMPORTANT: after making any changes to this enum, make sure
///  relevant changes are made in the 4 variables below, and in the `op_strs`
///  array in the file debug.cpp.
enum class Opcode {
	// opcodes with 1 operand
	load_const, /* load_const, idx */

	// opcodes with no operands
	pop,
	add,
	sub,
	mult,
	mod,
	div,
	nil,

	op_count
};

/// numerically lowest opcode that takes no operands
const auto Op_0_operands_start = Opcode::pop;
/// numerically highest opcode that takes no operands
const auto Op_0_operands_end = Opcode::mod;

/// numerically lowest opcode that takes one operand
const auto Op_1_operands_start = Opcode::load_const;
/// numerically highest opcode that takes one operand
const auto Op_1_operands_end = Opcode::load_const;

} // namespace snap