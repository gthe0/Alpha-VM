/*
 	Authors: csd4881, csd4988, csd5038														
	
	avm-lib.c

	Implementation of the library functions.
*/

#include <avm-lib.h>
#include <avm-mem.h>
#include <avm-log.h>
#include <avm-utils.h>
#include <avm-reader.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

/*totalargs function*/
static void libfunc_totalarguments (void)
{
	/* Get topsp of prev activation record.
	*/
	unsigned p_topsp = avm_get_envvalue(topsp + AVM_SAVEDTOPSP_OFFSET);
	avm_mem_cell_clear(&retval);

	if(!p_topsp)
	{
		avm_log(ERROR,"'totalarguments' called outside a function!\n");
		retval.type = nil_m ;
	}
	else
	{
		/*
		 Extract the number of actual arguments
		 for the previous activation record.
		*/
		retval.type = number_m;
		retval.data.numVal = avm_get_envvalue(p_topsp + AVM_NUMACTUALS_OFFSET);
	}
}

/* Just prints all the arguments 
by turning them to string first */
static void libfunc_print(void)
{
	unsigned n = avm_total_actuals();
	for (unsigned i = 0; i < n; i++)
	{
		char* s = avm_to_string(avm_getactual(i));
		printf(s);
		free(s);
	}
}

/* Retruns the typeof of an argument */
static void libfunc_typeof(void)
{
	unsigned n = avm_total_actuals();

	if(n != 1)
	{
		avm_log(ERROR,"one argument (not %d) expected in 'typeof'\n",n);
	}
	else
	{
		avm_mem_cell_clear(&retval);
		retval.type = string_m;
		retval.data.strVal = strdup(typeString[avm_getactual(0)->type]);
	}
}

/* Returns nth argument */
static void libfunc_argument(void)
{
	/* Get topsp of prev activation record.
	*/
	unsigned p_topsp = avm_get_envvalue(topsp + AVM_SAVEDTOPSP_OFFSET);
    unsigned n = avm_total_actuals();


	if(!p_topsp)
		avm_log(ERROR,"'argument' called outside a function!\n");
	else
	if(n != 1)
	{
		avm_log(ERROR,"one argument (not %d) expected in 'argument'\n",n);
	}
	else
	{
		avm_memcell* arg = avm_getactual(0);

		assert(arg->type == number_m);
		avm_mem_cell_clear(&retval);

		arg = &stack[p_topsp + AVM_STACKENV_SIZE  + 1 + (unsigned)arg->data.numVal];

		retval = *arg;
	}
}


/* Arithmetic operations */
static void libfunc_sin(void)
{
	/* Basically we call sin which exist already...*/
	unsigned n = avm_total_actuals();

	if(n != 1)
	{
		avm_log(ERROR,"one argument (not %d) expected in 'sin'\n",n);
	}
	else
	{
		avm_mem_cell_clear(&retval);
		avm_memcell* num = avm_getactual(0);

		if(num->type != number_m)
		{
			avm_log(ERROR,"Argument of 'sin' was not a number\n");
			return;
		}

		retval.type = number_m;
		retval.data.numVal = sin(num->data.numVal);
	}
}


static void libfunc_cos(void)
{
	/* Basically we call cos which exist already...*/
	unsigned n = avm_total_actuals();

	if(n != 1)
	{
		avm_log(ERROR,"one argument (not %d) expected in 'cos'\n",n);
	}
	else
	{
		avm_mem_cell_clear(&retval);
		avm_memcell* num = avm_getactual(0);

		if(num->type != number_m)
		{
			avm_log(ERROR,"Argument of 'cos' was not a number\n");
			return;
		}

		retval.type = number_m;
		retval.data.numVal = cos(num->data.numVal);
	}
}

static void libfunc_sqrt(void)
{
	/* Basically we call sin which exist already...*/
	unsigned n = avm_total_actuals();

	if(n != 1)
	{
		avm_log(ERROR,"one argument (not %d) expected in 'sqrt'\n",n);
	}
	else
	{
		avm_mem_cell_clear(&retval);
		avm_memcell* num = avm_getactual(0);

		if(num->type != number_m)
		{
			avm_log(ERROR,"Argument of 'sqrt' was not a number\n");
			return;
		}

		/* If the number is less than 0, return it*/
		if(num->data.numVal < 0.0)
		{
			avm_log(WARNING,"'sqrt' called with negative argument %lf\n", num->data.numVal);
			retval = *num;

			return;
		}

		retval.type = number_m;
		retval.data.numVal = sqrt(num->data.numVal);
	}
}


/* String to num */
void libfunc_strtonum(void)
{
	/* Basically we call strtod which exist already...*/
	unsigned n = avm_total_actuals();

	if(n != 1)
	{
		avm_log(ERROR,"one argument (not %d) expected in 'strtonum'\n",n);
	}
	else
	{
		avm_mem_cell_clear(&retval);
		avm_memcell* str = avm_getactual(0);

		char* endptr;

		if(str->type != string_m)
		{
			avm_log(ERROR,"Argument of 'strtonum' was not a string\n");
			return;
		}

		retval.type = number_m;
		retval.data.numVal = strtod(str->data.strVal, &endptr);
	}
}

/* Object related functions */
static void libfunc_objectmemberkeys(void)
{

}

static void  libfunc_objecttotalmembers(void)
{
	/* Basically we return the total field...*/
	unsigned n = avm_total_actuals();

	if(n != 1)
	{
		avm_log(ERROR,"one argument (not %d) expected in 'objecttotalmembers'\n",n);
	}
	else
	{
		avm_mem_cell_clear(&retval);
		avm_memcell* table = avm_getactual(0);

		char* endptr;

		if(table->type != table_m)
		{
			avm_log(ERROR,"Argument of 'objecttotalmembers' was not a table\n");
			return;
		}

		retval.type = number_m;
		retval.data.numVal = table->data.tableVal->total;
	}
}

static void libfunc_objectcopy(void)
{


}

/* Input getter function */
static void libfunc_input(void)
{


}

#define NO_OF_LIBFUNCTS 12

static library_map lib_map[] = {
	{"print",libfunc_print},
	{"typeof",libfunc_typeof},
	{"input",libfunc_input},
	{"cos",libfunc_cos},
	{"sin",libfunc_sin},
	{"sqrt",libfunc_sqrt},
	{"strtonum",libfunc_strtonum},
	{"argument",libfunc_argument},
	{"totalarguments",libfunc_totalarguments},
	{"objectmemberkeys",libfunc_objectmemberkeys},
	{"objectcopy",libfunc_objectcopy},
	{"objecttotalmembers",libfunc_objecttotalmembers}
};

/* find the right function */
void execute_lib_func(char* id)
{
	for (int i = 0; i < NO_OF_LIBFUNCTS; i++)
	{
		if(!strcmp(id,lib_map[i].id))
		{
			library_func_t func = lib_map[i].func;	
			
			topsp = top;
			totalActuals = 0;

			(*func)();

			return;
		}
	}
	
	return ;
}