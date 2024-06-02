/*
 	Authors: csd4881, csd4988, csd5038														
	
	avm-log.c										

	Implementation of the logging function
*/


#include <avm-log.h>

#include <stdio.h>
#include <stdarg.h>

extern unsigned executionFinished;

void avm_log(error_level_t error ,const char* msg,...)
{
	/* If we are on linux machines, use ansi escape sequences */
	#if ( __linux__)
		char* MAG 		= "\e[1;35m"; 	/*Warning == MAG*/
		char* RED		= "\e[1;31m";	/*Error == RED*/
		char* YEL		= "\e[1;33m";	/*Usage == YEL*/
		char* CYAN		= "\e[1;36m";	/*Note == CYN*/
		char* reset		= "\e[0m";		/*RESET*/
	#else
		char* MAG 		= ""
		char* RED		= ""
		char* YEL		= ""
		char* CYAN		= ""
		char* reset		= ""
	#endif
	
	fflush(stdout);

	switch (error)
	{
		case WARNING: fprintf(stderr,"%s[AVM] > WARNING: %s",MAG,reset); break;
		case ERROR: fprintf(stderr,"%s[AVM] > ERROR: %s",RED,reset); break;
		case USAGE: fprintf(stderr,"%s[AVM] > Usage: %s",YEL,reset); break;
		case NOTE: fprintf(stderr,"%s[AVM] > note: %s",CYAN,reset); break;
		default:;
	}

	/* Initialize arguments */
	va_list args;
	va_start(args, msg);

	vfprintf(stderr,msg,args);

	/* Free them */
	va_end(args);

	/* If any error is encountered, stop the execution */
	if (error = ERROR)	executionFinished = 1;

	return ;
}