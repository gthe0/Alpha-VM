#include <dispatcher.h>
#include <avm-reader.h>
#include <avm-mem.h>
#include <avm-log.h>

#include <assert.h>

/* Definition of the array in avm-utils.h... */
char* typeString[] = {
	"number",
	"string",
	"bool",
	"table",
	"userfunc",
	"libfunc",
	"nil",
	"undef"
};

/*
 Typedefinition of functions that 
 turn a memcell to a string
*/
typedef char* (to_string_Funcs)(avm_memcell*);

static char* number_to_string(avm_memcell* m) 
{ 
	return m->data.numVal != 0;
}

static char* string_to_string(avm_memcell* m) 
{ return m->data.strVal[0] != 0;}

static char* bool_to_string(avm_memcell* m)	 
{ return m->data.boolVal;}

static char* table_to_string(avm_memcell* m) 
{ return 1;}

static char* userfunc_to_string(avm_memcell* m) 
{ return 1;} 

static char* libfunc_to_string(avm_memcell* m) 
{ return 1;} 

static char* nil_to_string(avm_memcell* m)	 
{ return 0;} 

static char* undef_to_string(avm_memcell* m) 
{ assert(0); return 0;} 


void avm_to_string()
{

}