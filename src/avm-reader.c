#include <avm-reader.h>
#include <avm-mem.h>
#include <avm-log.h>

#include <avm-utils.h>

#include <stdlib.h>
#include <assert.h>

#define AVM_BIN_MAGIC_NUM 3401334

/* Array to store the Instructions */
Instruction_T code = (Instruction_T)0;
unsigned codeSize = 0;

/* Array to store user functions */
UserFunc_T userFuncs = (UserFunc_T)0;
unsigned total_userFuncs = 0;

/* Arrays to store Const variable information */
double *numConsts = (double*)0;
unsigned total_numConsts = 0;

char **stringConsts = (char**)0;
unsigned total_stringConsts = 0;

char **namedLibfuncs = (char**)0;
unsigned total_namedLibfuncs = 0;

unsigned programVarOffset = 0;

/* Array getters */
userfunc_t get_UserFunc(unsigned int i)
{
	assert(userFuncs && i <= total_userFuncs);
	return userFuncs[i];
}

double get_numConst(unsigned int i)
{
	assert(numConsts && i <= total_numConsts);
	return numConsts[i];
}

char* get_strConsts(unsigned int i)
{
	assert(stringConsts && i <= total_stringConsts);
	return stringConsts[i];
}

char* get_libFuncs(unsigned int i)
{
	assert(namedLibfuncs && i <= total_namedLibfuncs);
	return namedLibfuncs[i];
}

/* Reads the binary file and puts inputs into the arrays */
void alpha_bin_reader(char* ist_name)
{
	unsigned magicNum;
	FILE *ist = NULL;

	if (!(ist = fopen(ist_name, "rb")))
	{
		avm_log(ERROR, "Could not open file stream %s\n", ist_name);
		exit(EXIT_FAILURE);
	}

	/* Read and compare the magic Number */
	fread(&magicNum, sizeof(unsigned),1,ist);

	/* Read the data segment offset */
	fread(&programVarOffset, sizeof(unsigned),1,ist);

	if(magicNum != AVM_BIN_MAGIC_NUM)
	{
		avm_log(ERROR,"Wrong binary file format\n");
		exit(EXIT_FAILURE);
	}

	/* Read the constant Numbers */
	fread(&total_numConsts, sizeof(unsigned),1,ist);

	numConsts = malloc(sizeof(double)*total_numConsts);
	fread(numConsts,sizeof(double),total_numConsts,ist);


	/* Read the constant Strings */
	fread(&total_stringConsts, sizeof(unsigned),1,ist);
	stringConsts =  malloc(sizeof(char*)*total_stringConsts);

	for (int i = 0; i < total_stringConsts; i++)
	{
		unsigned length;

		fread(&length, sizeof(length),1,ist);

		stringConsts[i] = malloc(sizeof(char)*(length+1));
		fread(stringConsts[i],length*sizeof(char),1,ist);

		stringConsts[i][length] = '\0';
	}

	/* Read the total numbers of Library Functions */
	fread(&total_namedLibfuncs, sizeof(unsigned),1,ist);
	namedLibfuncs =  malloc(sizeof(char*)*total_namedLibfuncs);
	
	for (int i = 0; i < total_namedLibfuncs; i++)
	{
		unsigned length;

		fread(&length, sizeof(length),1,ist);

		namedLibfuncs[i] = malloc(sizeof(char)*(length+1));
		fread(namedLibfuncs[i],length*sizeof(char),1,ist);

		namedLibfuncs[i][length] = '\0';
	}

	/* Read the user defined functions */
	fread(&total_userFuncs, sizeof(unsigned),1,ist);
	userFuncs = malloc(sizeof(userfunc_t)*total_userFuncs);

	for (int i = 0; i < total_userFuncs; i++)
	{
		/* Read each field separately... */
		fread(&(userFuncs[i].address),sizeof(unsigned),1,ist);
		fread(&(userFuncs[i].localSize),sizeof(unsigned),1,ist);

		/*
		 Read the id, but because it is constant,
		 first read it in a regular string to add
		 the null terminator at the very end '\0'
		*/
		unsigned length = 0;
		fread(&length,sizeof(unsigned),1,ist);

		char* str =  malloc(sizeof(char)*(length+1));
		fread(str,length*sizeof(char),1,ist);

		str[length] = '\0';
		userFuncs[i].id = str;
	}

	/* Read Instructions */
	fread(&codeSize,sizeof(unsigned),1,ist);

	code = malloc(sizeof(instruction)*codeSize);
	fread(code,sizeof(instruction),codeSize,ist);

	fclose(ist);
	return;
}

/***************************************************************/


/* Number of OUTLINE_CHARs to be printend */
#define OUTLINE_PRINT_NUM 50
#define OUTLINE_CHAR "="

#define OUTLINE_FUNC(a)                         \
	for (int i = 0; i < OUTLINE_PRINT_NUM; i++) \
	{                                           \
		fprintf(ost, OUTLINE_CHAR);             \
		if (i == OUTLINE_PRINT_NUM / 2)         \
			fprintf(ost, a);                    \
	}                                           \
	fprintf(ost, "\n");

/* Function used to prin the numConst array */
static void print_numConsts(FILE *ost)
{
	/* Print a stylized outline for the array */
	OUTLINE_FUNC(" CONSTANT NUMBERS ");

	for (int i = 0; i < total_numConsts; i++)
		fprintf(ost, "#%-3u %lf\n", i, numConsts[i]);

	return;
}

static void print_stringConsts(FILE *ost)
{
	/* Print a stylized outline for the array */
	OUTLINE_FUNC(" CONSTANT STRINGS ");

	for (int i = 0; i < total_stringConsts; i++)
		fprintf(ost, "#%-3u %s\n", i, stringConsts[i]);

	return;
}

static void print_namedLibfuncs(FILE *ost)
{
	/* Print a stylized outline for the array */
	OUTLINE_FUNC(" LIBRARY FUNCTIONS ");

	for (int i = 0; i < total_namedLibfuncs; i++)
		fprintf(ost, "#%-3u %s\n", i, namedLibfuncs[i]);

	return;
}

static void print_userFuncs(FILE *ost)
{
	OUTLINE_FUNC(" USER FUNCTIONS ");

	for (int i = 0; i < total_userFuncs; i++)
		fprintf(ost, "#%-3u, address: %-3u, "
					 "total locals: %-3u, "
					 "id: %s\n",
				i, userFuncs[i].address, userFuncs[i].localSize, userFuncs[i].id);

	return;
}

/* This function here is used to output an arg's info*/
static void write_arg(FILE* ost,vmarg arg)
{
	/* If the argument is a label == 0 with a 0 value, then do not print it*/
	switch (arg.type)
	{
    	case label_a:
			fprintf(ost,"%u%-12s %-3u ",(unsigned)arg.type,"(label),",arg.val);
			break;
    	case global_a:
			fprintf(ost,"%u%-12s %-3u ",(unsigned)arg.type,"(global),",arg.val);
			break; 
    	case formal_a:
			fprintf(ost,"%u%-12s %-3u ",(unsigned)arg.type,"(formal),",arg.val);
			break;
    	case local_a:
			fprintf(ost,"%u%-12s %-3u ",(unsigned)arg.type,"(local),",arg.val);
			break;
    	case number_a:
			fprintf(ost,"%u%-12s %-3u ",(unsigned)arg.type,"(num),",arg.val);
			break;
    	case string_a:
			fprintf(ost,"%u%-12s %-3u ",(unsigned)arg.type,"(string),",arg.val);
			break;
    	case bool_a:
			fprintf(ost,"%u%-12s %-3u ",(unsigned)arg.type,"(bool),",arg.val);
			break;
    	case nil_a:
			fprintf(ost,"%u%-12s %-3u ",(unsigned)arg.type,"(nil),",arg.val);
			break;
    	case userfunc_a:
			fprintf(ost,"%u%-12s %-3u ",(unsigned)arg.type,"(userfunc),",arg.val);
			break;
    	case libfunc_a:
			fprintf(ost,"%u%-12s %-3u ",(unsigned)arg.type,"(libfunc),",arg.val);
			break;
    	case retval_a:
			fprintf(ost,"%u%-12s %-3u ",(unsigned)arg.type,"(retval),",arg.val);
			break;
		default:
			break;
	}
	
	return ;
}

/* Helper function to write an instrucion to the ost stream */
static void write_instr_i(FILE *ost, unsigned i)
{
	fprintf(ost,"#%-3u ",i);

	switch (code[i].opcode)
	{
		case assign_v:
			fprintf(ost,"%-15s","assign");
			break;
		/* Airthmetic opcodes */
		case add_v:
			fprintf(ost,"%-15s","add");
			break;
		case sub_v:
			fprintf(ost,"%-15s","sub");
			break;
		case mul_v:
			fprintf(ost,"%-15s","mul");
			break;
		case div_v:
			fprintf(ost,"%-15s","div");
			break;
		case mod_v:
			fprintf(ost,"%-15s","mod");
			break;
		case uminus_v:
			fprintf(ost,"%-15s","uminus");
			break;
	
		/* Jump opcode */
		case jump_v:
			fprintf(ost,"%-15s","jump");
			break;
	
		/* Branch opcodes */
		case jeq_v:
			fprintf(ost,"%-15s","jeq");
			break;
		case jne_v:
			fprintf(ost,"%-15s","jne");
			break;
		case jgt_v:
			fprintf(ost,"%-15s","jgt");
			break;
		case jge_v:
			fprintf(ost,"%-15s","jge");
			break;
		case jlt_v:
			fprintf(ost,"%-15s","jlt");
			break;
		case jle_v:
			fprintf(ost,"%-15s","jle");
			break;
	
		/* call related opcodes */
		case call_v:
			fprintf(ost,"%-15s","call");
			break;
		case pusharg_v:
			fprintf(ost,"%-15s","pusharg");
			break;
	
		/* function defintion related opcode */
		case funcenter_v:
			fprintf(ost,"%-15s","funcenter");
			break;
		case funcexit_v:
			fprintf(ost,"%-15s","funcexit");
			break;
	
		/* Boolean expression related opcode */
		case or_v:
			fprintf(ost,"%-15s","or");
			break;
		case and_v:
			fprintf(ost,"%-15s","and");
			break;
		case not_v:
			fprintf(ost,"%-15s","not");
			break;
	
		/* Table related opcodes */
		case newtable_v:
			fprintf(ost,"%-15s","newtable");
			break;
		case tablegetelem_v:
			fprintf(ost,"%-15s","tablegetelem");
			break;
		case tablesetelem_v:
			fprintf(ost,"%-15s","tablesetelem");
			break;
	
		/* NOP */
		case nop_v:
			fprintf(ost,"%-15s","nop");
			break;
	
		default:
			break;
	}

	/* Print the argument values */
	write_arg(ost,code[i].result);
	write_arg(ost,code[i].arg1);
	write_arg(ost,code[i].arg2);
	
	fprintf(ost,"\n");

	return ;	
}

static void print_instructions(FILE *ost)
{
	OUTLINE_FUNC(" INSTRUCTIONS ");
	fprintf(ost, "\n");


	for (int i = 0; i < codeSize; i++)
	{
		write_instr_i(ost,i);
	}

	return;
}

/* Print all the tcg buffers */
void print_tcg_arrays(FILE *ost)
{
	if (!ost)
		return;

	print_numConsts(ost);
	print_stringConsts(ost);
	print_namedLibfuncs(ost);
	print_userFuncs(ost);

	fprintf(ost, "\n");

	print_instructions(ost);
}
