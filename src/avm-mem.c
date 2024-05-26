/*
 	Authors: csd4881, csd4988, csd5038														
	
	avm-mem.c										

	Here we implement the avm-table ADT
*/

#include <avm-mem.h>

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/* Stack module */
avm_memcell stack[AVM_STACKSIZE];
avm_memcell ax, bx, cx, retval;

static unsigned top, topsp;

typedef void (*memclear_func_t)(avm_memcell*);

/**
* @brief Called by avm_mem_cell_clear to free the string data type
* @param m The memcell to be cleared
*/
static void memclear_string(avm_memcell* m)
{
	assert(m->data.strVal);
	free(m->data.strVal);
}

/**
* @brief Called by avm_mem_cell_clear to free the table data type
* @param m The memcell to be cleared
*/
static void memclear_table(avm_memcell* m)
{
	assert(m->data.tableVal);
	avm_tabledec_refcounter(m->data.tableVal);
}

/**
* @brief Used to store the enviroment information in the stack
* @param val the value to be pushed in the stack
*/
void avm_push_envvalue(unsigned val)
{
	stack[top].type = number_m;
	stack[top].data.numVal = val;

	avm_dec_top();
}

/*
 Function table used to call the correct memclear
 function based on the memcell type
*/
memclear_func_t memclearFuncs[] = {
	0,		/* number*/
	memclear_string,
	0,		/* bool */
	memclear_table,
	0,		/* userfunc */
	0,		/* libfunc */
	0,		/* nil */
	0,		/* undef */
};

void avm_initstack(void)
{
	for (unsigned i = 0; i < AVM_STACKSIZE ; i++)
	{	
		AVM_WIPEOUT(stack[i]);
		stack[i].type = undef_m;
	}

	return;
}

void avm_table_inc_refcounter(avm_table *table) {
    ++table->refCounter;
}

void avm_table_dec_refcounter(avm_table *table) {
	assert(table->refCounter > 0);
    
	/* Activate garbage collection... */
	if (!--table->refCounter) 
		avm_tabledestroy(table); 
}

void avm_table_buckets_init(avm_table_bucket** p)
{
	/* Same as assigningn NULL to each bucket */
	for (unsigned i = 0; i < AVM_TABLE_HASH_SIZE; i++)
		p[i] = (avm_table_bucket*)0; 
}


avm_table* avm_table_new(void)
{
    avm_table *table = malloc(sizeof(avm_table));
    AVM_WIPEOUT(*table);

    table->refCounter = table->total = 0;

    avm_tablebuckets_init(table->numIndexed);
    avm_tablebuckets_init(table->strIndexed);
    avm_tablebuckets_init(table->boolIndexed);
    avm_tablebuckets_init(table->userIndexed);
    avm_tablebuckets_init(table->libIndexed);

    return table;
}

void avm_mem_cell_clear(avm_memcell* m)
{
	assert(m);

	if(m->type == undef_m) return;

	memclear_func_t f = memclearFuncs[m->type];

	/* Free the correct data based on its type */
	if(f)
		(*f)(m);
	
	m->type = undef_m;
}

void avm_table_buckets_destroy(avm_table_bucket** p)
{
	for (unsigned i = 0; i < AVM_TABLE_HASH_SIZE ; i++, p++)
	{
		for (avm_table_bucket* b = p; i < b;)
		{
			avm_table_bucket* del = b;
			b = b->next;

			/* Free the cells before freeing the bucket */
			avm_mem_cell_clear(&del->key);
			avm_mem_cell_clear(&del->value);

			free(del);
		}

		p[i] = NULL ;
	}
}

void avm_table_destroy (avm_table* t)
{
	avm_table_buckets_destroy(t->numIndexed);
	avm_table_buckets_destroy(t->strIndexed);
	avm_table_buckets_destroy(t->boolIndexed);
	avm_table_buckets_destroy(t->userIndexed);
	avm_table_buckets_destroy(t->libIndexed);

	free(t);
}

void avm_assign(avm_memcell* lv, avm_memcell* rv)
{
	/* No need for assignment here, they are the same*/
	if	(lv == rv)	
		return ;

	/* If they are references to the same table,
	 there is no need for assignment
	*/
	if	(lv->type == table_m &&
		 rv->type == table_m &&
		 lv->data.tableVal == rv->data.tableVal)	
		return;

	/* Clear the old cell to replace it with the rv*/
	avm_mem_cell_clear(lv);		
	memcpy(lv,rv,sizeof(avm_memcell));

	/* Now we deep copy the data to make sure it does not get deleted */
	if	(lv->type == string_m)
		lv->data.strVal = strdup(rv->data.strVal);
	else 
	if	(lv->type == table_m)
		avm_table_inc_refcounter(lv->data.tableVal);
}


avm_memcell* avm_translate_operand(vmarg_T arg, avm_memcell* reg)
{
	assert(arg);

	switch (arg->type)
	{
		case global_a:	return &stack[AVM_STACKSIZE-arg->val-1];
		case local_a:	return &stack[topsp-arg->val];
		case formal_a:	return &stack[topsp+AVM_STACKENV_SIZE+arg->val+1];

		case retval_a:	return &retval;

		case number_a:
		{
			reg->type = number_m;
			reg->data.numVal = consts_getnumber(arg->val);
			return reg;
		}
		
		case string_a:
		{
			reg->type = string_m;
			reg->data.numVal = strdup(consts_getnumber(arg->val));
			return reg;
		}
		
		case bool_a:
		{
			reg->type = bool_m;
			reg->data.numVal = arg->val;
			return reg;
		}

		case undef_a:
		case nil_a:	reg->type = nil_m;	return reg;
		
		case userfunc_a:
		{
			reg->type = userfunc_m;
			reg->data.numVal = arg->val;	/* shallow copy it */
			return reg;
		}

		case libfunc_a:
		{
			reg->type = libfunc_m;
			reg->data.libfuncVal = libfuncs_getuesd(arg->val);
			return reg;
		}

		default:
			break;
	}

	return;
}

void avm_dec_top(void)
{
	if (!top)
	{
		perror("TEMP ERROR LOGGING FUNCTION\n");
	}
	else
		--top;
}

void avm_push_envvalue(unsigned val)
{
	stack[top].type = number_m;
	stack[top].data.numVal = val;

	avm_dec_top();
}