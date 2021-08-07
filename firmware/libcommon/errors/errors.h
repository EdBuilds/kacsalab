/*
 * errors.h
 *
 *  Created on: Aug 5, 2021
 *      Author: tamas
 */

#ifndef ERRORS_ERRORS_H_
#define ERRORS_ERRORS_H_

typedef enum {
	ERRORS_ok = 0,
	ERRORS_nullptr,
	ERRORS_argument_error,
	ERRORS_resource_busy,
	ERRORS_not_enough_memory,
	ERRORS_not_available,
	ERRORS_generic_error,
	ERRORS_os_error,
	ERRORS_not_implemented,
}ERRORS_return_t;

const char * ERRORS_error_to_string(ERRORS_return_t error);


#endif /* ERRORS_ERRORS_H_ */
