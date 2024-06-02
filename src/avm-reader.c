#include <avm-reader.h>

#include <stdio.h>

/* Array to store user functions */
UserFunc_T userFuncs = (UserFunc_T)0;
unsigned total_userFuncs = 0;

/* Arrays to store Const variable information */
double *numConsts;
unsigned total_numConsts = 0;

char **stringConsts;
unsigned total_stringConsts = 0;

char **namedLibfuncs;
unsigned total_namedLibfuncs = 0;

void alpha_bin_reader(char* ist_name)
{


}