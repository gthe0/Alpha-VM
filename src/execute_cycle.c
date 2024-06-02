#include <dispatcher.h>

/* Variables used for execution */
unsigned pc = 0;
unsigned currLine = 0;
unsigned codeSize = 0;
unsigned executionFinished = 0;

Instruction_T code = (Instruction_T) 0;

void execute_cycle (void) {

    if (executionFinished) 
	{
        return;
    }
	else
	if (pc == AVM_ENDING_PC)
	{
        executionFinished = 1;
        return;
    }
	else
	{
        assert(pc < AVM_ENDING_PC);
    
	    Instruction_T instr = code + pc;
		 
		/* Check if the opcode is valid */
        assert(instr->opcode >= 0 && instr->opcode <= nop_v);

        if (instr->srcLine)
            currLine = instr->srcLine;

        unsigned oldPC = pc;

        (*executeFuncs[instr->opcode])(instr);

        if (pc == oldPC)
            ++pc;
    }

	return ;
}