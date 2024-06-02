/*
 	Authors: csd4881, csd4988, csd5038														
	
	avm-log.h										

	Function used for logging avm errors
*/

#ifndef AVM_LOG_H
#define AVM_LOG_H

typedef enum error_level_t error_level_t;

/* Enums for error levels*/
enum error_level_t {
	ERROR,
	WARNING,
	USAGE,
	NOTE
};

/**
* @brief A variadic function that logs the errors in stderr with colors
*
* @param error The error type of the message (WARNING, ERROR, NOTE, USAGE)
* @param msg The message to print
*/
void avm_log(error_level_t error ,const char* msg,...);

#endif /* avm log lib */
