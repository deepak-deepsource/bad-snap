#pragma once

#include "common.hpp"

namespace snap {

///  IMPORTANT: after making any changes to this enum, make sure
///  relevant changes are made in the 4 variables below, and in the `op_strs`
///  array in the file debug.cpp.
enum class Opcode {
	// opcodes with a constant operand
	load_const,

	// opcodes with one operand
	set_var,
	get_var,

	// opcodes with no operands
	pop,
	add,
	sub,
	mult,
	mod,
	div,
	eq,
	nil,
	return_val,

	op_count
};

/// numerically lowest opcode that takes no operands
const auto Op_0_operands_start = Opcode::pop;
/// numerically highest opcode that takes no operands
const auto Op_0_operands_end = Opcode::nil;

const auto Op_const_start = Opcode::load_const;
const auto Op_const_end = Opcode::load_const;

/// numerically lowest opcode that takes one operand
const auto Op_1_operands_start = Opcode::set_var;
/// numerically highest opcode that takes one operand
const auto Op_1_operands_end = Opcode::get_var;

} // namespace snap