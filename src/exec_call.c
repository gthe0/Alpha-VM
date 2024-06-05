#include <dispatcher.h>
#include <avm-mem.h>
#include <avm-lib.h>
#include <avm-utils.h>
#include <avm-reader.h>
#include <avm-log.h>

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/* Typedefs of call expressions */
typedef void (*call_expressions) (avm_memcell*,Instruction_T);

/* Implementation of the call functions */
void call_string(avm_memcell* m,Instruction_T instr)
{
	assert(m && m->type == string_m);

    avm_call_saveenvironment();
	execute_lib_func(m->data.strVal);

	if(!executionFinished)
		execute_funcexit(instr);
}

void call_library(avm_memcell* m,Instruction_T instr)
{
	assert(m && m->type == libfunc_m);

    avm_call_saveenvironment();
	execute_lib_func(get_libFuncs(m->data.libfuncVal));

	if(!executionFinished)
		execute_funcexit(instr);
}

void call_userFuncs(avm_memcell* m,Instruction_T instr)
{
	assert(m && m->type == userfunc_m);

    avm_call_saveenvironment();
	pc = get_UserFunc(m->data.funcVal).address;

	assert( pc < AVM_ENDING_PC);
}

void call_functor(avm_memcell* m,Instruction_T instr)
{
	return;
}

/* call functions table */
call_expressions callFuncs[] = {
	NULL,	/* number */
    call_string,
    NULL,	/* bool */
    call_functor,
    call_userFuncs,
    call_library,
    NULL,	/* nil */
    NULL	/* undef*/
};

/* Execute object calls */
void execute_call(Instruction_T instr)
{
	avm_memcell* call_expr = avm_translate_operand(&(instr->arg1),&cx);
	assert(call_expr);
	
	call_expressions f = callFuncs[call_expr->type];

	if (!f)
		avm_log(ERROR,"Cannot call a '%s'\n",typeString[call_expr->type]);
	else (*f)(call_expr,instr);	

	return ;
}
