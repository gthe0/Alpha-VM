/*
 	Authors: csd4881, csd4988, csd5038														
	
	dispatcher.h										

	Library used for 'dispatching' instructions.
	Here we declare the various execution functions
	and variables used in the execution cycle of the program...
*/

#ifndef DISPATCHER_H
#define DISPATCHER_H

#include <avm-types.h>

extern unsigned pc;
extern unsigned currLine;
extern unsigned executionFinished;

#define execute_add execute_arithmetic
#define execute_sub execute_arithmetic
#define execute_mul execute_arithmetic
#define execute_div execute_arithmetic
#define execute_mod execute_arithmetic

#define execute_jgt execute_transfer_logic
#define execute_jge execute_transfer_logic
#define execute_jlt execute_transfer_logic
#define execute_jle execute_transfer_logic

/* General execution instructions */
void execute_arithmetic(Instruction_T);
void execute_transfer_logic(Instruction_T);

/* Execute assign */
void execute_assign(Instruction_T);

/* Execute arithmetic */
void execute_add(Instruction_T);
void execute_sub(Instruction_T);
void execute_mul(Instruction_T);
void execute_div(Instruction_T);
void execute_mod(Instruction_T);
void execute_uminus(Instruction_T);

/* Execute boolean (stub functions) */
void execute_and(Instruction_T);
void execute_or(Instruction_T);
void execute_not(Instruction_T);

/* Execute logic transfer insturctions */
void execute_jump(Instruction_T);

void execute_jeq(Instruction_T);
void execute_jne(Instruction_T);
void execute_jgt(Instruction_T);
void execute_jge(Instruction_T);
void execute_jlt(Instruction_T);
void execute_jle(Instruction_T);

/* Execute function related stuff */
void execute_call(Instruction_T);
void execute_pusharg(Instruction_T);
void execute_funcenter(Instruction_T);
void execute_funcexit(Instruction_T);

/* Execute table related stuff */
void execute_newtable(Instruction_T);
void execute_tablegetelem(Instruction_T);
void execute_tablesetelem(Instruction_T);

/* Execute nop */
void execute_nop(Instruction_T);

/**
* @brief This Function defines the execution FSM of the AVM 
*/
void execute_cycle(void);

#endif /* dispatcher lib */