#include <dispatcher.h>
#include <avm-mem.h>
#include <avm-reader.h>
#include <avm-log.h>

#include <assert.h>

/* Pushes arguments to the stack */
void execute_pusharg(Instruction_T instr)
{
	avm_memcell* arg = avm_translate_operand(&instr->result,&ax);

	assert(arg && top < AVM_STACKSIZE);

	avm_assign(&stack[top], arg);

	++totalActuals;

	avm_dec_top();

	return ;
}

/* Enters a function, pushes things to the stack */
void execute_funcenter(Instruction_T instr)
{
	avm_memcell* func  = avm_translate_operand(&instr->result, &ax);

	assert(func);
	assert(pc == func->data.funcVal);

	totalActuals = 0;

	userfunc_t function = get_UserFunc(pc);

	topsp = top;
	top = top - function.localSize; 

	return;
}

/* Exits the function, pops the stack */
void execute_funcexit(Instruction_T instr)
{
	unsigned oldTop = top ;

	top = avm_get_envvalue(topsp + AVM_SAVEDTOP_OFFSET);
	pc = avm_get_envvalue(topsp + AVM_SAVEDPC_OFFSET);
	topsp = avm_get_envvalue(topsp + AVM_SAVEDTOPSP_OFFSET);

	/* Intentionally ignoring first */
	while (++oldTop <= top)
		avm_mem_cell_clear(&stack[oldTop]);

	return ;
}
