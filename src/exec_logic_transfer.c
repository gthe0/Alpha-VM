#include <dispatcher.h>
#include <avm-utils.h>
#include <avm-mem.h>
#include <avm-log.h>

#include <assert.h>
#include <string.h>
#include <stdlib.h>

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

	if(rv1->type != number_m || rv2->type != number_m)
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

/* typedefinition for the boolean convertion functions */
typedef unsigned char (*tobool_func_t)(avm_memcell*);

static unsigned char number_tobool(avm_memcell* m)		{ return m->data.numVal != 0;} 
static unsigned char string_tobool(avm_memcell* m)		{ return m->data.strVal[0] != 0;}
static unsigned char bool_tobool(avm_memcell* m)		{ return m->data.boolVal;}
static unsigned char table_tobool(avm_memcell* m)		{ return 1;}
static unsigned char userfunc_tobool(avm_memcell* m)	{ return 1;} 
static unsigned char libfunc_tobool(avm_memcell* m)		{ return 1;} 
static unsigned char nil_tobool(avm_memcell* m)			{ return 0;} 
static unsigned char undef_tobool(avm_memcell* m)		{ assert(0); return 0;} 

static tobool_func_t toBoolFuncs[] = {
	number_tobool,
	string_tobool,
	bool_tobool,
	table_tobool,
	userfunc_tobool,
	libfunc_tobool,
	nil_tobool,
	undef_tobool
};

/* Function used to turn things into booleans */
static unsigned char avm_tobool(avm_memcell* m)
{
	assert(m && m->type >= 0 && m->type < undef_m);
	return (*toBoolFuncs[m->type])(m);
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
		result = rv1->type == nil_m && rv2->type == nil_m;
	else
	if (rv1->type == bool_m || rv2->type == bool_m )
		result = avm_tobool(rv1) == avm_tobool(rv2);
	else
	if (rv1->type != rv2->type)
		avm_log (
			ERROR,"%s == %s is illegal!\n",
			typeString[rv1->type],
			typeString[rv2->type]
			);	
	else
	{
		/* I was bored to implement functions so that is what you get */
		switch (rv1->type)
		{		
			case number_m:
				result = rv1->data.numVal == rv2->data.numVal;
				break;
			case string_m:
				result = !strcmp(rv1->data.strVal, rv2->data.strVal);
				break;
			case libfunc_m:
				result = rv1->data.libfuncVal == rv2->data.libfuncVal;
				break;
			case userfunc_m:
				result = rv1->data.funcVal == rv2->data.funcVal;
				break;
			case table_m:
				result = rv1->data.tableVal == rv2->data.tableVal;
				break;
			default:
				result = avm_tobool(rv1) == avm_tobool(rv2);
		}
	}

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
		result = rv1->type == nil_m && rv2->type == nil_m;
	else
	if (rv1->type == bool_m || rv2->type == bool_m )
		result = avm_tobool(rv1) == avm_tobool(rv2);
	else
	if (rv1->type != rv2->type)
		avm_log (
			ERROR,"%s == %s is illegal!\n",
			typeString[rv1->type],
			typeString[rv2->type]
			);
	else
	{
		/* I was bored to implement functions so that is what you get */
		switch (rv1->type)
		{		
			case number_m:
				result = rv1->data.numVal == rv2->data.numVal;
				break;
			case string_m:
				result = !strcmp(rv1->data.strVal, rv2->data.strVal);
				break;
			case libfunc_m:
				result = rv1->data.libfuncVal == rv2->data.libfuncVal;
				break;
			case userfunc_m:
				result = rv1->data.funcVal == rv2->data.funcVal;
				break;
			case table_m:
				result = rv1->data.tableVal == rv2->data.tableVal;
				break;
			default:
				result = avm_tobool(rv1) == avm_tobool(rv2);
		}
	}

	if(!executionFinished && !result)
		pc = instr->result.val;

	return ;
}