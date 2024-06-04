#include <dispatcher.h>
#include <avm-utils.h>
#include <avm-log.h>
#include <avm-mem.h>

#include <stdlib.h>
#include <assert.h>

/* Creates a new table entry */
void execute_newtable(Instruction_T instr)
{
	avm_memcell* lv = avm_translate_operand(&instr->arg1, (avm_memcell*) 0);
	assert(lv && (&stack[AVM_STACKSIZE - 1] >= lv && lv>&stack[top] || lv == &retval));

	lv->type = table_m;
	lv->data.tableVal = avm_table_new();

	avm_table_inc_refcounter(lv->data.tableVal);

	return ;
}

/* executes table[index]; */
void execute_tablegetelem(Instruction_T instr)
{
	avm_memcell* lv = avm_translate_operand(&instr->result, (avm_memcell*) 0);
	avm_memcell* t = avm_translate_operand(&instr->arg1, (avm_memcell*) 0);
	avm_memcell* i = avm_translate_operand(&instr->arg2, &ax);
	
	assert(lv && (&stack[AVM_STACKSIZE - 1] >= lv && lv>&stack[top] || lv == &retval));
	assert(t && (&stack[AVM_STACKSIZE - 1] >= lv && t>&stack[top] || t == &retval));
	assert(i);

	avm_mem_cell_clear(lv);
	lv->type = nil_m;

	if (t->type != table_m)
		avm_log(ERROR,"Illegal usage of type %s as table !\n",typeString[t->type]);	
	else
	{
		avm_memcell* content = avm_tablegetelem(t->data.tableVal, i);

		if(content)
			avm_assign(lv, content);
		else
		{
			char* ts = avm_to_string(t);
			char* is = avm_to_string(i);

			avm_log(WARNING,"%s[%s] is Illegal\n",ts,is);			

			free(ts);
			free(is);
		}
	}

	return ;
}

/* executes table[index] = content; */
void execute_tablesetelem(Instruction_T instr)
{
	avm_memcell* t = avm_translate_operand(&instr->arg1, (avm_memcell*) 0);
	avm_memcell* i = avm_translate_operand(&instr->arg2, &ax);
	avm_memcell* c = avm_translate_operand(&instr->result, &bx);

	assert(t && (&stack[AVM_STACKSIZE - 1 ] >= t  && t>&stack[top]));
	assert(i && c);

	if(t->type != table_m)
		avm_log(ERROR,"Illegal usage of type %s as table !\n",typeString[t->type]);	
	else
		avm_tablesetelem(t->data.tableVal, i, c);

	return;
}