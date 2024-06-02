#include <dispatcher.h>
#include <avm-mem.h>
#include <avm-log.h>

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
		avm_log(ERROR,"Comparison execution without using numbers\n");
	}
	else
	{
		transfer_logic_func_t op = transfer_logic_funcs[instr->opcode - jgt_v];

		if(!executionFinished && (*op)(rv1->data.numVal, rv2->data.numVal))
			pc = instr->result.val;
	}
	return ;
}

/* Jump instructions are simple */
void execute_jump(Instruction_T instr)
{
	assert(instr && (instr->result.type == label_a));

	if (!executionFinished)
		pc  = instr->result.val;

	return ;
}

/* jeq label arg1 arg2 */
void execute_jeq(Instruction_T instr)
{
	assert(instr && (instr->result.type == label_a));

	avm_memcell* rv1 = avm_translate_operand(&instr->arg1, &ax);
	avm_memcell* rv2 = avm_translate_operand(&instr->arg2, &bx);

	unsigned char result = 0;

	if (rv1->type == undef_m || rv2->type == undef_m)
		avm_log(ERROR,"'undef' involved in equality!\n");	
	else
	if (rv1->type == nil_m || rv2->type == nil_m )
		result == rv1->type == nil_m && rv2->type == nil_m;
	else
	if (rv1->type == bool_m || rv2->type == bool_m )
		/* ADD AVM TO BOOL*/
		result == rv1->type == nil_m && rv2->type == nil_m;
	else
	if (rv1->type != rv2->type)
		avm_log(ERROR,"'undef' involved in equality!\n");	
	else
		/*EQ TEST*/	
		return;

	if(!executionFinished && result)
		pc = instr->result.val;

	return;
}

/*
 jne label arg1 arg2 
 The code is the same with jeq but it differs
 in the way that the pc is updated.
*/
void execute_jne(Instruction_T instr)
{
	assert(instr && (instr->result.type == label_a));

	avm_memcell* rv1 = avm_translate_operand(&instr->arg1, &ax);
	avm_memcell* rv2 = avm_translate_operand(&instr->arg2, &bx);

	unsigned char result = 0;

	if (rv1->type == undef_m || rv2->type == undef_m)
		avm_log(ERROR,"'undef' involved in equality!\n");	
	else
	if (rv1->type == nil_m || rv2->type == nil_m )
		result == rv1->type == nil_m && rv2->type == nil_m;
	else
	if (rv1->type == bool_m || rv2->type == bool_m )
		/* ADD AVM TO BOOL*/
		result == rv1->type == nil_m && rv2->type == nil_m;
	else
	if (rv1->type != rv2->type)
		avm_log(ERROR,"'undef' involved in equality!\n");	
	else
		/*EQ TEST*/	
		return;

	if(!executionFinished && !result)
		pc = instr->result.val;

	return ;
}