#include <dispatcher.h>
#include <avm-mem.h>
#include <avm-log.h>

#include <assert.h>

typedef double (*arithmetic_func_t)	(double x, double y);

/* Implementation of the arithmetic functions */
double add_impl	(double x, double y)	{ return x + y ; }
double sub_impl	(double x, double y)	{ return x - y ; }
double mul_impl	(double x, double y)	{ return x * y ; }
double div_impl	(double x, double y)	{ return x / y ; }
double mod_impl	(double x, double y)	{ return ((unsigned)x) % ((unsigned)y) ; }

/* arithmetic functions table */
arithmetic_func_t arithmeticFuncs[] = {
	add_impl,
	sub_impl,
	mul_impl,
	div_impl,
	mod_impl
};

/* Execute arithmetic */
void execute_arithmetic(Instruction_T instr)
{
	avm_memcell* lv = avm_translate_operand(&instr->result, (avm_memcell*)0);
	avm_memcell* rv1 = avm_translate_operand(&instr->arg1, &ax);
	avm_memcell* rv2 = avm_translate_operand(&instr->arg2, &bx);

	assert(lv && (&stack[0] < lv && &stack[top] >= lv || lv == &retval));
	assert(rv1 && rv2);

	if(rv1->type != number_a || rv2->type != number_a)
	{
		avm_log(ERROR,"Arithmetic execution without using numbers\n");
	}
	else
	{
		arithmetic_func_t op = arithmeticFuncs[instr->opcode - add_v];

		avm_mem_cell_clear(lv);
		lv->type = number_m;
		lv->data.numVal = (*op)(rv1->data.numVal, rv2->data.numVal); 
	}

	return ;
}

/* stub function for uminus (it gets translated to mul)*/
void execute_uminus(Instruction_T instr){}

/* Execute boolean (stub functions) */
void execute_and(Instruction_T instr){}
void execute_or(Instruction_T instr){}
void execute_not(Instruction_T instr){}

/* Execute nop (stub function)*/
void execute_nop(Instruction_T instr){}