#include <avm-reader.h>

#include <stdio.h>
#include <assert.h>

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


}