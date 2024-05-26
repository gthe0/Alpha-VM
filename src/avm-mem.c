#include <avm-mem.h>

#include <stdlib.h>
#include <assert.h>

avm_memcell stack[AVM_STACKSIZE];
avm_memcell ax, bx, cv, retval;

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

/**
* @brief Used to initialize the stack module
*/
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

