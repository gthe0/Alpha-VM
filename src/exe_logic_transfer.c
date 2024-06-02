#include <dispatcher.h>
#include <avm-mem.h>

#include <assert.h>

typedef int (*transfer_logic_func_t)	(double x, double y);

/* Implementation of the transfer logic functions */
int jgt_impl	(double x, double y)	{ return (x > y); }
int jge_impl	(double x, double y)	{ return (x >= y); }
int jlt_impl	(double x, double y)	{ return (x < y) ; }
int jle_impl	(double x, double y)	{ return (x <= y) ; }

/* comparison functions table */
transfer_logic_func_t transfer_logic_funcs[] = {
	jgt_impl,
	jge_impl,
	jlt_impl,
	jle_impl
};

/* Execute transfer logic */
void execute_transfer_logic(Instruction_T instr)
{
	assert(instr && (instr->result.type == label_a));

	avm_memcell* rv1 = avm_translate_operand(&instr->arg1, &ax);
	avm_memcell* rv2 = avm_translate_operand(&instr->arg2, &bx);

	assert(rv1 && rv2);

	if(rv1->type != number_a || rv2->type != number_a)
	{
		/* ADD AVM_ERROR */
		exit(1);
	}
	else
	{
		transfer_logic_func_t op = transfer_logic_funcs[instr->opcode - jgt_v];

		if(!executionFinished && (*op)(rv1->data.numVal, rv2->data.numVal))
			pc = instr->result.val;
	}
	return ;
}

void execute_jump(Instruction_T instr)
{
	assert(instr && (instr->result.type == label_a));

	if (!executionFinished)
		pc  = instr->result.val;

	return ;
}


void execute_jeq(Instruction_T instr)
{
	return;
}

void execute_jne(Instruction_T instr)
{
	return ;
}