/*
 * errors.c
 *
 *  Created on: Aug 7, 2021
 *      Author: tamas
 */
#include "errors.h"

const char * ERRORS_error_to_string(ERRORS_return_t error)
{
	switch (error) {
	case ERRORS_not_implemented:
		return "Function not implemented";
	break;
	default:
		return "Error string not available";
	break;

	}
	return "";
}
