#ifndef DISPATCHER_H
#define DISPATCHER_H


#include <avm-types.h>

/* Execute assign */
void execute_assign(instruction*);

/* Execute arithmetic */
void execute_add(instruction*);
void execute_sub(instruction*);
void execute_mul(instruction*);
void execute_div(instruction*);
void execute_mod(instruction*);
void execute_uminus(instruction*);

/* Execute boolean (stub functions) */
void execute_and(instruction*);
void execute_or(instruction*);
void execute_not(instruction*);

/* Execute logic transfer insturctions */
void execute_jeq(instruction*);
void execute_jne(instruction*);
void execute_jle(instruction*);
void execute_jge(instruction*);
void execute_jlt(instruction*);
void execute_jgt(instruction*);

/* Execute function related stuff */
void execute_call(instruction*);
void execute_pusharg(instruction*);
void execute_funcenter(instruction*);
void execute_funcexit(instruction*);

/* Execute table related stuff */
void execute_newtable(instruction*);
void execute_tablegetelem(instruction*);
void execute_tablesetelem(instruction*);

/* Execute nop */
void execute_nop(instruction*);

#endif /* dispatcher lib */