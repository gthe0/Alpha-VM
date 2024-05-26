/*----------------------------------------------------------*/
/* Authors: csd4881, csd4988, csd5038						*/
/* 														    */
/* avm-types.h												*/
/* 														    */
/* Type definitions of the various structs and enums used	*/
/* for the target code generation to a binary format that	*/	
/* the avm recognizes and executes							*/
/*----------------------------------------------------------*/

#ifndef AVM_TYPES_H
#define AVM_TYPES_H

/* typedefs of the various structs and enums used */
typedef struct instruction instruction, *Instruction_T;

typedef struct userfunc userfunc_t, *UserFunc_T;
typedef struct vmarg vmarg, *vmarg_T;

typedef enum vmopcode vmopcode;
typedef enum vmarg_t vmarg_t;


/* The opcode of the vm Alpha-Byte code */
enum vmopcode {

    /* Assignment opcode */
	assign_v,
    
	/* Airthmetic opcodes */
	add_v,
    sub_v,
    mul_v,
    div_v,
    mod_v,
    uminus_v,
    
	/* Jump opcode */
    jump_v,

	/* Branch opcodes */
    jeq_v,
    jne_v,
    jgt_v,
    jge_v,
    jlt_v,
	jle_v,

	/* call related opcodes */
    call_v,
    pusharg_v,

	/* function defintion related opcode */
    funcenter_v,
    funcexit_v,

	/* Boolean expression related opcode */
    or_v,
    and_v,
    not_v,

	/* Table related opcodes */
    newtable_v,
    tablegetelem_v, 
    tablesetelem_v,

	/* NOP */
    nop_v
};

/* The vm argument type */
enum vmarg_t {
    label_a = 0,
    global_a = 1, 
    formal_a = 2,
    local_a = 3,
    number_a = 4,
    string_a = 5,
    bool_a = 6,
    nil_a = 7,
    userfunc_a = 8,
    libfunc_a = 9,
    retval_a = 10,
	undef_a = 11
};

/* The virtual machine argument */
struct vmarg {
    vmarg_t type;
    unsigned val;
};

/* The Instruction that the vm will execute */
struct instruction {
    vmopcode		opcode;
    vmarg			result;
    vmarg			arg1;
    vmarg			arg2;
    unsigned		srcLine;
};

/* Userfunc struct, used to store functions information */
struct userfunc {
    unsigned address;
    unsigned localSize;
    const char* id;
};

#endif /* avm type definitions */